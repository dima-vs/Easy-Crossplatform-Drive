#include <QJsonObject>
#include <optional>
#include "domain/file_type.h"
#include "converter/file_type_converter.h"

#include "dto/file/create_empty_request.h"
#include "dto/file/create_empty_response.h"
#include "dto/file/metadata_response.h"
#include "dto/file/rename_request.h"
#include "dto/file/rename_response.h"
#include "dto/file/tree_response.h"
#include "dto/file/upload_complete_response.h"
#include "dto/file/upload_init_request.h"
#include "dto/file/upload_init_response.h"

namespace Serialization::File
{

using FileType = Common::Domain::FileType;
using FileTypeConverter = Common::Converter::FileTypeConverter;

// [DTO::File::CreateEmptyRequest]
std::optional<DTO::File::CreateEmptyRequest> fromJsonCreateEmptyRequest(const QJsonObject& json);
QJsonObject toJson(const DTO::File::CreateEmptyRequest& dto);

// [DTO::File::CreateEmptyResponse]
std::optional<DTO::File::CreateEmptyResponse> fromJsonCreateEmptyResponse(const QJsonObject& json);
QJsonObject toJson(const DTO::File::CreateEmptyResponse& dto);

// [DTO::File::MetadataResponse]
std::optional<DTO::File::MetadataResponse> fromJsonMetadataResponse(const QJsonObject& json);
QJsonObject toJson(const DTO::File::MetadataResponse& dto);

// [DTO::File::RenameRequest]
std::optional<DTO::File::RenameRequest> fromJsonRenameRequest(const QJsonObject& json);
QJsonObject toJson(const DTO::File::RenameRequest& dto);

// [DTO::File::RenameResponse]
std::optional<DTO::File::RenameResponse> fromJsonRenameResponse(const QJsonObject& json);
QJsonObject toJson(const DTO::File::RenameResponse& dto);

// [DTO::File::TreeNodeResponse]
std::optional<QList<DTO::File::TreeNodeResponse>> fromJsonTreeNodeArray(const QJsonArray& jsonArray);
QJsonArray toJson(const QList<DTO::File::TreeNodeResponse>& list);
std::optional<DTO::File::TreeNodeResponse> fromJsonTreeNodeResponse(const QJsonObject& json);
QJsonObject toJson(const DTO::File::TreeNodeResponse& dto);

// [DTO::File::UploadCompleteResponse]
std::optional<DTO::File::UploadCompleteResponse> fromJsonUploadCompleteResponse(const QJsonObject& json);
QJsonObject toJson(const DTO::File::UploadCompleteResponse& dto);

// [DTO::File::UploadInitRequest]
std::optional<DTO::File::UploadInitRequest> fromJsonUploadInitRequest(const QJsonObject& json);
QJsonObject toJson(const DTO::File::UploadInitRequest& dto);

// [DTO::File::UploadInitResponse]
std::optional<DTO::File::UploadInitResponse> fromJsonUploadInitResponse(const QJsonObject& json);
QJsonObject toJson(const DTO::File::UploadInitResponse& dto);

}
