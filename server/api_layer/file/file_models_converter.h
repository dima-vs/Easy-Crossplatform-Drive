#ifndef FILE_MODELS_CONVERTER_H
#define FILE_MODELS_CONVERTER_H

#include <QList>
#include "file/models.h"
#include "dto/file/create_empty_request.h"
#include "dto/file/create_empty_response.h"
#include "dto/file/metadata_response.h"
#include "dto/file/rename_request.h"
#include "dto/file/rename_response.h"
#include "dto/file/tree_response.h"
#include "dto/file/upload_complete_response.h"
#include "dto/file/upload_init_request.h"
#include "dto/file/upload_init_response.h"

namespace Api::Converters
{

class FileModelsConverter
{
public:
    static DTO::File::TreeNodeResponse fileNodeToDTO(
        const ServiceModel::File::FileNode& node
        );

    static QList<DTO::File::TreeNodeResponse> fileNodeListToDTO(
        const QList<ServiceModel::File::FileNode>& nodes
        );

    static DTO::File::UploadInitResponse initUploadResToDTO(
        const ServiceModel::File::UploadSessionResult& result
        );

    static DTO::File::UploadCompleteResponse completeUploadResToDTO(
        const ServiceModel::File::CompleteUploadResult& result
        );

    static DTO::File::RenameResponse renameResToDTO(
        const ServiceModel::File::RenameResult& result
        );

    static DTO::File::CreateEmptyResponse createEmptyResToDTO(
        const ServiceModel::File::CreatedFileObjectResult& result
        );
};

}

#endif // FILE_MODELS_CONVERTER_H
