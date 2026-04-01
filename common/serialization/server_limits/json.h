#include <QJsonObject>
#include <optional>
#include "dto/server_limits/transfer.h"

namespace Serialization::ServerLimits
{

// [DTO::ServerLimits::TransferResponse]
std::optional<DTO::ServerLimits::TransferResponse> fromJsonTransferResponse(const QJsonObject& json);
QJsonObject toJson(const DTO::ServerLimits::TransferResponse& dto);

}
