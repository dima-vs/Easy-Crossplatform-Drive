#ifndef FILE_CONFIG_H
#define FILE_CONFIG_H

#include <QtTypes>
#include <QString>

namespace Config::File
{

struct FileTransferLimits
{
    // Default: 5 MB chunk, 1 GB file
    qint64 maxChunkSize = 5LL * 1024LL * 1024LL;
    qint64 maxFileSize = 1024LL * 1024LL * 1024LL;
};

struct FileConfig
{
    FileTransferLimits upload;
    FileTransferLimits download;

    struct Storage
    {
        // 10 GB per user
        qint64 maxTotalStoragePerUser = 10LL * 1024LL * 1024LL * 1024LL;
        QString baseStoragePath = "storage";
    } storage;

    struct Session
    {
        int uploadSessionsDurationSec = 3 * 60 * 60; // 3 hours
    } session;

    struct Uuid
    {
        // max count of iterations to generate uuid in case of
        // it already exists
        int generationAttemptsLimit = 8;
    } uuid;
};

}

#endif // FILE_CONFIG_H
