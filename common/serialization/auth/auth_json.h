#include <QJsonObject>
#include <optional>
#include "dto/auth_dto/auth_response_dto.h"
#include "dto/auth_dto/confirm_register_request_dto.h"
#include "dto/auth_dto/login_request_dto.h"
#include "dto/auth_dto/register_init_request_dto.h"
#include "dto/auth_dto/register_init_response_dto.h"

namespace Serialization::Auth
{

// [RegisterInitRequestDTO]
std::optional<RegisterInitRequestDTO> fromJsonRegisterInitRequest(const QJsonObject& json);
QJsonObject toJson(const RegisterInitRequestDTO& dto);

// [RegisterInitResponseDTO]
std::optional<RegisterInitResponseDTO> fromJsonRegisterInitResponse(const QJsonObject& json);
QJsonObject toJson(const RegisterInitResponseDTO& dto);

// [LoginRequestDTO]
std::optional<LoginRequestDTO> fromJsonLoginRequest(const QJsonObject& json);
QJsonObject toJson(const LoginRequestDTO& dto);

// [ConfirmRegisterRequestDTO]
std::optional<ConfirmRegisterRequestDTO> fromJsonConfirmRegisterRequest(const QJsonObject& json);
QJsonObject toJson(const ConfirmRegisterRequestDTO& dto);

// [AuthResponseDTO]
std::optional<AuthResponseDTO> fromJsonAuthResponse(const QJsonObject& json);
QJsonObject toJson(const AuthResponseDTO& dto);

}
