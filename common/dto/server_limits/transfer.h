#ifndef TRANSFER_H
#define TRANSFER_H

#include <QtTypes>

namespace DTO::ServerLimits
{

struct Upload
{
    qint64 maxChunkSize;
    qint64 maxFileSize;
};

struct Download
{
    qint64 maxChunkSize;
};

struct Storage
{
    qint64 maxTotalPerUser;
};

struct TransferResponse
{
    Upload upload;
    Download download;
    Storage storage;
};

}

#endif // TRANSFER_H
