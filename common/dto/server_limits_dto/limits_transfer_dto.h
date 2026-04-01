#ifndef LIMITS_TRANSFER_DTO_H
#define LIMITS_TRANSFER_DTO_H

#include <QtTypes>

struct UploadLimitsDTO
{
    qint64 maxChunkSize;
    qint64 maxFileSize;
};

struct DownloadLimitsDTO
{
    qint64 maxChunkSize;
};

struct StorageLimitsDTO
{
    qint64 maxTotalPerUser;
};

struct TransferLimitsResponseDTO
{
    UploadLimitsDTO upload;
    DownloadLimitsDTO download;
    StorageLimitsDTO storage;
};

#endif // LIMITS_TRANSFER_DTO_H
