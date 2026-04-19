#ifndef FILE_MODELS_H
#define FILE_MODELS_H

#include <QString>
#include <QVariant>
#include <QDateTime>
#include <QList>
#include <QVariant>
#include <list>
#include <QtTypes>
#include <vector> // for bool optimization
#include "domain/file_type.h"

namespace ServiceModel::File
{

struct FileNode
{
    int id;
    QString logicalName;
    Common::Domain::FileType type;
    QVariant size;
    QDateTime uploadTime;
    std::list<FileNode> children;
};

struct UploadSession
{
    int userId = 0;
    QString userName;
    QDateTime expiresAt;
    qint64 chunkSize;
    QString fileName;
    QVariant parentId;
    qint64 fileSize;

    bool wasOverwritten = false;
    int oldFileIdToDelete = 0;
    QString serverFileName = "";

    std::vector<bool> chunksUploaded;
    bool completed = false;
};

struct UploadSessionResult
{
    QString uploadId;
    QDateTime expiresAt;
    qint64 chunkSize;
};

struct ContentRange
{
    int startByte;
    int endByte; // included
    int totalBytes;
};

struct RequestedRange
{
    int startByte;
    int endByte; // included
};

struct CompleteUploadResult
{
    QDateTime createdAt;
    int fileId;
    QString fileName;
    QVariant parentId;
    qint64 size;
};

struct CreatedFileObjectResult
{
    QDateTime createdAt;
    int fileId;
    QString fileName;
    QVariant parentId;
};

}

#endif // FILE_MODELS_H
