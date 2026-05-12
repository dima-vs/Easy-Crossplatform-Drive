#include "model.h"
#include <QDebug>

bool Model::authorize(QHttpHeaders &headersOut)
{
    if (token.isEmpty())
        return false;
    headersOut.append(QHttpHeaders::WellKnownHeader::Authorization, QString("Bearer %1").arg(token));
    return true;
}

void Model::uploadData(QString uploadId, QByteArray chunkData, qint64 startByte, qint64 endByte, qint64 totalBytes)
{
    qDebug() << "Enter uploadData";
    baseUrl = "http://" + host.toString() + ":8080";
    QUrl url(baseUrl + "/uploads/" + uploadId);
    QNetworkRequest httprequest(url);
    QHttpHeaders headers;
    headers.append(QHttpHeaders::WellKnownHeader::ContentType, "application/octet-stream");
    headers.append(QHttpHeaders::WellKnownHeader::ContentRange, QString("bytes %1-%2/%3").arg(startByte).arg(endByte).arg(totalBytes));
    authorize(headers);
    httprequest.setHeaders(headers);
    QNetworkReply *reply = manager.post(httprequest, chunkData);

    QObject::connect(reply, &QNetworkReply::finished, [reply, this]()
    {
        baseUrl.clear();
        CommonTypes::NoData nodata;
        if (reply->error() == QNetworkReply::NoError)
        {
            std::optional<CommonTypes::NoData> response = nodata;
            emit uploadFinished(response);
        }
        else
        {
            qDebug() << "Error:" << reply->errorString();
            emit uploadFinished(std::nullopt);
        }
        reply->deleteLater();
    });
}

void Model::uploadInit(QString fileName, std::optional<int> parentId, qint64 fileSize, bool overwrite)
{
    qDebug() << "Enter uploadInit";
    DTO::File::UploadInitRequest request;
    request.fileName = fileName;
    request.fileSize = fileSize;
    request.parentId = parentId;
    request.overwrite = overwrite;

    QJsonObject jsonreq = Serialization::File::toJson(request);
    QByteArray data = QJsonDocument(jsonreq).toJson();

    baseUrl = "http://" + host.toString() + ":8080";
    QUrl url(baseUrl + "/uploads");
    QNetworkRequest httprequest(url);
    QHttpHeaders headers;
    headers.append(QHttpHeaders::WellKnownHeader::ContentType, "application/json");
    authorize(headers);
    httprequest.setHeaders(headers);
    QNetworkReply *reply = manager.post(httprequest, data);

    QObject::connect(reply, &QNetworkReply::finished, [reply, this]()
    {
        baseUrl.clear();
        if (reply->error() == QNetworkReply::NoError)
        {
            std::optional<DTO::File::UploadInitResponse> response;

            QByteArray responseData = reply->readAll();

            QJsonObject jsonresp = QJsonDocument::fromJson(responseData).object();

            response = Serialization::File::fromJsonUploadInitResponse(jsonresp);
            qDebug() << "Success:" << responseData;
            emit uploadInitFinished(response);
        }
        else
        {
            qDebug() << "Error:" << reply->errorString();
            emit uploadInitFinished(std::nullopt);
        }
        reply->deleteLater();
    });
}

void Model::downloadData(int fileId, qint64 startByte, qint64 endByte)
{
    baseUrl = "http://" + host.toString() + ":8080";
    QUrl url(baseUrl + "/files/" + QString::number(fileId));
    QNetworkRequest httprequest(url);
    QHttpHeaders headers;
    headers.append(QHttpHeaders::WellKnownHeader::Range, QString("Bytes=%1-%2").arg(startByte).arg(endByte));
    authorize(headers);
    httprequest.setHeaders(headers);
    QNetworkReply *reply = manager.get(httprequest);

    QObject::connect(reply, &QNetworkReply::finished, [reply, this]()
    {
        baseUrl.clear();
        if (reply->error() == QNetworkReply::NoError)
        {
            std::optional<QByteArray> response;

            response = reply->readAll();

            qDebug() << "Success";
            emit downloadChunkFinished(response);
        }
        else
        {
            qDebug() << "Error:" << reply->errorString();
            emit downloadChunkFinished(std::nullopt);
        }
        reply->deleteLater();
        });
    }

