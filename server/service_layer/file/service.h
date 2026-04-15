#ifndef FILE_SERVICE_H
#define FILE_SERVICE_H

#include <QString>
#include <QList>
#include <QVariant>
#include <QMetaType>
#include "file_repository.h"
#include "file_storage.h"
#include "file.h"
#include "file/models.h"
#include "file/error_codes.h"
#include "service_result.h"

namespace Service::File
{

namespace Model = ::ServiceModel::File;
using ServiceError = ::ErrorCode::File::ServiceError;
using TreeResult = ServiceResult<QList<Model::FileNode>, ServiceError>;

class FileService
{
private:
    FileStorage& m_fileStorage;
    FileRepository& m_fileRep;
public:
    FileService(
        FileStorage& fileStorage,
        FileRepository& fileRep
    );

    TreeResult getFileTree(
        int userId,
        QVariant parentId = QVariant(QMetaType::fromType<int>()),
        QVariant maxDepth = QVariant(QMetaType::fromType<int>())
        );

private:

};

}

#endif // FILE_SERVICE_H
