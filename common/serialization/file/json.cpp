#include <QDebug>
#include <QDateTime>
#include <QString>
#include <QList>
#include <QJsonValue>
#include <QJsonArray>
#include <utility>
#include "serialization/file/json.h"
#include "serialization/file/json_keys.h"
#include "serialization/json_helpers.h"

namespace Serialization::File
{


namespace Keys = JsonKeys::File;

// ==========================================
// [DTO::File::CreateEmptyRequest]
// ==========================================
std::optional<DTO::File::CreateEmptyRequest> fromJsonCreateEmptyRequest(const QJsonObject& json)
{
    DTO::File::CreateEmptyRequest dto;
    QString typeStr;

    if (!JsonHelper::requireString(json, Keys::Name, dto.fileName) ||
        !JsonHelper::requireNullableInt(json, Keys::ParentId, dto.parentId) ||
        !JsonHelper::requireString(json, Keys::Type, typeStr) ||
        !JsonHelper::requireBool(json, Keys::Overwrite, dto.overwrite))
    {
        return std::nullopt;
    }

    dto.type = Common::Converter::FileTypeConverter::fromString(typeStr);
    if (dto.type == Common::Domain::FileType::Unknown)
    {
        qWarning() << "field 'type' has unknown value";
        return std::nullopt;
    }

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

    obj[Keys::Name] = dto.fileName;
    obj[Keys::ParentId] = JsonHelper::getNullableInt(dto.parentId);
    obj[Keys::Type] = Common::Converter::FileTypeConverter::toString(dto.type);
    obj[Keys::Overwrite] = dto.overwrite;

    return obj;
}


// ==========================================
// [DTO::File::CreateEmptyResponse]
// ==========================================
std::optional<DTO::File::CreateEmptyResponse> fromJsonCreateEmptyResponse(const QJsonObject& json)
{
    DTO::File::CreateEmptyResponse dto;

    if (!JsonHelper::requireDateTime(json, Keys::CreatedAt, dto.createdAt) ||
        !JsonHelper::requireInt(json, Keys::Id, dto.fileId) ||
        !JsonHelper::requireString(json, Keys::Name, dto.fileName) ||
        !JsonHelper::requireNullableInt(json, Keys::ParentId, dto.parentId))
    {
        return std::nullopt;
    }

    return dto;
}

QJsonObject toJson(const DTO::File::CreateEmptyResponse& dto)
{
    QJsonObject obj;

    if (!dto.createdAt.isValid())
        qDebug() << "serializing invalid createdAt date";
    if (dto.fileName.isEmpty())
        qDebug() << "serializing empty fileName";

    obj[Keys::CreatedAt] = dto.createdAt.toString(Qt::ISODate);
    obj[Keys::Id] = dto.fileId;
    obj[Keys::Name] = dto.fileName;
    obj[Keys::ParentId] = JsonHelper::getNullableInt(dto.parentId);

    return obj;
}


// ==========================================
// [DTO::File::MetadataResponse]
// ==========================================
std::optional<DTO::File::MetadataResponse> fromJsonMetadataResponse(const QJsonObject& json)
{
    DTO::File::MetadataResponse dto;
    QString typeStr;

    if (!JsonHelper::requireDateTime(json, Keys::CreatedAt, dto.createdAt) ||
        !JsonHelper::requireString(json, Keys::Type, typeStr) ||
        !JsonHelper::requireInt(json, Keys::Id, dto.fileId) ||
        !JsonHelper::requireString(json, Keys::Name, dto.fileName) ||
        !JsonHelper::requireInt64(json, Keys::Size, dto.size) ||
        !JsonHelper::requireNullableInt(json, Keys::ParentId, dto.parentId))
    {
        return std::nullopt;
    }

    dto.type = Common::Converter::FileTypeConverter::fromString(typeStr);
    if (dto.type == Common::Domain::FileType::Unknown)
    {
        qWarning() << "invalid 'type' value";
        return std::nullopt;
    }

    return dto;
}

QJsonObject toJson(const DTO::File::MetadataResponse& dto)
{
    QJsonObject obj;

    obj[Keys::CreatedAt] = dto.createdAt.toString(Qt::ISODate);
    obj[Keys::Type] = Common::Converter::FileTypeConverter::toString(dto.type);
    obj[Keys::Id] = dto.fileId;
    obj[Keys::Name] = dto.fileName;
    obj[Keys::Size] = dto.size;
    obj[Keys::ParentId] = JsonHelper::getNullableInt(dto.parentId);

    return obj;
}


// ==========================================
// [DTO::File::RenameRequest]
// ==========================================
std::optional<DTO::File::RenameRequest> fromJsonRenameRequest(const QJsonObject& json)
{
    using KeysRename = JsonKeys::File::Rename;
    DTO::File::RenameRequest dto;

    if (!json.contains(KeysRename::NewName) &&
        !json.contains(KeysRename::NewParentId))
    {
        qWarning() << "RenameRequest must contain at least newFileName or newParentId";
        return std::nullopt;
    }

    if (!JsonHelper::extractOptionalString(
            json, KeysRename::NewName, dto.newFileName) ||
        !JsonHelper::extractOptionalParentId(
            json, KeysRename::NewParentId, dto.newParentId))
    {
        return std::nullopt;
    }

    return dto;
}

QJsonObject toJson(const DTO::File::RenameRequest& dto)
{
    using KeysRename = JsonKeys::File::Rename;
    QJsonObject obj;

    if (dto.newFileName.has_value())
    {
        obj[KeysRename::NewName] = dto.newFileName.value();
    }

    if (dto.newParentId.has_value())
    {
        QVariant val = dto.newParentId.value();
        obj[KeysRename::NewParentId] = val.isNull() ?
                                    QJsonValue(QJsonValue::Null) :
                                    QJsonValue(val.toInt());
    }

    return obj;
}


// ==========================================
// [DTO::File::RenameResponse]
// ==========================================
std::optional<DTO::File::RenameResponse> fromJsonRenameResponse(const QJsonObject& json)
{
    DTO::File::RenameResponse dto;

    if (!JsonHelper::requireInt(json, Keys::Id, dto.fileId) ||
        !JsonHelper::requireString(json, Keys::Name, dto.fileName) ||
        !JsonHelper::requireNullableInt(json, Keys::ParentId, dto.parentId))
    {
        return std::nullopt;
    }

    return dto;
}

QJsonObject toJson(const DTO::File::RenameResponse& dto)
{
    QJsonObject obj;

    if (dto.fileName.isEmpty())
        qDebug() << "serializing empty fileName";

    obj[Keys::Id] = dto.fileId;
    obj[Keys::Name] = dto.fileName;
    obj[Keys::ParentId] = JsonHelper::getNullableInt(dto.parentId);

    return obj;
}

// ==========================================
// [DTO::File::TreeNodeResponse]
// ==========================================
std::optional<QList<DTO::File::TreeNodeResponse>> fromJsonTreeNodeArray(const QJsonArray& jsonArray);

std::optional<DTO::File::TreeNodeResponse> fromJsonTreeNodeResponse(const QJsonObject& json)
{
    DTO::File::TreeNodeResponse dto;

    if (!JsonHelper::requireInt(json, Keys::Id, dto.fileId) ||
        !JsonHelper::requireString(json, Keys::Name, dto.name) ||
        !JsonHelper::requireBool(json, Keys::Tree::IsDirectory, dto.isDirectory))
    {
        return std::nullopt;
    }

    if (dto.isDirectory)
    {
        dto.size = std::nullopt;

        if (!json.contains(Keys::Tree::Children) ||
            !json[Keys::Tree::Children].isArray())
        {
            qWarning() << "invalid or missing 'children' array in JSON for directory";
            return std::nullopt;
        }

        auto childrenOpt = fromJsonTreeNodeArray(
            json[Keys::Tree::Children].toArray()
            );

        if (!childrenOpt.has_value())
            return std::nullopt;

        dto.children = std::move(*childrenOpt);
    }
    else
    {
        dto.children = std::nullopt;

        qint64 size;
        if (!JsonHelper::requireInt64(json, Keys::Size, size))
        {
            return std::nullopt;
        }
        dto.size = size;
    }

    return dto;
}

QJsonObject toJson(const DTO::File::TreeNodeResponse& dto)
{
    QJsonObject json;

    if (dto.isDirectory && !dto.children.has_value())
    {
        qWarning() << "directory must have 'children' field value";
        return json;
    }
    if (!dto.isDirectory && !dto.size.has_value())
    {
        qWarning() << "file must have 'size' field value";
        return json;
    }

    json[Keys::Id] = dto.fileId;
    json[Keys::Name] = dto.name;
    json[Keys::Tree::IsDirectory] = dto.isDirectory;

    if (dto.isDirectory)
    {
        json[Keys::Tree::Children] = toJson(dto.children.value());
    }
    else
    {
        json[Keys::Size] = dto.size.value();
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
        if (!dtoOpt.has_value())
        {
            return std::nullopt;
        }

        list.append(std::move(*dtoOpt));
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

    if (!JsonHelper::requireDateTime(json, Keys::CreatedAt, dto.createdAt) ||
        !JsonHelper::requireInt(json, Keys::Id, dto.fileId) ||
        !JsonHelper::requireString(json, Keys::Name, dto.fileName) ||
        !JsonHelper::requireInt64(json, Keys::Size, dto.size) ||
        !JsonHelper::requireNullableInt(json, Keys::ParentId, dto.parentId))
    {
        return std::nullopt;
    }

    return dto;
}

QJsonObject toJson(const DTO::File::UploadCompleteResponse& dto)
{
    QJsonObject obj;

    if (!dto.createdAt.isValid())
        qDebug() << "serializing invalid createdAt date";
    if (dto.fileName.isEmpty())
        qDebug() << "serializing empty fileName";

    obj[Keys::CreatedAt] = dto.createdAt.toString(Qt::ISODate);
    obj[Keys::Id] = dto.fileId;
    obj[Keys::Name] = dto.fileName;
    obj[Keys::Size] = dto.size;
    obj[Keys::ParentId] = JsonHelper::getNullableInt(dto.parentId);

    return obj;
}


// ==========================================
// [DTO::File::UploadInitRequest]
// ==========================================
std::optional<DTO::File::UploadInitRequest> fromJsonUploadInitRequest(const QJsonObject& json)
{
    DTO::File::UploadInitRequest dto;

    if (!JsonHelper::requireString(json, Keys::Name, dto.fileName) ||
        !JsonHelper::requireInt64(json, Keys::Size, dto.fileSize) ||
        !JsonHelper::requireBool(json, Keys::Overwrite, dto.overwrite) ||
        !JsonHelper::requireNullableInt(json, Keys::ParentId, dto.parentId))
    {
        return std::nullopt;
    }

    return dto;
}

QJsonObject toJson(const DTO::File::UploadInitRequest& dto)
{
    QJsonObject obj;

    if (dto.fileName.isEmpty())
        qDebug() << "serializing empty fileName";

    obj[Keys::Name] = dto.fileName;
    obj[Keys::Size] = dto.fileSize;
    obj[Keys::Overwrite] = dto.overwrite;
    obj[Keys::ParentId] = JsonHelper::getNullableInt(dto.parentId);

    return obj;
}


// ==========================================
// [DTO::File::UploadInitResponse]
// ==========================================
std::optional<DTO::File::UploadInitResponse> fromJsonUploadInitResponse(const QJsonObject& json)
{
    DTO::File::UploadInitResponse dto;

    if (!JsonHelper::requireInt64(json, Keys::Upload::ChunkSize, dto.chunkSize) ||
        !JsonHelper::requireString(json, Keys::Upload::UploadId, dto.uploadId) ||
        !JsonHelper::requireDateTime(json, Keys::Upload::ExpiresAt, dto.expiresAt))
    {
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

    obj[Keys::Upload::ChunkSize] = dto.chunkSize;
    obj[Keys::Upload::UploadId] = dto.uploadId;
    obj[Keys::Upload::ExpiresAt] = dto.expiresAt.toString(Qt::ISODate);

    return obj;
}

}