void Model::completeUpload(QString uploadId)
{
    qDebug() << "Enter completeUpload";
    baseUrl = "http://" + host.toString() + ":8080";
    QUrl url(baseUrl + "/uploads/" + uploadId + "/complete");

    QNetworkRequest httprequest(url);
    QHttpHeaders headers;
    authorize(headers);
    httprequest.setHeaders(headers);
    QNetworkReply *reply = manager.sendCustomRequest(httprequest, "POST");

    QObject::connect(reply, &QNetworkReply::finished, [reply, this]()
    {
        baseUrl.clear();
        if (reply->error() == QNetworkReply::NoError)
        {
            std::optional<DTO::File::UploadCompleteResponse> response;

            QByteArray responseData = reply->readAll();

            QJsonObject jsonresp = QJsonDocument::fromJson(responseData).object();

            response = Serialization::File::fromJsonUploadCompleteResponse(jsonresp);
            qDebug() << "Success:" << responseData;
            emit uploadCompleteFinished(response);
        }
        else
        {
            qDebug() << "Error:" << reply->errorString();
            emit uploadCompleteFinished(std::nullopt);
        }
        reply->deleteLater();
    });
}

void Model::createFolder(QString fileName, std::optional<int> parentId, bool overwrite)
{
    DTO::File::CreateEmptyRequest request;
    request.fileName = fileName;
    request.overwrite = overwrite;
    request.parentId = parentId;
    request.type = Common::Domain::FileType::Directory;
    QJsonObject jsonreq = Serialization::File::toJson(request);
    QByteArray data = QJsonDocument(jsonreq).toJson();

    baseUrl = "http://" + host.toString() + ":8080";
    QUrl url(baseUrl + "/files");
    QNetworkRequest httprequest(url);
    QHttpHeaders headers;
    headers.append(QHttpHeaders::WellKnownHeader::ContentType, "application/json");
    authorize(headers);
    httprequest.setHeaders(headers);

    QNetworkReply *reply = manager.post(httprequest, data);
    QObject::connect(reply, &QNetworkReply::finished, [reply, this]()
    {
        baseUrl.clear();
        if (reply->error() == QNetworkReply::NoError)
        {
            std::optional<DTO::File::CreateEmptyResponse> response;
            QByteArray responseData = reply->readAll();

            QJsonObject jsonresp = QJsonDocument::fromJson(responseData).object();
            response = Serialization::File::fromJsonCreateEmptyResponse(jsonresp);

            emit createFolderFinished(response);
        }
        else
        {
            qDebug() << "Error:" << reply->errorString();
            emit createFolderFinished(std::nullopt);
        }
        reply->deleteLater();
    });
}

void Model::getSavedToken(QString accessToken)
{
    token = accessToken;
}

void Model::requestDeletion(int fileId)
{
    baseUrl = "http://" + host.toString() + ":8080";
    QUrl url(baseUrl + "/files/" + QString::number(fileId));
    QNetworkRequest httprequest(url);
    QHttpHeaders headers;
    headers.append(QHttpHeaders::WellKnownHeader::ContentType, "application/json");
    authorize(headers);
    httprequest.setHeaders(headers);
    QNetworkReply *reply = manager.deleteResource(httprequest);

    QObject::connect(reply, &QNetworkReply::finished, [reply, this]()
    {
        baseUrl.clear();
        CommonTypes::NoData nodata;
        if (reply->error() == QNetworkReply::NoError)
        {
            std::optional<CommonTypes::NoData> response = nodata;
            emit deletionFinished(response);
        }
        else
        {
            qDebug() << "Error:" << reply->errorString();
            emit deletionFinished(std::nullopt);
        }
        reply->deleteLater();
    });
}

