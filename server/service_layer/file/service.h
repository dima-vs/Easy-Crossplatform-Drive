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

namespace Service::File
{

namespace Model = ::ServiceModel::File;
using ServiceError = ::ErrorCode::File::ServiceError;
using TreeResult = ServiceResult<QList<Model::FileNode>, ServiceError>;
using InitUploadSessionResult = ServiceResult<Model::UploadSessionResult, ServiceError>;
using FileConfig = Config::File::FileConfig;
using NoDataResult = ServiceResult<QVariant, ServiceError>;

class FileService
{
private:
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

private:

};

}

#endif // FILE_SERVICE_H
