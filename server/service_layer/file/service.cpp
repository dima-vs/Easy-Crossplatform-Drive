#include <QHash>
#include <QPair>
#include <QList>
#include <QUuid>
#include <list>
#include <vector>
#include <cmath>
#include <utility>
#include <algorithm>
#include "file/service.h"

namespace Service::File
{


FileService::FileService(
    FileStorage &fileStorage,
    FileRepository& fileRep,
    Time::ITimeProvider &timeProvider,
    const FileConfig &fileConfig) :
    m_fileStorage(fileStorage),
    m_fileRep(fileRep),
    m_timeProvider(timeProvider),
    m_fileConfig(fileConfig)
{}

TreeResult FileService::getTree(
    int userId,
    QVariant parentId,
    QVariant maxDepth
    )
{
    QPair<QList<FileRecord>, QList<FileRecord>> filesAndDirs;
    bool success = m_fileRep.getAllNestedObjects(
        userId,
        parentId,
        filesAndDirs,
        maxDepth
        );

    if (!success)
    {
        return TreeResult::fail(ServiceError::CannotGetFileTree);
    }

    bool isRequestedRootFixed = !parentId.isNull();
    int requestedRootId = isRequestedRootFixed ? parentId.toInt() : 0;

    std::list<Model::FileNode> roots;
    QHash<int, Model::FileNode*> lookupTable;

    QList<FileRecord>& files = filesAndDirs.first;
    QList<FileRecord>& dirs = filesAndDirs.second;

    for (int i = 0; i < dirs.size(); ++i)
    {
        FileRecord& fileRecord = dirs[i];
        Model::FileNode fileNode;

        fileNode.id = fileRecord.id();
        fileNode.logicalName = fileRecord.logicalName();
        fileNode.size = fileRecord.size();
        fileNode.type = fileRecord.type();
        fileNode.uploadTime = fileRecord.uploadTime();

        bool isRoot = (!isRequestedRootFixed && fileRecord.parentId().isNull()) ||
                      (isRequestedRootFixed && (fileRecord.id() == requestedRootId) );

        if (isRoot)
        {
            roots.push_back(fileNode);
            lookupTable[fileNode.id] = &roots.back();
        }
        else
        {
            int pId = fileRecord.parentId().toInt();
            Model::FileNode* parentPtr = lookupTable.value(pId, nullptr);
            if (parentPtr)
            {
                parentPtr->children.push_back(fileNode);
                lookupTable[fileNode.id] = &parentPtr->children.back();
            }
        }
    }

    for (int i = 0; i < files.size(); ++i)
    {
        FileRecord& fileRecord = files[i];
        Model::FileNode fileNode;

        fileNode.id = fileRecord.id();
        fileNode.logicalName = fileRecord.logicalName();
        fileNode.size = fileRecord.size();
        fileNode.type = fileRecord.type();
        fileNode.uploadTime = fileRecord.uploadTime();

        bool isRoot = (!isRequestedRootFixed && fileRecord.parentId().isNull()) ||
                      (isRequestedRootFixed && (fileRecord.id() == requestedRootId) );

        if (isRoot)
        {
            roots.push_back(fileNode);
        }
        else
        {
            int pId = fileRecord.parentId().toInt();
            Model::FileNode* parentPtr = lookupTable.value(pId, nullptr);
            if (parentPtr) {
                parentPtr->children.push_back(fileNode);
            }
        }
    }

    QList<Model::FileNode> result;
    for (const Model::FileNode& rootObj : roots)
    {
        result.append(rootObj);
    }

    return TreeResult::ok(result);
}

InitUploadSessionResult FileService::initUpload(
    int userId,
    QString userName,
    QString fileName,
    QVariant parentId,
    qint64 fileSize,
    bool overwrite
    )
{
    if (fileSize <= 0)
    {
        return InitUploadSessionResult::fail(
            ServiceError::InvalidFileSize);
    }
    if (fileSize > m_fileConfig.upload.maxFileSize)
    {
        return InitUploadSessionResult::fail(ServiceError::FileTooLarge);
    }

    FileRecord existing = m_fileRep.getFile(userId, parentId, fileName);
    bool fileEntryExist = existing.isValid();

    if (fileEntryExist && overwrite &&
        (existing.type() == FileType::Directory)
        )
    {
        return InitUploadSessionResult::fail(
            ServiceError::CannotOverwriteDirectory);
    }

    if (fileEntryExist && !overwrite)
    {
        return InitUploadSessionResult::fail(
            ServiceError::FileAlreadyExist);
    }

    QString uploadId;
    bool idGenerated = false;
    for (int i = 0; i < m_fileConfig.uuid.generationAttemptsLimit; ++i)
    {
        uploadId = QUuid::createUuid().toString(
            QUuid::StringFormat::WithoutBraces);
        if (!m_uploadSessions.contains(uploadId))
        {
            idGenerated = true;
            break;
        }
    }

    if (!idGenerated)
    {
        return InitUploadSessionResult::fail(
            ServiceError::UuidAlreadyExists);
    }

    qint64 chunkSize = m_fileConfig.upload.maxChunkSize;
    if (chunkSize <= 0)
    {
        return InitUploadSessionResult::fail(
            ServiceError::InvalidChunkSize);
    }

    Model::UploadSession uploadSession;
    uploadSession.userId = userId;
    uploadSession.userName = userName;
    uploadSession.chunkSize = chunkSize;
    uploadSession.expiresAt =
        m_timeProvider.currentDateTimeUtc()
            .addSecs(m_fileConfig.session.uploadSessionsDurationSec);
    uploadSession.fileName = fileName;
    uploadSession.fileSize = fileSize;
    uploadSession.parentId = parentId;

    QString uuidStr = QUuid::createUuid().toString(
        QUuid::StringFormat::WithoutBraces);
    QString serverFileName = uploadSession.userName + "_" +
                             uuidStr;
    uploadSession.serverFileName = serverFileName;

    int chunksCountNeeded = std::ceil(
        static_cast<double>(fileSize) / chunkSize
        );

    uploadSession.chunksUploaded = std::vector<bool>(
        chunksCountNeeded, false);

    if (fileEntryExist && overwrite)
    {
        uploadSession.wasOverwritten = true;
        uploadSession.replacedFileId =
            existing.id();
    }
    else
    {
        uploadSession.wasOverwritten = false;
    }

    m_uploadSessions[uploadId] = uploadSession;

    Model::UploadSessionResult result;
    result.chunkSize = uploadSession.chunkSize;
    result.expiresAt = uploadSession.expiresAt;
    result.uploadId = uploadId;

    return InitUploadSessionResult::ok(result);
}

NoDataResult FileService::upload(
    QString uploadId,
    Model::ContentRange contentRange,
    const QByteArray &chunk)
{
    if (chunk.size() > m_fileConfig.upload.maxChunkSize)
        return NoDataResult::fail(ServiceError::ChunkTooLarge);

    int chunkSizeCalculated = contentRange.endByte -
                              contentRange.startByte + 1;

    if (chunkSizeCalculated != chunk.size())
        return NoDataResult::fail(ServiceError::InvalidContentRange);

    if (!m_uploadSessions.contains(uploadId))
        return NoDataResult::fail(ServiceError::SessionDoesNotExists);

    Model::UploadSession& uploadSession = m_uploadSessions[uploadId];

    int chunkIndex = contentRange.startByte /
                     uploadSession.chunkSize;

    if (chunkIndex < 0 || chunkIndex >= uploadSession.chunksUploaded.size())
    {
        return NoDataResult::fail(ServiceError::InvalidContentRange);
    }

    if (uploadSession.completed)
    {
        return NoDataResult::fail(
            ServiceError::UploadSessionAlreadyCompleted);
    }

    // if the chunk has already been written, skip it
    if (uploadSession.chunksUploaded[chunkIndex])
    {
        return NoDataResult::ok(Model::NoData());
    }

    qint64 lastChunkSize = uploadSession.fileSize %
                           uploadSession.chunkSize;
    if (lastChunkSize == 0)
    {
        lastChunkSize = uploadSession.chunkSize;
    }

    // if chunkIndex is not last and sizes dont equal
    if (
        (chunkIndex != uploadSession.chunksUploaded.size()-1) &&
        chunkSizeCalculated != uploadSession.chunkSize
        )
    {
        return NoDataResult::fail(ServiceError::InvalidChunkSize);
    }
    else if (
        // if chunkIndex is last and chunk size doesn't match lastChunkSize
        (chunkIndex == uploadSession.chunksUploaded.size()-1) &&
        chunkSizeCalculated != lastChunkSize
        )
    {
        return NoDataResult::fail(ServiceError::InvalidChunkSize);
    }

    bool success = m_fileStorage.writeChunk(
        uploadSession.serverFileName,
        contentRange.startByte,
        chunk
        );

    if (!success)
    {
        return NoDataResult::fail(
            ServiceError::FailedToPerformStorageOperation);
    }

    uploadSession.chunksUploaded[chunkIndex] = true;
    uploadSession.completed = std::all_of(
        uploadSession.chunksUploaded.begin(),
        uploadSession.chunksUploaded.end(),
            [](bool uploaded) { return uploaded; }
        );
    return NoDataResult::ok(Model::NoData());
}

CompleteUploadResult FileService::completeUpload(QString uploadId)
{
    if (!m_uploadSessions.contains(uploadId))
        return CompleteUploadResult::fail(ServiceError::SessionDoesNotExists);

    Model::UploadSession& uploadSession = m_uploadSessions[uploadId];
    int userId = uploadSession.userId;

    if (!uploadSession.completed)
    {
        return CompleteUploadResult::fail(
            ServiceError::SessionIsNotCompleted);
    }

    // if file with the same name exists, first, delete it
    if (uploadSession.wasOverwritten)
    {
        int fileToDeleteId = uploadSession.replacedFileId;
        NoDataResult res = removeEntry(userId, fileToDeleteId);
        if (!res.isOk())
        {
            return CompleteUploadResult::fail(res.error());
        }
    }

    bool fileCreated = m_fileStorage.exists(
        uploadSession.serverFileName);
    if (!fileCreated)
    {
        return CompleteUploadResult::fail(
            ServiceError::FileNotCreated);
    }

    FileRecord fileRecord(
        userId,
        FileType::File,
        uploadSession.fileName,
        uploadSession.serverFileName,
        uploadSession.fileSize,
        uploadSession.parentId
        );
    bool fileRecordCreated = m_fileRep.addNewFile(fileRecord);
    if (!fileRecordCreated)
    {
        return CompleteUploadResult::fail(
            ServiceError::FailedToPerformDBOperation);
    }
    if (!fileRecord.isIDSet())
    {
        qCritical() << "database didn't set id after file insertion";
        return CompleteUploadResult::fail(
            ServiceError::FailedToPerformDBOperation);
    }

    m_uploadSessions.remove(uploadId);

    Model::CompleteUploadResult result;
    result.createdAt = m_timeProvider.currentDateTimeUtc();
    result.fileId = fileRecord.id();
    result.fileName = fileRecord.logicalName();
    result.parentId = fileRecord.parentId();
    result.size = fileRecord.size();

    return CompleteUploadResult::ok(result);
}

DownloadChunkResult FileService::download(
    int userId,
    int fileId,
    Model::RequestedRange reqRange,
    QByteArray& chunkBytesOut
    )
{
    FileRecord fileRecord = m_fileRep.getFile(fileId);

    if (!fileRecord.isValid())
    {
        return DownloadChunkResult::fail(
            ServiceError::FileNotFound);
    }

    if (!m_fileRep.checkPermission(userId, fileRecord))
    {
        return DownloadChunkResult::fail(
            ServiceError::PermissionDenied);
    }

    if (
        (reqRange.startByte < 0) ||
        (reqRange.endByte < 0) ||
        (reqRange.startByte > reqRange.endByte)
        )
    {
        return DownloadChunkResult::fail(
            ServiceError::InvalidContentRange);
    }

    if (fileRecord.size() > 0 && reqRange.startByte >= fileRecord.size())
    {
        return DownloadChunkResult::fail(
            ServiceError::InvalidContentRange);
    }

    int requestedSize = reqRange.endByte -
                        reqRange.startByte + 1;

    if (requestedSize <= 0)
    {
        return DownloadChunkResult::fail(
            ServiceError::InvalidContentRange);
    }

    if (fileRecord.size() > m_fileConfig.download.maxFileSize)
    {
        return DownloadChunkResult::fail(
            ServiceError::FileTooLarge);
    }

    if (requestedSize > m_fileConfig.download.maxChunkSize)
    {
        return DownloadChunkResult::fail(
            ServiceError::ChunkTooLarge);
    }

    chunkBytesOut = std::move(m_fileStorage.readChunk(
        fileRecord.serverName().toString(),
        reqRange.startByte,
        requestedSize
        ));

    if (chunkBytesOut.isEmpty() && (fileRecord.size() > 0))
    {
        return DownloadChunkResult::fail(
            ServiceError::FailedToPerformStorageOperation);
    }

    Model::ContentRange result;
    result.startByte = reqRange.startByte;
    result.totalBytes = fileRecord.size();
    int actualReadSize = chunkBytesOut.size();
    result.endByte = (actualReadSize > 0) ?
                         (reqRange.startByte + actualReadSize - 1) : 0;
    return DownloadChunkResult::ok(result);
}

CreatedFileObjectResult FileService::createEmpty(
    int userId,
    QString userName,
    QString fileName,
    QVariant parentId,
    FileType type,
    bool overwrite
    )
{
    if (type == FileType::Unknown)
    {
        return CreatedFileObjectResult::fail(
            ServiceError::InvalidFileObjType);
    }

    int size = 0;
    QVariant serverName;
    // generate a server name for file
    if (type == FileType::File)
    {
        QString uuidStr = QUuid::createUuid().toString(
            QUuid::StringFormat::WithoutBraces);
        QString serverNameStr = userName + "_" +
                                 uuidStr;
        serverName = QVariant(serverNameStr);
    }

    FileRecord existing = m_fileRep.getFile(
        userId,
        parentId,
        fileName
        );

    bool fileEntryExist = existing.isValid();

    if (fileEntryExist && !overwrite)
    {
        return CreatedFileObjectResult::fail(
            ServiceError::FileAlreadyExist
            );
    }
    else if (fileEntryExist && overwrite &&
               (existing.type() == FileType::Directory)
               )
    {
        return CreatedFileObjectResult::fail(
            ServiceError::CannotOverwriteDirectory
            );
    }
    else if (fileEntryExist && overwrite)
    {
        NoDataResult fileDeletedRes = removeEntry(existing);
        if (!fileDeletedRes.isOk())
        {
            return CreatedFileObjectResult::fail(fileDeletedRes.error());
        }

    }

    FileRecord fileRecord(
        userId,
        type,
        fileName,
        serverName,
        size,
        parentId
        );

    NoDataResult fileCreatedRes = createEmpty(fileRecord);
    if (!fileCreatedRes.isOk())
    {
        return CreatedFileObjectResult::fail(fileCreatedRes.error());
    }

    if (!fileRecord.isIDSet())
    {
        qCritical() << "database didn't set id after file insertion";
        return CreatedFileObjectResult::fail(
            ServiceError::FailedToPerformDBOperation);
    }

    Model::CreatedFileObjectResult result;
    result.createdAt = m_timeProvider.currentDateTimeUtc();
    result.fileId = fileRecord.id();
    result.fileName = fileRecord.logicalName();
    result.parentId = fileRecord.parentId();

    return CreatedFileObjectResult::ok(result);
}

NoDataResult FileService::createEmpty(FileRecord& fileRecord)
{
    if (fileRecord.type() == FileType::File)
    {
        bool fileCreated = m_fileStorage.createEmptyFile(
            fileRecord.serverName().toString()
            );
        if (!fileCreated)
        {
            return NoDataResult::fail(
                ServiceError::FailedToPerformStorageOperation);
        }
    }

    bool fileRecordAdded = m_fileRep.addNewFile(fileRecord);
    if (!fileRecordAdded)
    {
        if (fileRecord.type() == FileType::File)
        {
            m_fileStorage.removeFile(fileRecord.serverName().toString());
        }

        return NoDataResult::fail(
            ServiceError::FailedToPerformDBOperation);
    }
    return NoDataResult::ok(Model::NoData());
}

NoDataResult FileService::removeEntry(int userId, int fileId)
{
    FileRecord toDelete = m_fileRep.getFile(fileId);
    if (!toDelete.isValid())
    {
        return NoDataResult::fail(ServiceError::FileNotFound);
    }

    if (!m_fileRep.checkPermission(userId, toDelete))
    {
        return NoDataResult::fail(
            ServiceError::PermissionDenied);
    }

    return removeEntry(toDelete);
}

NoDataResult FileService::removeEntry(const FileRecord& toDelete)
{
    QList<QString> physicalFilesToDelete;
    int deletedCount = 0;

    bool fileRecordDeleted = m_fileRep.deleteFile(
        toDelete.ownerId(),
        toDelete.id(),
        physicalFilesToDelete,
        &deletedCount
        );

    if (!fileRecordDeleted)
    {
        qCritical() << "cannot delete db record about file with id:" <<
            toDelete.id();
        return NoDataResult::fail(
            ServiceError::FailedToPerformDBOperation);
    }

    for (const QString& serverName : physicalFilesToDelete)
    {
        bool physicalFileDeleted = m_fileStorage.removeFile(serverName);
        if (!physicalFileDeleted)
        {
            qWarning() << "cannot delete physical file:" << serverName;
        }
    }

    return NoDataResult::ok(Model::NoData());
}


}