void Model::renameFile(int fileId, std::optional<int> parentId, std::optional<QString> name)
{
    DTO::File::RenameRequest request;

    request.newFileName = name;

    if (parentId.has_value())
    {
        if (parentId.value() == -1)
        {
            request.newParentId = QVariant();
        }
        else
        {
            request.newParentId = QVariant(parentId.value());
        }
    }
    else
    {
        request.newParentId = std::nullopt;
    }

    QJsonObject jsonreq = Serialization::File::toJson(request);
    QByteArray data = QJsonDocument(jsonreq).toJson();

    baseUrl = "http://" + host.toString() + ":8080";
    QUrl url(baseUrl + "/files/" + QString::number(fileId));
    QNetworkRequest httprequest(url);
    QHttpHeaders headers;
    headers.append(QHttpHeaders::WellKnownHeader::ContentType, "application/json");
    authorize(headers);
    httprequest.setHeaders(headers);
    QNetworkReply *reply = manager.sendCustomRequest(httprequest, "PATCH", data);

    QObject::connect(reply, &QNetworkReply::finished, [reply, this]()
    {
        baseUrl.clear();
        if (reply->error() == QNetworkReply::NoError)
        {
            std::optional<DTO::File::RenameResponse> response;

            QByteArray responseData = reply->readAll();

            QJsonObject jsonresp = QJsonDocument::fromJson(responseData).object();

            response = Serialization::File::fromJsonRenameResponse(jsonresp);
            qDebug() << "Success:" << responseData;
            emit renameFinished(response);
        }
        else
        {
            qDebug() << "Error:" << reply->errorString();
            emit renameFinished(std::nullopt);
        }
        reply->deleteLater();
    });
}

void Model::requestFileTree()
{
    // QList<DTO::File::TreeNodeResponse> result;
    // DTO::File::TreeNodeResponse rootdir;
    // rootdir.fileId = 1;
    // rootdir.isDirectory = true;
    // rootdir.name = "ROOT";

    // DTO::File::TreeNodeResponse child;
    // child.fileId = 2;
    // child.isDirectory = false;
    // child.name = "file.doc";
    // child.size = 123;

    // rootdir.children = QList<DTO::File::TreeNodeResponse>();
    // rootdir.children->append(child);

    // result.append(rootdir);

    int depth = -1;
    int parentId = -1;

    baseUrl = "http://" + host.toString() + ":8080";
    QUrl url(baseUrl + "/files");
    QUrlQuery query;

    if (depth != -1)
    {
        query.addQueryItem("maxDepth", QString::number(depth));
    }
    if (parentId != -1)
    {
        query.addQueryItem(JsonKeys::File::ParentId, QString::number(parentId));
    }

    url.setQuery(query);
    QNetworkRequest httprequest(url);
    QHttpHeaders headers;
    headers.append(QHttpHeaders::WellKnownHeader::ContentType, "application/json");
    authorize(headers);
    httprequest.setHeaders(headers);
    QNetworkReply *reply = manager.get(httprequest);

    QObject::connect(reply, &QNetworkReply::finished, [reply, this]()
    {
        baseUrl.clear();
        if (reply->error() == QNetworkReply::NoError)
        {
            std::optional<QList<DTO::File::TreeNodeResponse>> response;

            QByteArray responseData = reply->readAll();

            QJsonArray jsonresp = QJsonDocument::fromJson(responseData).array();

            response = Serialization::File::fromJsonTreeNodeArray(jsonresp);
            qDebug() << "Success:" << responseData;
            emit treeRequestFinished(response);
        }
        else
        {
            qDebug() << "Error:" << reply->errorString();
            emit treeRequestFinished(std::nullopt);
        }
        reply->deleteLater();
    });
}

