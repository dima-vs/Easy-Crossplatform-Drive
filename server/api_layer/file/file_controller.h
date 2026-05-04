#ifndef FILE_CONTROLLER_H
#define FILE_CONTROLLER_H

#include <QHttpServer>
#include <QHttpServerRequest>
#include <QHttpServerResponse>
#include <QHttpServerResponder>
#include <QString>
#include <optional>

#include "file/service.h"
#include "auth/service.h"
#include "file/error_codes.h"
#include "file/file_models_converter.h"

namespace Api::Controllers
{

class FileController
{
    QHttpServer& m_server;
    Service::File::FileService& m_fileService;
    Service::Auth::AuthService& m_authService;

public:
    FileController(
        QHttpServer& server,
        Service::File::FileService& fileService,
        Service::Auth::AuthService& authService
        );

    void registerEndpoints();

    // GET /files?parentId={parentId}&maxDepth={maxDepth}
    QHttpServerResponse getTree(const QHttpServerRequest& request);

    // POST /uploads/
    QHttpServerResponse initUpload(const QHttpServerRequest& request);

    // POST /uploads/{uploadId}
    QHttpServerResponse upload(
        const QString& uploadId,
        const QHttpServerRequest& request
        );

    // POST /uploads/{uploadId}/complete
    QHttpServerResponse completeUpload(
        const QString& uploadId,
        const QHttpServerRequest& request
        );

    // GET /files/{id}
    QHttpServerResponse download(
        int id,
        const QHttpServerRequest& request
        );

    // POST /files
    QHttpServerResponse createEmpty(const QHttpServerRequest& request);

    // DELETE /files/{fileId}
    QHttpServerResponse remove(
        int fileId,
        const QHttpServerRequest& request
        );

    // PATCH /files/{fileId}
    QHttpServerResponse rename(
        int fileId,
        const QHttpServerRequest& request
        );
private:
    std::optional<ServiceModel::Auth::Result> authenticate(
        const QHttpServerRequest& request
        );

    std::optional<ServiceModel::File::ContentRange> parseContentRangeHeader(
        const QHttpServerRequest& request
        ) const;

    std::optional<ServiceModel::File::RequestedRange> parseRangeHeader(
        const QHttpServerRequest& request
        ) const;

    QString buildContentRangeHeader(
        const ServiceModel::File::ContentRange &contentRange
        ) const;

    QHttpServerResponse buildErrorResponse(
        ErrorCode::File::ServiceError error
        );

    QHttpServerResponse buildBadRequestErrorResponse(
        const QString& errorMsg
        );

    QHttpServerResponse buildErrorResponse(
        const QString& errorMsg,
        QHttpServerResponder::StatusCode statusCode
        );
};

} // namespace Api::Controllers

#endif // FILE_CONTROLLER_H
