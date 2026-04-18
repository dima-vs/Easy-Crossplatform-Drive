#include <QHash>
#include <QPair>
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

TreeResult FileService::getFileTree(
    int userId,
    QVariant parentId,
    QVariant maxDepth
    )
{
    QPair<QList<::File>, QList<::File>> filesAndDirs;
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

    QList<::File>& files = filesAndDirs.first;
    QList<::File>& dirs = filesAndDirs.second;

    for (int i = 0; i < dirs.size(); ++i)
    {
        ::File& fileObj = dirs[i];
        Model::FileNode fileNode;

        fileNode.id = fileObj.id();
        fileNode.logicalName = fileObj.logicalName();
        fileNode.size = fileObj.size();
        fileNode.type = fileObj.type();
        fileNode.uploadTime = fileObj.uploadTime();

        bool isRoot = (!isRequestedRootFixed && fileObj.parentId().isNull()) ||
                      (isRequestedRootFixed && (fileObj.id() == requestedRootId) );

        if (isRoot)
        {
            roots.push_back(fileNode);
            lookupTable[fileNode.id] = &roots.back();
        }
        else
        {
            int pId = fileObj.parentId().toInt();
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
        ::File& fileObj = files[i];
        Model::FileNode fileNode;

        fileNode.id = fileObj.id();
        fileNode.logicalName = fileObj.logicalName();
        fileNode.size = fileObj.size();
        fileNode.type = fileObj.type();
        fileNode.uploadTime = fileObj.uploadTime();

        bool isRoot = (!isRequestedRootFixed && fileObj.parentId().isNull()) ||
                      (isRequestedRootFixed && (fileObj.id() == requestedRootId) );

        if (isRoot)
        {
            roots.push_back(fileNode);
        }
        else
        {
            int pId = fileObj.parentId().toInt();
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

InitUploadSessionResult FileService::initUploadSession(
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

    ::File existingDbFile = m_fileRep.getFile(userId, parentId, fileName);
    bool fileExist = existingDbFile.isValid();

    if (!overwrite && fileExist)
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

    if (overwrite && fileExist)
    {
        uploadSession.wasOverwritten = true;
        uploadSession.oldFileIdToDelete =
            existingDbFile.id();
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

NoDataResult FileService::uploadChunk(
    QString uploadId,
    Model::ContentRange contentRange,
    const QByteArray &chunkData)
{
    if (chunkData.size() > m_fileConfig.upload.maxChunkSize)
        return NoDataResult::fail(ServiceError::ChunkTooLarge);

    int chunkSizeCalculated = contentRange.endByte -
                              contentRange.startByte + 1;

    if (chunkSizeCalculated != chunkData.size())
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
        return NoDataResult::ok(QVariant());
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
        chunkData
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
    return NoDataResult::ok(QVariant());
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
        int fileToDeleteId = uploadSession.oldFileIdToDelete;
        ::File fileToDelete = m_fileRep.getFile(fileToDeleteId);
        if (!fileToDelete.isValid())
            return CompleteUploadResult::fail(ServiceError::FileNotFound);

        bool fileRecordDeleted = m_fileRep.deleteFile(
            userId, fileToDeleteId);
        if (!fileRecordDeleted)
        {
            qCritical() << "cannot delete db record about file" <<
                "with id:" << fileToDeleteId;
            return CompleteUploadResult::fail(
                ServiceError::FailedToPerformDBOperation);
        }

        bool fileDeleted = m_fileStorage.removeFile(
            fileToDelete.serverName().toString());
        if (!fileDeleted)
        {
            qCritical() << "cannot delete file" <<
                "with id:" << fileToDeleteId;
            return CompleteUploadResult::fail(
                ServiceError::FailedToPerformStorageOperation);
        }
    }

    bool fileCreated = m_fileStorage.exists(
        uploadSession.serverFileName);
    if (!fileCreated)
    {
        return CompleteUploadResult::fail(
            ServiceError::FileNotCreated);
    }

    ::File createdFileRecord(
        userId,
        "file",
        uploadSession.fileName,
        uploadSession.serverFileName,
        uploadSession.fileSize,
        uploadSession.parentId
        );
    bool fileRecordCreated = m_fileRep.addNewFile(createdFileRecord);
    if (!fileRecordCreated)
    {
        return CompleteUploadResult::fail(
            ServiceError::FailedToPerformDBOperation);
    }
    if (!createdFileRecord.isIDSet())
    {
        qCritical() << "database didn't set id after file insertion";
        return CompleteUploadResult::fail(
            ServiceError::FailedToPerformDBOperation);
    }

    m_uploadSessions.remove(uploadId);

    Model::CompleteUploadResult result;
    result.createdAt = m_timeProvider.currentDateTimeUtc();
    result.fileId = createdFileRecord.id();
    result.fileName = createdFileRecord.logicalName();
    result.parentId = createdFileRecord.parentId();
    result.size = createdFileRecord.size();

    return CompleteUploadResult::ok(result);
}

DownloadChunkResult FileService::downloadChunk(
    int userId,
    int fileId,
    Model::RequestedRange reqRange,
    QByteArray& chunkBytesOut
    )
{
    ::File file = m_fileRep.getFile(fileId);

    if (!file.isValid())
    {
        return DownloadChunkResult::fail(
            ServiceError::FileNotFound);
    }

    if (!m_fileRep.checkPermission(userId, file))
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

    if (file.size() > 0 && reqRange.startByte >= file.size())
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

    if (file.size() > m_fileConfig.download.maxFileSize)
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
        file.serverName().toString(),
        reqRange.startByte,
        requestedSize
        ));

    if (chunkBytesOut.isEmpty() && (file.size() > 0))
    {
        return DownloadChunkResult::fail(
            ServiceError::FailedToPerformStorageOperation);
    }

    Model::ContentRange result;
    result.startByte = reqRange.startByte;
    result.totalBytes = file.size();
    int actualReadSize = chunkBytesOut.size();
    result.endByte = (actualReadSize > 0) ?
                         (reqRange.startByte + actualReadSize - 1) : 0;
    return DownloadChunkResult::ok(result);
}


}