void Model::login(QString username, QString password)
{
    DTO::Auth::LoginRequest request;
    request.password = password;
    request.login = username;

    QJsonObject jsonreq = Serialization::Auth::toJson(request);

    baseUrl = "http://" + host.toString() + ":8080";
    QUrl url(baseUrl + "/auth/login");

    QByteArray data = QJsonDocument(jsonreq).toJson();

    QNetworkRequest httprequest(url);

    httprequest.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    QNetworkReply *reply = manager.post(httprequest, data);

    QObject::connect(reply, &QNetworkReply::finished, [reply, this]()
    {
        baseUrl.clear();
        if (reply->error() == QNetworkReply::NoError)
        {
            std::optional<DTO::Auth::GeneralResponse> response;

            QByteArray responseData = reply->readAll();

            QJsonObject jsonresp = QJsonDocument::fromJson(responseData).object();

            response = Serialization::Auth::fromJsonGeneralResponse(jsonresp);
            if (response.has_value())
            {
                token = response.value().accessToken;
                emit saveToken(token);
            }
            qDebug() << "Success:" << responseData;
            emit loginFinished(response);
        }
        else
        {
            qDebug() << "Error:" << reply->errorString();
            emit loginFinished(std::nullopt);
        }
        reply->deleteLater();
    });
}

void Model::signup(QString verificationId, int accessCode, QString username, QString password)
{
    DTO::Auth::ConfirmRegisterRequest request;
    request.accessCode = accessCode;
    request.password = password;
    request.username = username;
    request.verificationId = verificationId;

    QJsonObject jsonreq = Serialization::Auth::toJson(request);

    baseUrl = "http://" + host.toString() + ":8080";
    QUrl url(baseUrl + "/auth/register/confirm");

    QByteArray data = QJsonDocument(jsonreq).toJson();

    QNetworkRequest httprequest(url);

    httprequest.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    QNetworkReply *reply = manager.post(httprequest, data);

    QObject::connect(reply, &QNetworkReply::finished, [reply, this]()
    {
        baseUrl.clear();
        if (reply->error() == QNetworkReply::NoError)
        {
            std::optional<DTO::Auth::GeneralResponse> response;

            QByteArray responseData = reply->readAll();

            QJsonObject jsonresp = QJsonDocument::fromJson(responseData).object();

            response = Serialization::Auth::fromJsonGeneralResponse(jsonresp);
            if (response.has_value())
            {
                token = response.value().accessToken;
                emit saveToken(token);
            }
            qDebug() << "Success:" << responseData;
            emit signupFinished(response);
        }
        else
        {
            qDebug() << "Error:" << reply->errorString();
            emit signupFinished(std::nullopt);
        }
        reply->deleteLater();
    });
}

void Model::sendEmail(QString email)
{
    DTO::Auth::RegisterInitRequest request;
    request.email = email;

    QJsonObject jsonreq = Serialization::Auth::toJson(request);

    baseUrl = "http://" + host.toString() + ":8080";
    QUrl url(baseUrl + "/auth/register/init");

    QByteArray data = QJsonDocument(jsonreq).toJson();

    QNetworkRequest httprequest(url);

    httprequest.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    QNetworkReply *reply = manager.post(httprequest, data);

    QObject::connect(reply, &QNetworkReply::finished, [reply, this]()
    {
        baseUrl.clear();
        if (reply->error() == QNetworkReply::NoError)
        {
            std::optional<DTO::Auth::RegisterInitResponse> response;

            QByteArray responseData = reply->readAll();

            QJsonObject jsonresp = QJsonDocument::fromJson(responseData).object();

            response = Serialization::Auth::fromJsonRegisterInitResponse(jsonresp);
            qDebug() << "Success:" << responseData;
            emit sendEmailFinished(response);
        }
        else
        {
            qDebug() << "Error:" << reply->errorString();
            emit sendEmailFinished(std::nullopt);
        }
        reply->deleteLater();
    });
}
