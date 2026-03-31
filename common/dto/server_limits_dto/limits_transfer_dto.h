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
    UploadLimitsDto upload;
    DownloadLimitsDto download;
    StorageLimitsDto storage;
};

#endif // LIMITS_TRANSFER_DTO_H
