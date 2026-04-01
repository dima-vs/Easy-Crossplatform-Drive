#include <QJsonObject>
#include <optional>
#include "dto/server_limits_dto/limits_transfer_dto.h"

namespace Serialization::ServerLimits
{

// [TransferLimitsResponseDTO]
std::optional<TransferLimitsResponseDTO> fromJsonTransferLimitsResponse(const QJsonObject& json);
QJsonObject toJson(const TransferLimitsResponseDTO& dto);

}
