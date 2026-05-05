#include <QDebug>
#include "serialization/server_limits/json.h"
#include "serialization/server_limits/json_keys.h"
#include "serialization/json_helpers.h"


namespace Serialization::ServerLimits
{

namespace Keys = JsonKeys::ServerLimits;

// ==========================================
// [DTO::ServerLimits::TransferResponse]
// ==========================================
std::optional<DTO::ServerLimits::TransferResponse> fromJsonTransferResponse(const QJsonObject& json)
{
    DTO::ServerLimits::TransferResponse dto;

    QJsonObject uploadObj;
    QJsonObject downloadObj;
    QJsonObject storageObj;

    if (!JsonHelper::requireObject(json, Keys::Upload, uploadObj) ||
        !JsonHelper::requireObject(json, Keys::Download, downloadObj) ||
        !JsonHelper::requireObject(json, Keys::Storage, storageObj))
    {
        return std::nullopt;
    }

    if (!JsonHelper::requireInt64(uploadObj, Keys::MaxChunkSize, dto.upload.maxChunkSize) ||
        !JsonHelper::requireInt64(uploadObj, Keys::MaxFileSize, dto.upload.maxFileSize))
    {
        return std::nullopt;
    }

    if (!JsonHelper::requireInt64(downloadObj, Keys::MaxChunkSize, dto.download.maxChunkSize) ||
        !JsonHelper::requireInt64(downloadObj, Keys::MaxFileSize, dto.download.maxFileSize))
    {
        return std::nullopt;
    }

    if (!JsonHelper::requireInt64(storageObj, Keys::MaxTotalPerUser, dto.storage.maxTotalPerUser))
    {
        return std::nullopt;
    }

    return dto;
}

QJsonObject toJson(const DTO::ServerLimits::TransferResponse& dto)
{
    QJsonObject root;

    // upload
    QJsonObject uploadObj;
    uploadObj[Keys::MaxChunkSize] = dto.upload.maxChunkSize;
    uploadObj[Keys::MaxFileSize] = dto.upload.maxFileSize;

    // download
    QJsonObject downloadObj;
    downloadObj[Keys::MaxChunkSize] = dto.download.maxChunkSize;
    downloadObj[Keys::MaxFileSize] = dto.download.maxFileSize;

    // storage
    QJsonObject storageObj;
    storageObj[Keys::MaxTotalPerUser] = dto.storage.maxTotalPerUser;

    root[Keys::Upload] = uploadObj;
    root[Keys::Download] = downloadObj;
    root[Keys::Storage] = storageObj;

    return root;
}

}
