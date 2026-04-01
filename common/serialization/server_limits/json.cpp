#include "serialization/server_limits/json.h"

namespace Serialization::ServerLimits
{

// ==========================================
// [DTO::ServerLimits::TransferResponse]
// ==========================================
std::optional<DTO::ServerLimits::TransferResponse> fromJsonTransferResponse(const QJsonObject& json)
{
    DTO::ServerLimits::TransferResponse dto;

    if (!json.contains("upload") || !json["upload"].isObject())
    {
        qWarning() << "invalid or missing 'upload' field in JSON";
        return std::nullopt;
    }

    if (!json.contains("download") || !json["download"].isObject())
    {
        qWarning() << "invalid or missing 'download' field in JSON";
        return std::nullopt;
    }

    if (!json.contains("storage") || !json["storage"].isObject())
    {
        qWarning() << "invalid or missing 'storage' field in JSON";
        return std::nullopt;
    }

    QJsonObject uploadObj = json["upload"].toObject();
    QJsonObject downloadObj = json["download"].toObject();
    QJsonObject storageObj = json["storage"].toObject();

    if (!uploadObj.contains("maxChunkSize") || !uploadObj["maxChunkSize"].isDouble() ||
        !uploadObj.contains("maxFileSize") || !uploadObj["maxFileSize"].isDouble())
    {
        qWarning() << "invalid or missing numeric fields in 'upload' object";
        return std::nullopt;
    }

    if (!downloadObj.contains("maxChunkSize") || !downloadObj["maxChunkSize"].isDouble())
    {
        qWarning() << "invalid or missing 'maxChunkSize' numeric field in 'download' object";
        return std::nullopt;
    }

    if (!storageObj.contains("maxTotalPerUser") || !storageObj["maxTotalPerUser"].isDouble())
    {
        qWarning() << "invalid or missing 'maxTotalPerUser' numeric field in 'storage' object";
        return std::nullopt;
    }

    dto.upload.maxChunkSize = uploadObj["maxChunkSize"].toInteger();
    dto.upload.maxFileSize = uploadObj["maxFileSize"].toInteger();

    dto.download.maxChunkSize = downloadObj["maxChunkSize"].toInteger();

    dto.storage.maxTotalPerUser = storageObj["maxTotalPerUser"].toInteger();

    return dto;
}

QJsonObject toJson(const DTO::ServerLimits::TransferResponse& dto)
{
    QJsonObject root;

    // upload
    QJsonObject uploadObj;
    uploadObj["maxChunkSize"] = dto.upload.maxChunkSize;
    uploadObj["maxFileSize"] = dto.upload.maxFileSize;

    // download
    QJsonObject downloadObj;
    downloadObj["maxChunkSize"] = dto.download.maxChunkSize;

    // storage
    QJsonObject storageObj;
    storageObj["maxTotalPerUser"] = dto.storage.maxTotalPerUser;

    root["upload"] = uploadObj;
    root["download"] = downloadObj;
    root["storage"] = storageObj;

    return root;
}

}
