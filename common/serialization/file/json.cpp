#include <QDebug>
#include <QDateTime>
#include <QString>
#include <QList>
#include <QJsonValue>
#include <QJsonArray>
#include <utility>
#include "serialization/file/json.h"

namespace Serialization::File
{

// ==========================================
// internal private namespace for helper functions
// ==========================================
namespace
{

// JSON -> DTO helpers
bool validateTreeNodeRespJSON(const QJsonObject& json);
void parseJsonNodeRequiredFields(const QJsonObject& json, DTO::File::TreeNodeResponse& dtoOut);
void parseJsonFileRequiredFields(const QJsonObject& json, DTO::File::TreeNodeResponse& dtoOut);
void parseJsonDirRequiredFields(const QJsonObject& json, DTO::File::TreeNodeResponse& dtoOut);

// DTO -> JSON helpers
bool validateTreeNodeDTO(const DTO::File::TreeNodeResponse& dto);
void serializeTreeNodeGeneralFields(const DTO::File::TreeNodeResponse& dto, QJsonObject& jsonOut);
void serializeTreeNodeFileFields(const DTO::File::TreeNodeResponse& dto, QJsonObject& jsonOut);


bool validateTreeNodeRespJSON(const QJsonObject& json)
{
    if (!json.contains("fileId") || !json["fileId"].isDouble())
    {
        qWarning() << "invalid or missing 'fileId' field in JSON";
        return false;
    }

    if (!json.contains("name") || !json["name"].isString())
    {
        qWarning() << "invalid or missing 'name' field in JSON";
        return false;
    }

    if (!json.contains("isDirectory") || !json["isDirectory"].isBool())
    {
        qWarning() << "invalid or missing 'isDirectory' field in JSON";
        return false;
    }

    if (json["isDirectory"].toBool())
    {
        // directory must have 'children' field
        // it can be empty though
        if (!json.contains("children") || !json["children"].isArray())
        {
            qWarning() << "invalid or missing 'children' field in JSON";
            return false;
        }
    } else {
        // file must have 'size' field
        if (!json.contains("size") || !json["size"].isDouble())
        {
            qWarning() << "invalid or missing 'size' field in JSON";
            return false;
        }
    }

    return true;
}

void parseJsonNodeRequiredFields(
    const QJsonObject& json,
    DTO::File::TreeNodeResponse& dtoOut
    )
{
    dtoOut.fileId = json["fileId"].toInt();
    dtoOut.name = json["name"].toString();
    dtoOut.isDirectory = json["isDirectory"].toBool();
}

void parseJsonFileRequiredFields(
    const QJsonObject& json,
    DTO::File::TreeNodeResponse& dtoOut
    )
{
    dtoOut.children = std::nullopt; // file doesn't have 'children' field
    dtoOut.size = json["size"].toInteger(); // file must have 'size' field
}

void parseJsonDirRequiredFields(
    const QJsonObject& json,
    DTO::File::TreeNodeResponse& dtoOut
    )
{
    dtoOut.size = std::nullopt; // directory doesn't have 'size' field
    // directory must have 'children' field
    // it can be empty though
    dtoOut.children = QList<DTO::File::TreeNodeResponse>();
}


bool validateTreeNodeDTO(const DTO::File::TreeNodeResponse& dto)
{
    if (dto.isDirectory)
    {
        if (!dto.children.has_value())
        {
            qWarning() << "directory must have 'children' field value";
            return false;
        }
        else if (dto.size.has_value())
        {
            qWarning() << "directory must not have 'size' field value";
            return false;
        }
    }
    else
    {
        if (dto.children.has_value())
        {
            qWarning() << "file must not have 'children' field value";
            return false;
        }
        else if (!dto.size.has_value())
        {
            qWarning() << "file must have 'size' field value";
            return false;
        }
    }
    return true;
}

void serializeTreeNodeGeneralFields(
    const DTO::File::TreeNodeResponse& dto,
    QJsonObject& jsonOut
    )
{
    jsonOut["fileId"] = dto.fileId;
    jsonOut["name"] = dto.name;
    jsonOut["isDirectory"] = dto.isDirectory;
}

void serializeTreeNodeFileFields(
    const DTO::File::TreeNodeResponse& dto,
    QJsonObject& jsonOut
    )
{
    jsonOut["size"] = dto.size.value();
}


} // === end private namespace ===


// ==========================================
// [DTO::File::CreateEmptyRequest]
// ==========================================
std::optional<DTO::File::CreateEmptyRequest> fromJsonCreateEmptyRequest(const QJsonObject& json)
{
    DTO::File::CreateEmptyRequest dto;

    if (!json.contains("fileName") || !json["fileName"].isString())
    {
        qWarning() << "invalid or missing 'fileName' field in JSON";
        return std::nullopt;
    }

    if (!json.contains("parentId") ||
        (!json["parentId"].isDouble() && !json["parentId"].isNull()))
    {
        qWarning() << "invalid or missing 'parentId' field in JSON";
        return std::nullopt;
    }

    if (!json.contains("type") || !json["type"].isString())
    {
        qWarning() << "invalid or missing 'type' field in JSON";
        return std::nullopt;
    }

    if (!json.contains("overwrite") || !json["overwrite"].isBool())
    {
        qWarning() << "invalid or missing 'overwrite' field in JSON";
        return std::nullopt;
    }

    dto.type = FileTypeConverter::fromString(json["type"].toString());
    if (dto.type == FileType::Unknown)
    {
        qWarning() << "field 'type' has unknown value";
        return std::nullopt;
    }

    dto.fileName = json["fileName"].toString();
    dto.parentId = json["parentId"].isNull() ?
                       std::optional<int>(std::nullopt) :
                       json["parentId"].toInt();
    dto.overwrite = json["overwrite"].toBool();

    return dto;
}

QJsonObject toJson(const DTO::File::CreateEmptyRequest& dto)
{
    QJsonObject obj;

    if (dto.fileName.isEmpty())
    {
        qDebug() << "serializing empty fileName";
    }

    if (dto.parentId.has_value() && dto.parentId.value() < 0)
    {
        qDebug() << "serializing negative parentId";
    }

    if (dto.type == FileType::Unknown)
    {
        qDebug() << "serializing unknown type";
    }

    obj["fileName"] = dto.fileName;
    obj["parentId"] = dto.parentId.has_value()?
                          dto.parentId.value() : QJsonValue();
    obj["type"] = FileTypeConverter::toString(dto.type);
    obj["overwrite"] = dto.overwrite;

    return obj;
}


// ==========================================
// [DTO::File::CreateEmptyResponse]
// ==========================================
std::optional<DTO::File::CreateEmptyResponse> fromJsonCreateEmptyResponse(const QJsonObject& json)
{
    DTO::File::CreateEmptyResponse dto;

    if (!json.contains("createdAt") || !json["createdAt"].isString())
    {
        qWarning() << "invalid or missing 'createdAt' field in JSON";
        return std::nullopt;
    }
    if (!json.contains("fileId") || !json["fileId"].isDouble())
    {
        qWarning() << "invalid or missing 'fileId' field in JSON";
        return std::nullopt;
    }
    if (!json.contains("fileName") || !json["fileName"].isString())
    {
        qWarning() << "invalid or missing 'fileName' field in JSON";
        return std::nullopt;
    }

    if (!json.contains("parentId") ||
        (!json["parentId"].isDouble() && !json["parentId"].isNull()))
    {
        qWarning() << "invalid or missing 'parentId' field in JSON";
        return std::nullopt;
    }

    dto.createdAt = QDateTime::fromString(json["createdAt"].toString(), Qt::ISODate);
    if (!dto.createdAt.isValid())
    {
        qWarning() << "invalid 'createdAt' date format (expected ISO 8601)";
        return std::nullopt;
    }

    dto.fileId = json["fileId"].toInt();
    dto.fileName = json["fileName"].toString();
    dto.parentId = json["parentId"].isNull() ?
                       std::optional<int>(std::nullopt) :
                       json["parentId"].toInt();

    return dto;
}

QJsonObject toJson(const DTO::File::CreateEmptyResponse& dto)
{
    QJsonObject obj;

    if (!dto.createdAt.isValid())
        qDebug() << "serializing invalid createdAt date";
    if (dto.fileName.isEmpty())
        qDebug() << "serializing empty fileName";

    obj["createdAt"] = dto.createdAt.toString(Qt::ISODate);
    obj["fileId"] = dto.fileId;
    obj["fileName"] = dto.fileName;
    obj["parentId"] = dto.parentId.has_value()?
                          dto.parentId.value() : QJsonValue();

    return obj;
}


// ==========================================
// [DTO::File::MetadataResponse]
// ==========================================
std::optional<DTO::File::MetadataResponse> fromJsonMetadataResponse(const QJsonObject& json)
{
    DTO::File::MetadataResponse dto;

    if (!json.contains("createdAt") || !json["createdAt"].isString())
    {
        qWarning() << "invalid or missing 'createdAt' field in JSON";
        return std::nullopt;
    }
    if (!json.contains("type") || !json["type"].isString())
    {
        qWarning() << "invalid or missing 'type' field in JSON";
        return std::nullopt;
    }
    if (!json.contains("fileId") || !json["fileId"].isDouble())
    {
        qWarning() << "invalid or missing 'fileId' field in JSON";
        return std::nullopt;
    }
    if (!json.contains("fileName") || !json["fileName"].isString())
    {
        qWarning() << "invalid or missing 'fileName' field in JSON";
        return std::nullopt;
    }
    if (!json.contains("size") || !json["size"].isDouble())
    {
        qWarning() << "invalid or missing 'size' field in JSON";
        return std::nullopt;
    }
    if (!json.contains("parentId") ||
        (!json["parentId"].isDouble() && !json["parentId"].isNull()))
    {
        qWarning() << "invalid or missing 'parentId' field in JSON";
        return std::nullopt;
    }

    dto.createdAt = QDateTime::fromString(json["createdAt"].toString(), Qt::ISODate);
    if (!dto.createdAt.isValid())
    {
        qWarning() << "invalid 'createdAt' date format";
        return std::nullopt;
    }

    QString typeStr = json["type"].toString();
    FileType fileType = FileTypeConverter::fromString(typeStr);

    if (fileType == FileType::Unknown)
    {
        qWarning() << "invalid 'type' value";
        return std::nullopt;
    }

    dto.type = fileType;
    dto.fileId = json["fileId"].toInt();
    dto.fileName = json["fileName"].toString();
    dto.size = json["size"].toInteger();
    dto.parentId = json["parentId"].isNull() ?
                       std::optional<int>(std::nullopt) :
                       json["parentId"].toInt();

    return dto;
}

QJsonObject toJson(const DTO::File::MetadataResponse& dto)
{
    QJsonObject obj;

    obj["createdAt"] = dto.createdAt.toString(Qt::ISODate);
    obj["fileId"] = dto.fileId;
    obj["fileName"] = dto.fileName;
    obj["size"] = dto.size;
    obj["type"] = FileTypeConverter::toString(dto.type);
    obj["parentId"] = dto.parentId.has_value()?
                          dto.parentId.value() : QJsonValue();

    return obj;
}


// ==========================================
// [DTO::File::RenameRequest]
// ==========================================
std::optional<DTO::File::RenameRequest> fromJsonRenameRequest(const QJsonObject& json)
{
    DTO::File::RenameRequest dto;

    if (!json.contains("newFileName") || !json["newFileName"].isString())
    {
        qWarning() << "invalid or missing 'newFileName' field in JSON";
        return std::nullopt;
    }

    if (!json.contains("newParentId") || !json["newParentId"].isDouble() ||
        !json["newParentId"].isNull())
    {
        qWarning() << "invalid or missing 'newParentId' field in JSON";
        return std::nullopt;
    }

    dto.newFileName = json["newFileName"].toString();
    dto.newParentId = json["newParentId"].isNull() ?
                          std::optional<int>(std::nullopt) :
                          json["newParentId"].toInt();

    return dto;
}

QJsonObject toJson(const DTO::File::RenameRequest& dto)
{
    QJsonObject obj;

    if (dto.newFileName.isEmpty())
        qDebug() << "serializing empty newFileName";

    obj["newFileName"] = dto.newFileName;
    obj["newParentId"] = dto.newParentId.has_value()?
                          dto.newParentId.value() : QJsonValue();
    return obj;
}


// ==========================================
// [DTO::File::RenameResponse]
// ==========================================
std::optional<DTO::File::RenameResponse> fromJsonRenameResponse(const QJsonObject& json)
{
    DTO::File::RenameResponse dto;

    if (!json.contains("fileId") || !json["fileId"].isDouble())
    {
        qWarning() << "invalid or missing 'fileId' field in JSON";
        return std::nullopt;
    }
    if (!json.contains("fileName") || !json["fileName"].isString())
    {
        qWarning() << "invalid or missing 'fileName' field in JSON";
        return std::nullopt;
    }
    if (!json.contains("parentId") ||
        (!json["parentId"].isDouble() && !json["parentId"].isNull()))
    {
        qWarning() << "invalid or missing 'parentId' field in JSON";
        return std::nullopt;
    }

    dto.fileId = json["fileId"].toInt();
    dto.fileName = json["fileName"].toString();
    dto.parentId = json["parentId"].isNull() ?
                       std::optional<int>(std::nullopt) :
                       json["parentId"].toInt();

    return dto;
}

QJsonObject toJson(const DTO::File::RenameResponse& dto)
{
    QJsonObject obj;

    if (dto.fileName.isEmpty())
        qDebug() << "serializing empty fileName";

    obj["fileId"] = dto.fileId;
    obj["fileName"] = dto.fileName;
    obj["parentId"] = dto.parentId.has_value()?
                          dto.parentId.value() : QJsonValue();

    return obj;
}


// ==========================================
// [DTO::File::TreeNodeResponse]
// ==========================================
std::optional<DTO::File::TreeNodeResponse> fromJsonTreeNodeResponse(const QJsonObject& json)
{
    if (!validateTreeNodeRespJSON(json))
    {
        return std::nullopt;
    }

    DTO::File::TreeNodeResponse dto;
    parseJsonNodeRequiredFields(json, dto);

    if (dto.isDirectory)
    {
        parseJsonDirRequiredFields(json, dto);

        QJsonArray childrenArray = json["children"].toArray();
        auto childrenOpt = fromJsonTreeNodeArray(childrenArray);

        if (childrenOpt.has_value())
        {
            dto.children = std::move(*childrenOpt);
        }
        else
        {
            return std::nullopt;
        }
    }
    else
    {
        parseJsonFileRequiredFields(json, dto);
    }

    return dto;
}

QJsonObject toJson(const DTO::File::TreeNodeResponse& dto)
{
    QJsonObject json;
    if (!validateTreeNodeDTO(dto))
    {
        return json;
    }

    serializeTreeNodeGeneralFields(dto, json);

    if (dto.isDirectory)
    {
        json["children"] = toJson(*dto.children);
    }
    else
    {
        serializeTreeNodeFileFields(dto, json);
    }

    return json;
}

std::optional<QList<DTO::File::TreeNodeResponse>> fromJsonTreeNodeArray(const QJsonArray& jsonArray)
{
    QList<DTO::File::TreeNodeResponse> list;
    list.reserve(jsonArray.size());

    for (const QJsonValue& val : jsonArray)
    {
        if (!val.isObject())
        {
            qWarning() << "array element is not an object";
            return std::nullopt;
        }

        auto dtoOpt = fromJsonTreeNodeResponse(val.toObject());
        if (dtoOpt.has_value())
        {
            list.append(std::move(*dtoOpt));
        }
        else
        {
            return std::nullopt;
        }
    }

    return list;
}

QJsonArray toJson(const QList<DTO::File::TreeNodeResponse>& list)
{
    QJsonArray arr;
    for (const auto& item : list)
    {
        arr.append(toJson(item));
    }
    return arr;
}


// ==========================================
// [DTO::File::UploadCompleteResponse]
// ==========================================
std::optional<DTO::File::UploadCompleteResponse> fromJsonUploadCompleteResponse(const QJsonObject& json)
{
    DTO::File::UploadCompleteResponse dto;

    if (!json.contains("createdAt") || !json["createdAt"].isString())
    {
        qWarning() << "invalid or missing 'createdAt' field in JSON";
        return std::nullopt;
    }
    if (!json.contains("fileId") || !json["fileId"].isDouble())
    {
        qWarning() << "invalid or missing 'fileId' field in JSON";
        return std::nullopt;
    }
    if (!json.contains("fileName") || !json["fileName"].isString())
    {
        qWarning() << "invalid or missing 'fileName' field in JSON";
        return std::nullopt;
    }
    if (!json.contains("size") || !json["size"].isDouble())
    {
        qWarning() << "invalid or missing 'size' field in JSON";
        return std::nullopt;
    }

    dto.createdAt = QDateTime::fromString(json["createdAt"].toString(), Qt::ISODate);
    if (!dto.createdAt.isValid())
    {
        qWarning() << "invalid 'createdAt' date format (expected ISO 8601)";
        return std::nullopt;
    }

    if (!json.contains("parentId") ||
        (!json["parentId"].isDouble() && !json["parentId"].isNull()))
    {
        qWarning() << "invalid or missing 'parentId' field in JSON";
        return std::nullopt;
    }

    dto.fileId = json["fileId"].toInt();
    dto.fileName = json["fileName"].toString();
    dto.size = json["size"].toInteger();
    dto.parentId = json["parentId"].isNull() ?
                       std::optional<int>(std::nullopt) :
                       json["parentId"].toInt();

    return dto;
}

QJsonObject toJson(const DTO::File::UploadCompleteResponse& dto)
{
    QJsonObject obj;

    if (!dto.createdAt.isValid())
        qDebug() << "serializing invalid createdAt date";
    if (dto.fileName.isEmpty())
        qDebug() << "serializing empty fileName";

    obj["createdAt"] = dto.createdAt.toString(Qt::ISODate);
    obj["fileId"] = dto.fileId;
    obj["fileName"] = dto.fileName;
    obj["size"] = dto.size;
    obj["parentId"] = dto.parentId.has_value()?
                        dto.parentId.value() : QJsonValue();

    return obj;
}


// ==========================================
// [DTO::File::UploadInitRequest]
// ==========================================
std::optional<DTO::File::UploadInitRequest> fromJsonUploadInitRequest(const QJsonObject& json)
{
    DTO::File::UploadInitRequest dto;

    if (!json.contains("fileName") || !json["fileName"].isString())
    {
        qWarning() << "invalid or missing 'fileName' field in JSON";
        return std::nullopt;
    }
    if (!json.contains("fileSize") || !json["fileSize"].isDouble())
    {
        qWarning() << "invalid or missing 'fileSize' field in JSON";
        return std::nullopt;
    }
    if (!json.contains("overwrite") || !json["overwrite"].isBool())
    {
        qWarning() << "invalid or missing 'overwrite' field in JSON";
        return std::nullopt;
    }

    if (!json.contains("parentId") ||
        (!json["parentId"].isDouble() && !json["parentId"].isNull()))
    {
        qWarning() << "invalid or missing 'parentId' field in JSON";
        return std::nullopt;
    }

    dto.fileName = json["fileName"].toString();
    dto.fileSize = json["fileSize"].toInteger();
    dto.overwrite = json["overwrite"].toBool();
    dto.parentId = json["parentId"].isNull() ?
                       std::optional<int>(std::nullopt) :
                       json["parentId"].toInt();

    return dto;
}

QJsonObject toJson(const DTO::File::UploadInitRequest& dto)
{
    QJsonObject obj;

    if (dto.fileName.isEmpty())
        qDebug() << "serializing empty fileName";

    obj["fileName"] = dto.fileName;
    obj["fileSize"] = dto.fileSize;
    obj["overwrite"] = dto.overwrite;
    obj["parentId"] = dto.parentId.has_value()?
                          dto.parentId.value() : QJsonValue();

    return obj;
}


// ==========================================
// [DTO::File::UploadInitResponse]
// ==========================================
std::optional<DTO::File::UploadInitResponse> fromJsonUploadInitResponse(const QJsonObject& json)
{
    DTO::File::UploadInitResponse dto;

    if (!json.contains("chunkSize") || !json["chunkSize"].isDouble())
    {
        qWarning() << "invalid or missing 'chunkSize' field in JSON";
        return std::nullopt;
    }
    if (!json.contains("uploadId") || !json["uploadId"].isString())
    {
        qWarning() << "invalid or missing 'uploadId' field in JSON";
        return std::nullopt;
    }
    if (!json.contains("expiresAt") || !json["expiresAt"].isString())
    {
        qWarning() << "invalid or missing 'expiresAt' field in JSON";
        return std::nullopt;
    }

    dto.chunkSize = json["chunkSize"].toInteger();
    dto.uploadId = json["uploadId"].toString();
    dto.expiresAt = QDateTime::fromString(json["expiresAt"].toString(), Qt::ISODate);

    if (!dto.expiresAt.isValid())
    {
        qWarning() << "invalid 'expiresAt' date format (expected ISO 8601)";
        return std::nullopt;
    }

    return dto;
}

QJsonObject toJson(const DTO::File::UploadInitResponse& dto)
{
    QJsonObject obj;

    if (dto.uploadId.isEmpty())
        qDebug() << "serializing empty uploadId";
    if (!dto.expiresAt.isValid())
        qDebug() << "serializing invalid expiresAt date";

    obj["chunkSize"] = dto.chunkSize;
    obj["uploadId"] = dto.uploadId;
    obj["expiresAt"] = dto.expiresAt.toString(Qt::ISODate);

    return obj;
}

}
