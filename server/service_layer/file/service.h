#ifndef FILE_SERVICE_H
#define FILE_SERVICE_H

#include <QString>
#include <QList>
#include <QVariant>
#include <QMetaType>
#include <QHash>
#include <QtTypes>
#include <QByteArray>
#include "file_repository.h"
#include "file_storage.h"
#include "file.h"
#include "file/models.h"
#include "file/error_codes.h"
#include "service_result.h"
#include "file/file_config.h"
#include "datetime/time_provider_interface.h"
#include "domain/file_type.h"
#include "converter/file_type_converter.h"

namespace Service::File
{

namespace Model = ::ServiceModel::File;
using ServiceError = ::ErrorCode::File::ServiceError;
using TreeResult = ServiceResult<QList<Model::FileNode>, ServiceError>;
using InitUploadSessionResult = ServiceResult<Model::UploadSessionResult, ServiceError>;
using FileConfig = Config::File::FileConfig;
using NoDataResult = ServiceResult<QVariant, ServiceError>;
using CompleteUploadResult = ServiceResult<Model::CompleteUploadResult, ServiceError>;
using DownloadChunkResult = ServiceResult<Model::ContentRange, ServiceError>;
using CreatedFileObjectResult = ServiceResult<Model::CreatedFileObjectResult, ServiceError>;

class FileService
{
private:
    using FileType = Common::Domain::FileType;
    using FileTypeConverter = Common::Converter::FileTypeConverter;

    FileStorage& m_fileStorage;
    FileRepository& m_fileRep;
    Service::Time::ITimeProvider& m_timeProvider;
    FileConfig m_fileConfig;

    // <uploadId, UploadSession>
    QHash<QString, Model::UploadSession> m_uploadSessions;
public:
    FileService(
        FileStorage& fileStorage,
        FileRepository& fileRep,
        Service::Time::ITimeProvider& timeProvider,
        const FileConfig& fileConfig
    );

    TreeResult getFileTree(
        int userId,
        QVariant parentId = QVariant(QMetaType::fromType<int>()),
        QVariant maxDepth = QVariant(QMetaType::fromType<int>())
        );

    InitUploadSessionResult initUploadSession(
        int userId,
        QString userName,
        QString fileName,
        QVariant parentId,
        qint64 fileSize,
        bool overwrite
        );

    NoDataResult uploadChunk(
        QString uploadId,
        Model::ContentRange contentRange,
        const QByteArray& chunkData
        );

    CompleteUploadResult completeUpload(QString uploadId);

    DownloadChunkResult downloadChunk(
        int userId,
        int fileId,
        Model::RequestedRange reqRange,
        QByteArray& chunkBytesOut
        );

private:
};

}

#endif // FILE_SERVICE_H
