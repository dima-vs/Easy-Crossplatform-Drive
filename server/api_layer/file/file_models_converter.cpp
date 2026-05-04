#include "file_models_converter.h"
#include "domain/file_type.h"

namespace Api::Converters
{

DTO::File::TreeNodeResponse FileModelsConverter::fileNodeToDTO(
    const ServiceModel::File::FileNode& node
    )
{
    DTO::File::TreeNodeResponse dto;

    dto.fileId = node.id;
    dto.name = node.logicalName;

    dto.isDirectory = (node.type == Common::Domain::FileType::Directory);

    if (dto.isDirectory)
    {
        QList<DTO::File::TreeNodeResponse> dtoChildren;
        for (const auto& childNode : node.children)
        {
            dtoChildren.append(fileNodeToDTO(childNode));
        }
        dto.children = dtoChildren;
    }
    else
    {
        dto.size = node.size.toLongLong();
    }

    return dto;
}

QList<DTO::File::TreeNodeResponse> FileModelsConverter::fileNodeListToDTO(
    const QList<ServiceModel::File::FileNode>& nodes
    )
{
    QList<DTO::File::TreeNodeResponse> dtoList;
    dtoList.reserve(nodes.size());

    for (const auto& node : nodes)
    {
        dtoList.append(fileNodeToDTO(node));
    }

    return dtoList;
}

DTO::File::UploadInitResponse FileModelsConverter::initUploadResToDTO(
    const ServiceModel::File::UploadSessionResult &result
    )
{
    DTO::File::UploadInitResponse dto;

    dto.uploadId = result.uploadId;
    dto.chunkSize = result.chunkSize;
    dto.expiresAt = result.expiresAt;

    return dto;
}

DTO::File::UploadCompleteResponse FileModelsConverter::completeUploadResToDTO(
    const ServiceModel::File::CompleteUploadResult& result
    )
{
    DTO::File::UploadCompleteResponse dto;
    dto.createdAt = result.createdAt;
    dto.fileId = result.fileId;
    dto.fileName = result.fileName;
    dto.size = result.size;

    if (result.parentId.isNull()) {
        dto.parentId = std::nullopt;
    } else {
        dto.parentId = result.parentId.toInt();
    }

    return dto;
}

DTO::File::RenameResponse FileModelsConverter::renameResToDTO(
    const ServiceModel::File::RenameResult& result
    )
{
    DTO::File::RenameResponse dto;
    dto.fileId = result.fileRecordId;
    dto.fileName = result.logicalName;

    if (result.parentId.isNull()) {
        dto.parentId = std::nullopt;
    } else {
        dto.parentId = result.parentId.toInt();
    }

    return dto;
}

DTO::File::CreateEmptyResponse FileModelsConverter::createEmptyResToDTO(
    const ServiceModel::File::CreatedFileObjectResult& result
    )
{
    DTO::File::CreateEmptyResponse dto;
    dto.createdAt = result.createdAt;
    dto.fileId = result.fileId;
    dto.fileName = result.fileName;

    if (result.parentId.isNull()) {
        dto.parentId = std::nullopt;
    } else {
        dto.parentId = result.parentId.toInt();
    }

    return dto;
}

} // namespace Api::Converters
