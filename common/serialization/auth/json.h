#include <QJsonObject>
#include <optional>
#include "dto/auth/general_response.h"
#include "dto/auth/confirm_register_request.h"
#include "dto/auth/login_request.h"
#include "dto/auth/register_init_request.h"
#include "dto/auth/register_init_response.h"

namespace Serialization::Auth
{

// [DTO::Auth::RegisterInitRequest]
std::optional<DTO::Auth::RegisterInitRequest> fromJsonRegisterInitRequest(const QJsonObject& json);
QJsonObject toJson(const DTO::Auth::RegisterInitRequest& dto);

// [DTO::Auth::RegisterInitResponse]
std::optional<DTO::Auth::RegisterInitResponse> fromJsonRegisterInitResponse(const QJsonObject& json);
QJsonObject toJson(const DTO::Auth::RegisterInitResponse& dto);

// [DTO::Auth::LoginRequest]
std::optional<DTO::Auth::LoginRequest> fromJsonLoginRequest(const QJsonObject& json);
QJsonObject toJson(const DTO::Auth::LoginRequest& dto);

// [DTO::Auth::ConfirmRegisterRequest]
std::optional<DTO::Auth::ConfirmRegisterRequest> fromJsonConfirmRegisterRequest(const QJsonObject& json);
QJsonObject toJson(const DTO::Auth::ConfirmRegisterRequest& dto);

// [DTO::Auth::GeneralResponse]
std::optional<DTO::Auth::GeneralResponse> fromJsonGeneralResponse(const QJsonObject& json);
QJsonObject toJson(const DTO::Auth::GeneralResponse& dto);

}
