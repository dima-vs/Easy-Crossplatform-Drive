#ifndef TRANSFER_H
#define TRANSFER_H

#include <QtTypes>

namespace DTO::ServerLimits
{

struct FileTransferLimits
{
    qint64 maxChunkSize;
    qint64 maxFileSize;
};

struct Storage
{
    qint64 maxTotalPerUser;
};

struct TransferResponse
{
    FileTransferLimits upload;
    FileTransferLimits download;
    Storage storage;
};

}

#endif // TRANSFER_H
