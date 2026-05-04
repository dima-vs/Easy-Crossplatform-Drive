#include "file_controller.h"
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <QUrlQuery>
#include <QHttpHeaders>
#include <QVariant>
#include <QByteArray>
#include <QRegularExpression>
#include <QRegularExpressionMatch>
#include <QtTypes>

#include "serialization/file/json.h"

namespace Api::Controllers
{

FileController::FileController(
    QHttpServer& server,
    Service::File::FileService& fileService,
    Service::Auth::AuthService& authService
    )
    : m_server(server),
    m_fileService(fileService),
    m_authService(authService)
{
}

void FileController::registerEndpoints()
{
    m_server.route(
        "/files",
        QHttpServerRequest::Method::Get,
        [this](const QHttpServerRequest& request) {
            return this->getTree(request);
        }
        );

    m_server.route(
        "/uploads",
        QHttpServerRequest::Method::Post,
        [this](const QHttpServerRequest& request) {
            return this->initUpload(request);
        }
        );

    m_server.route(
        "/uploads/<arg>",
        QHttpServerRequest::Method::Post,
        [this](const QString& uploadId, const QHttpServerRequest& request)
        {
            return this->upload(uploadId, request);
        }
        );

    m_server.route(
        "/uploads/<arg>/complete",
        QHttpServerRequest::Method::Post,
        [this](const QString& uploadId, const QHttpServerRequest& request)
        {
            return this->completeUpload(uploadId, request);
        }
        );

    m_server.route(
        "/files/<arg>",
        QHttpServerRequest::Method::Get,
        [this](int fileId, const QHttpServerRequest& request)
        {
            return this->download(fileId, request);
        }
        );

    m_server.route(
        "/files",
        QHttpServerRequest::Method::Post,
        [this](const QHttpServerRequest& request) {
            return this->createEmpty(request);
        }
        );

    m_server.route(
        "/files/<arg>",
        QHttpServerRequest::Method::Delete,
        [this](int fileId, const QHttpServerRequest& request)
        {
            return this->remove(fileId, request);
        }
        );

    m_server.route(
        "/files/<arg>",
        QHttpServerRequest::Method::Patch,
        [this](int fileId, const QHttpServerRequest& request)
        {
            return this->rename(fileId, request);
        }
        );
}

QHttpServerResponse FileController::getTree(
    const QHttpServerRequest& request
    )
{
    auto authResultOpt = authenticate(request);
    if (!authResultOpt.has_value())
    {
        return buildErrorResponse(
            "Missing or invalid credentials",
            QHttpServerResponder::StatusCode::Unauthorized
            );
    }
    ServiceModel::Auth::Result authResult = authResultOpt.value();

    QUrlQuery query = request.query();
    QVariant parentId;
    QVariant maxDepth;

    if (query.hasQueryItem("parentId"))
    {
        bool ok;
        int id = query.queryItemValue("parentId").toInt(&ok);
        if (ok) parentId = id;
    }
    if (query.hasQueryItem("maxDepth"))
    {
        bool ok;
        int id = query.queryItemValue("maxDepth").toInt(&ok);
        if (ok) maxDepth = id;
    }

    auto treeResult = m_fileService.getTree(
        authResult.userId,
        parentId,
        maxDepth
        );

    if (!treeResult.isOk())
    {
        return buildErrorResponse(treeResult.error());
    }

    // form response
    auto responseDtoList = Api::Converters::FileModelsConverter
        ::fileNodeListToDTO(treeResult.data());

    QJsonArray responseJsonArray = Serialization::File::toJson(
        responseDtoList);

    return QHttpServerResponse(
        responseJsonArray,
        QHttpServerResponder::StatusCode::Ok
        );
}

QHttpServerResponse FileController::initUpload(
    const QHttpServerRequest& request
    )
{
    auto authResultOpt = authenticate(request);
    if (!authResultOpt.has_value())
    {
        return buildErrorResponse(
            "Missing or invalid credentials",
            QHttpServerResponder::StatusCode::Unauthorized
            );
    }
    ServiceModel::Auth::Result authResult = authResultOpt.value();

    QJsonParseError parseError;
    QJsonDocument jsonDoc = QJsonDocument::fromJson(
        request.body(),
        &parseError
        );

    if ((parseError.error != QJsonParseError::NoError) ||
        !jsonDoc.isObject())
    {
        return buildBadRequestErrorResponse("Invalid JSON format");
    }

    // JSON -> DTO
    auto requestDtoOpt = Serialization::File::
        fromJsonUploadInitRequest(jsonDoc.object());
    if (!requestDtoOpt.has_value())
    {
        return buildBadRequestErrorResponse("Missing or invalid fields in request");
    }
    const auto& requestDto = requestDtoOpt.value();

    // call service
    auto serviceResult = m_fileService.initUpload(
        authResult.userId,
        authResult.userName,
        requestDto.fileName,
        requestDto.parentId.has_value() ?
            QVariant(requestDto.parentId.value()) :
            QVariant(),
        requestDto.fileSize,
        requestDto.overwrite
        );

    if (!serviceResult.isOk())
    {
        return buildErrorResponse(serviceResult.error());
    }

    // form response
    DTO::File::UploadInitResponse responseDto = Api::Converters::
        FileModelsConverter::initUploadResToDTO(serviceResult.data());

    QJsonObject responseJson = Serialization::File::toJson(responseDto);
    return QHttpServerResponse(
        responseJson,
        QHttpServerResponder::StatusCode::Ok
        );
}

QHttpServerResponse FileController::upload(
    const QString &uploadId,
    const QHttpServerRequest& request
)
{
    auto authResultOpt = authenticate(request);
    if (!authResultOpt.has_value())
    {
        return buildErrorResponse(
            "Missing or invalid credentials",
            QHttpServerResponder::StatusCode::Unauthorized
            );
    }
    ServiceModel::Auth::Result authResult = authResultOpt.value();

    auto contentRangeOpt = parseContentRangeHeader(request);
    if (!contentRangeOpt.has_value())
        return  buildBadRequestErrorResponse("Missing or invalid Content-Range header");

    ServiceModel::File::ContentRange contentRange =
        contentRangeOpt.value();

    QByteArray chunk = request.body();

    // call service
    auto serviceResult = m_fileService.upload(
        uploadId,
        contentRange,
        chunk
        );

    if (!serviceResult.isOk())
    {
        return buildErrorResponse(serviceResult.error());
    }

    return QHttpServerResponse(
        QHttpServerResponder::StatusCode::NoContent
        );
}

QHttpServerResponse FileController::completeUpload(
    const QString& uploadId,
    const QHttpServerRequest& request
    )
{
    auto authResultOpt = authenticate(request);
    if (!authResultOpt.has_value())
    {
        return buildErrorResponse(
            "Missing or invalid credentials",
            QHttpServerResponder::StatusCode::Unauthorized
            );
    }
    ServiceModel::Auth::Result authResult = authResultOpt.value();

    // call service
    auto serviceResult = m_fileService.completeUpload(
        uploadId
        );

    if (!serviceResult.isOk())
    {
        return buildErrorResponse(serviceResult.error());
    }

    // form response
    DTO::File::UploadCompleteResponse responseDto = Api::Converters::
        FileModelsConverter::completeUploadResToDTO(serviceResult.data());

    QJsonObject responseJson = Serialization::File::toJson(responseDto);
    return QHttpServerResponse(
        responseJson,
        QHttpServerResponder::StatusCode::Created
        );
}

QHttpServerResponse FileController::download(
    int id,
    const QHttpServerRequest& request
    )
{
    auto authResultOpt = authenticate(request);
    if (!authResultOpt.has_value())
    {
        return buildErrorResponse(
            "Missing or invalid credentials",
            QHttpServerResponder::StatusCode::Unauthorized
            );
    }
    ServiceModel::Auth::Result authResult = authResultOpt.value();

    auto rangeOpt = parseRangeHeader(request);
    if (!rangeOpt.has_value())
        return  buildBadRequestErrorResponse("Missing or invalid Range header");

    ServiceModel::File::RequestedRange range =
        rangeOpt.value();

    QByteArray chunk;

    // call service
    auto serviceResult = m_fileService.download(
        authResult.userId,
        id,
        range,
        chunk
        );

    if (!serviceResult.isOk())
    {
        return buildErrorResponse(serviceResult.error());
    }

    // form response
    QHttpServerResponse httpResponse(
        chunk,
        QHttpServerResponder::StatusCode::PartialContent
        );

    QHttpHeaders headers;
    headers.append(
        QHttpHeaders::WellKnownHeader::ContentRange,
        buildContentRangeHeader(serviceResult.data())
        );
    httpResponse.setHeaders(headers);

    return httpResponse;
}

QHttpServerResponse FileController::createEmpty(const QHttpServerRequest& request)
{
    auto authResultOpt = authenticate(request);
    if (!authResultOpt.has_value())
    {
        return buildErrorResponse(
            "Missing or invalid credentials",
            QHttpServerResponder::StatusCode::Unauthorized
            );
    }
    ServiceModel::Auth::Result authResult = authResultOpt.value();

    QJsonParseError parseError;
    QJsonDocument jsonDoc = QJsonDocument::fromJson(
        request.body(),
        &parseError
        );

    if ((parseError.error != QJsonParseError::NoError) ||
        !jsonDoc.isObject())
    {
        return buildBadRequestErrorResponse("Invalid JSON format");
    }

    // JSON -> DTO
    auto requestDtoOpt = Serialization::File::
        fromJsonCreateEmptyRequest(jsonDoc.object());
    if (!requestDtoOpt.has_value())
    {
        return buildBadRequestErrorResponse("Missing or invalid fields in request");
    }
    const auto& requestDto = requestDtoOpt.value();

    // call service
    auto serviceResult = m_fileService.createEmpty(
        authResult.userId,
        authResult.userName,
        requestDto.fileName,
        requestDto.parentId.has_value() ?
            QVariant(requestDto.parentId.value()) :
            QVariant(),
        requestDto.type,
        requestDto.overwrite
        );

    if (!serviceResult.isOk())
    {
        return buildErrorResponse(serviceResult.error());
    }

    // form response
    DTO::File::CreateEmptyResponse responseDto = Api::Converters::
        FileModelsConverter::createEmptyResToDTO(serviceResult.data());

    QJsonObject responseJson = Serialization::File::toJson(responseDto);
    return QHttpServerResponse(
        responseJson,
        QHttpServerResponder::StatusCode::Created
        );
}

QHttpServerResponse FileController::remove(
    int fileId,
    const QHttpServerRequest& request
    )
{
    auto authResultOpt = authenticate(request);
    if (!authResultOpt.has_value())
    {
        return buildErrorResponse(
            "Missing or invalid credentials",
            QHttpServerResponder::StatusCode::Unauthorized
            );
    }
    ServiceModel::Auth::Result authResult = authResultOpt.value();

    // call service
    auto serviceResult = m_fileService.removeEntry(
        authResult.userId,
        fileId
        );

    if (!serviceResult.isOk())
    {
        return buildErrorResponse(serviceResult.error());
    }

    return QHttpServerResponse(
        QHttpServerResponder::StatusCode::NoContent);
}

QHttpServerResponse FileController::rename(
    int fileId,
    const QHttpServerRequest& request
    )
{
    auto authResultOpt = authenticate(request);
    if (!authResultOpt.has_value())
    {
        return buildErrorResponse(
            "Missing or invalid credentials",
            QHttpServerResponder::StatusCode::Unauthorized
            );
    }
    ServiceModel::Auth::Result authResult = authResultOpt.value();

    QJsonParseError parseError;
    QJsonDocument jsonDoc = QJsonDocument::fromJson(
        request.body(),
        &parseError
        );

    if ((parseError.error != QJsonParseError::NoError) ||
        !jsonDoc.isObject())
    {
        return buildBadRequestErrorResponse("Invalid JSON format");
    }

    // JSON -> DTO
    auto requestDtoOpt = Serialization::File::
        fromJsonRenameRequest(jsonDoc.object());
    if (!requestDtoOpt.has_value())
    {
        return buildBadRequestErrorResponse("Missing or invalid fields in request");
    }
    const auto& requestDto = requestDtoOpt.value();

    // call service
    auto serviceResult = m_fileService.renameAndMove(
        authResult.userId,
        fileId,
        requestDto.newParentId,
        requestDto.newFileName
        );

    if (!serviceResult.isOk())
    {
        return buildErrorResponse(serviceResult.error());
    }

    // form response
    DTO::File::RenameResponse responseDto = Api::Converters::
        FileModelsConverter::renameResToDTO(serviceResult.data());

    QJsonObject responseJson = Serialization::File::toJson(responseDto);
    return QHttpServerResponse(
        responseJson,
        QHttpServerResponder::StatusCode::Ok
        );
}

QString FileController::buildContentRangeHeader(
    const ServiceModel::File::ContentRange& contentRange
    ) const
{
    return QString("Bytes %1-%2/%3")
        .arg(contentRange.startByte)
        .arg(contentRange.endByte)
        .arg(contentRange.totalBytes);
}

std::optional<ServiceModel::File::ContentRange>
    FileController::parseContentRangeHeader(
        const QHttpServerRequest& request
    ) const
{
    auto headers = request.headers();
    if (!headers.contains("Content-Range"))
    {
        return std::nullopt;
    }

    QString contentRange = headers.value("Content-Range")
                               .toByteArray();

    QRegularExpression re(
        "^Bytes (?<start>\\d+)-(?<end>\\d+)/(?<total>\\d+)$",
        QRegularExpression::CaseInsensitiveOption
        );
    QRegularExpressionMatch match = re.match(contentRange);
    if (!match.hasMatch())
    {
        return std::nullopt;
    }

    qint64 start = match.captured("start").toLongLong();
    qint64 end = match.captured("end").toLongLong();
    qint64 total = match.captured("total").toLongLong();

    ServiceModel::File::ContentRange result;
    result.startByte = start;
    result.endByte = end;
    result.totalBytes = total;
    return result;
}

std::optional<ServiceModel::File::RequestedRange> FileController::parseRangeHeader(
    const QHttpServerRequest& request
    ) const
{
    auto headers = request.headers();
    if (!headers.contains("Range"))
    {
        return std::nullopt;
    }

    QString range = headers.value("Range").toByteArray();

    QRegularExpression re(
        "^Bytes=(?<start>\\d+)-(?<end>\\d+)$",
        QRegularExpression::CaseInsensitiveOption
        );
    QRegularExpressionMatch match = re.match(range);
    if (!match.hasMatch())
    {
        return std::nullopt;
    }

    qint64 start = match.captured("start").toLongLong();
    qint64 end = match.captured("end").toLongLong();

    ServiceModel::File::RequestedRange result;
    result.startByte = start;
    result.endByte = end;
    return result;
}

std::optional<ServiceModel::Auth::Result> FileController::authenticate(
    const QHttpServerRequest& request
    )
{
    auto headers = request.headers();
    if (!headers.contains("Authorization"))
    {
        return std::nullopt;
    }

    QString authHeader = headers.value("Authorization")
                             .toByteArray();

    // check format "Bearer <token>"
    if (!authHeader.startsWith("Bearer ", Qt::CaseInsensitive))
    {
        return std::nullopt;
    }

    // extract token part
    QString token = authHeader.mid(7).trimmed();
    if (token.isEmpty())
    {
        return std::nullopt;
    }

    auto authResult = m_authService.authenticateByToken(token);
    if (!authResult.isOk())
    {
        return std::nullopt;
    }

    return authResult.data();
}

QHttpServerResponse FileController::buildErrorResponse(
    ErrorCode::File::ServiceError error
    )
{
    using ServiceError = ErrorCode::File::ServiceError;
    using StatusCode = QHttpServerResponder::StatusCode;

    QString errorMsg;
    StatusCode statusCode = StatusCode::BadRequest;

    switch (error)
    {
    case ServiceError::FileNotFound:
    case ServiceError::SessionDoesNotExists:
        errorMsg = "File or session not found";
        statusCode = StatusCode::NotFound;
        break;

    case ServiceError::PermissionDenied:
        errorMsg = "Access denied";
        statusCode = StatusCode::Forbidden;
        break;

    case ServiceError::FileAlreadyExist:
        errorMsg = "File or directory already exists";
        statusCode = StatusCode::Conflict;
        break;

    case ServiceError::FileTooLarge:
    case ServiceError::ChunkTooLarge:
        errorMsg = "Payload too large";
        statusCode = StatusCode::PayloadTooLarge;
        break;

    case ServiceError::FailedToPerformDBOperation:
    case ServiceError::FailedToPerformStorageOperation:
        errorMsg = "Internal server storage error";
        statusCode = StatusCode::InternalServerError;
        break;

    default:
        errorMsg = "Invalid request or file operation failed";
        statusCode = StatusCode::BadRequest;
        break;
    }

    return buildErrorResponse(errorMsg, statusCode);
}

QHttpServerResponse FileController::buildBadRequestErrorResponse(
    const QString& errorMsg
    )
{
    return buildErrorResponse(
        errorMsg,
        QHttpServerResponder::StatusCode::BadRequest
        );
}

QHttpServerResponse FileController::buildErrorResponse(
    const QString& errorMsg,
    QHttpServerResponder::StatusCode statusCode
    )
{
    QJsonObject errorJson;
    errorJson["error"] = errorMsg;
    return QHttpServerResponse(errorJson, statusCode);
}

} // namespace Api::Controllers
