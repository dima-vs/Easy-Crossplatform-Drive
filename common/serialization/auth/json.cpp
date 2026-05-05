#include <QDebug>
#include <QDateTime>
#include <QString>
#include "serialization/auth/json.h"
#include "serialization/auth/json_keys.h"
#include "serialization/json_helpers.h"

namespace Serialization::Auth
{

// ==========================================
// [DTO::Auth::RegisterInitRequest]
// ==========================================
std::optional<DTO::Auth::RegisterInitRequest> fromJsonRegisterInitRequest(const QJsonObject& json)
{
    using Keys = JsonKeys::Auth::RegisterInit;
    DTO::Auth::RegisterInitRequest dto;

    if (!JsonHelper::requireString(json, Keys::Email, dto.email))
        return std::nullopt;

    return dto;
}

QJsonObject toJson(const DTO::Auth::RegisterInitRequest& dto)
{
    using Keys = JsonKeys::Auth::RegisterInit;
    QJsonObject obj;

    if (dto.email.isEmpty())
    {
        qDebug() << "serializing empty email";
    }

    obj[Keys::Email] = dto.email;
    return obj;
}


// ==========================================
// [DTO::Auth::RegisterInitResponse]
// ==========================================
std::optional<DTO::Auth::RegisterInitResponse> fromJsonRegisterInitResponse(const QJsonObject& json)
{
    using Keys = JsonKeys::Auth::RegisterInit;
    DTO::Auth::RegisterInitResponse dto;

    if (!JsonHelper::requireString(json, Keys::VerificationId, dto.verificationId) ||
        !JsonHelper::requireDateTime(json, Keys::ExpiresAt, dto.expiresAt))
        return std::nullopt;

    return dto;
}

QJsonObject toJson(const DTO::Auth::RegisterInitResponse& dto)
{
    using Keys = JsonKeys::Auth::RegisterInit;
    QJsonObject obj;

    if (dto.verificationId.isEmpty())
    {
        qDebug() << "serializing empty verificationId";
    }
    if (!dto.expiresAt.isValid())
    {
        qDebug() << "serializing invalid expiresAt date";
    }

    obj[Keys::VerificationId] = dto.verificationId;
    obj[Keys::ExpiresAt] = dto.expiresAt.toString(Qt::ISODate);

    return obj;
}


// ==========================================
// [DTO::Auth::LoginRequest]
// ==========================================
std::optional<DTO::Auth::LoginRequest> fromJsonLoginRequest(const QJsonObject& json)
{
    using Keys = JsonKeys::Auth::Login;
    DTO::Auth::LoginRequest dto;

    if (!JsonHelper::requireString(json, Keys::LoginField, dto.login) ||
        !JsonHelper::requireString(json, Keys::Password, dto.password))
        return std::nullopt;

    return dto;
}

QJsonObject toJson(const DTO::Auth::LoginRequest& dto)
{
    using Keys = JsonKeys::Auth::Login;
    QJsonObject obj;

    if (dto.login.isEmpty())
    {
        qDebug() << "serializing empty login";
    }
    if (dto.password.isEmpty())
    {
        qDebug() << "serializing empty password";
    }

    obj[Keys::LoginField] = dto.login;
    obj[Keys::Password] = dto.password;

    return obj;
}


// ==========================================
// [DTO::Auth::ConfirmRegisterRequest]
// ==========================================
std::optional<DTO::Auth::ConfirmRegisterRequest> fromJsonConfirmRegisterRequest(const QJsonObject& json)
{
    using Keys = JsonKeys::Auth::ConfirmRegister;
    DTO::Auth::ConfirmRegisterRequest dto;

    if (!JsonHelper::requireString(json, Keys::VerificationId, dto.verificationId) ||
        !JsonHelper::requireInt(json, Keys::AccessCode, dto.accessCode) ||
        !JsonHelper::requireString(json, Keys::Username, dto.username) ||
        !JsonHelper::requireString(json, Keys::Password, dto.password))
        return std::nullopt;

    return dto;
}

QJsonObject toJson(const DTO::Auth::ConfirmRegisterRequest& dto)
{
    using Keys = JsonKeys::Auth::ConfirmRegister;
    QJsonObject obj;

    if (dto.verificationId.isEmpty())
    {
        qDebug() << "serializing empty verificationId";
    }
    if (dto.username.isEmpty())
    {
        qDebug() << "serializing empty username";
    }
    if (dto.password.isEmpty())
    {
        qDebug() << "serializing empty password";
    }

    obj[Keys::VerificationId] = dto.verificationId;
    obj[Keys::AccessCode] = dto.accessCode;
    obj[Keys::Username] = dto.username;
    obj[Keys::Password] = dto.password;

    return obj;
}


// ==========================================
// [DTO::Auth::GeneralResponse]
// ==========================================
std::optional<DTO::Auth::GeneralResponse> fromJsonGeneralResponse(const QJsonObject& json)
{
    using Keys = JsonKeys::Auth::General;
    DTO::Auth::GeneralResponse dto;

    if (!JsonHelper::requireString(json, Keys::AccessToken, dto.accessToken) ||
        !JsonHelper::requireDateTime(json, Keys::ExpiresAt, dto.expiresAt))
        return std::nullopt;

    return dto;
}

QJsonObject toJson(const DTO::Auth::GeneralResponse& dto)
{
    using Keys = JsonKeys::Auth::General;
    QJsonObject obj;

    if (dto.accessToken.isEmpty())
    {
        qDebug() << "serializing empty accessToken";
    }
    if (!dto.expiresAt.isValid())
    {
        qDebug() << "serializing invalid expiresAt date";
    }

    obj[Keys::AccessToken] = dto.accessToken;
    obj[Keys::ExpiresAt] = dto.expiresAt.toString(Qt::ISODate);

    return obj;
}

}
