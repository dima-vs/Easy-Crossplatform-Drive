#include <QDebug>
#include <QDateTime>
#include <QString>
#include "serialization/auth/json.h"

namespace Serialization::Auth
{

// ==========================================
// [DTO::Auth::RegisterInitRequest]
// ==========================================
std::optional<DTO::Auth::RegisterInitRequest> fromJsonRegisterInitRequest(const QJsonObject& json)
{
    DTO::Auth::RegisterInitRequest dto;

    if (json.contains("email") && json["email"].isString())
    {
        dto.email = json["email"].toString();
        return dto;
    } else {
        qWarning() << "invalid or missing 'email' field in JSON";
        return std::nullopt;
    }
}

QJsonObject toJson(const DTO::Auth::RegisterInitRequest& dto)
{
    QJsonObject obj;

    if (dto.email.isEmpty())
    {
        qDebug() << "serializing empty email";
    }

    obj["email"] = dto.email;
    return obj;
}


// ==========================================
// [DTO::Auth::RegisterInitResponse]
// ==========================================
std::optional<DTO::Auth::RegisterInitResponse> fromJsonRegisterInitResponse(const QJsonObject& json)
{
    DTO::Auth::RegisterInitResponse dto;

    if (!json.contains("verificationId") || !json["verificationId"].isString())
    {
        qWarning() << "invalid or missing 'verificationId' field in JSON";
        return std::nullopt;
    }

    if (!json.contains("expiresAt") || !json["expiresAt"].isString())
    {
        qWarning() << "invalid or missing 'expiresAt' field in JSON";
        return std::nullopt;
    }

    dto.verificationId = json["verificationId"].toString();
    dto.expiresAt = QDateTime::fromString(json["expiresAt"].toString(), Qt::ISODate);

    if (!dto.expiresAt.isValid())
    {
        qWarning() << "invalid 'expiresAt' date format (expected ISO 8601)";
        return std::nullopt;
    }

    return dto;
}

QJsonObject toJson(const DTO::Auth::RegisterInitResponse& dto)
{
    QJsonObject obj;

    if (dto.verificationId.isEmpty())
    {
        qDebug() << "serializing empty verificationId";
    }
    if (!dto.expiresAt.isValid())
    {
        qDebug() << "serializing invalid expiresAt date";
    }

    obj["verificationId"] = dto.verificationId;
    obj["expiresAt"] = dto.expiresAt.toString(Qt::ISODate);

    return obj;
}


// ==========================================
// [DTO::Auth::LoginRequest]
// ==========================================
std::optional<DTO::Auth::LoginRequest> fromJsonLoginRequest(const QJsonObject& json)
{
    DTO::Auth::LoginRequest dto;

    if (!json.contains("login") || !json["login"].isString())
    {
        qWarning() << "invalid or missing 'login' field in JSON";
        return std::nullopt;
    }
    if (!json.contains("password") || !json["password"].isString())
    {
        qWarning() << "invalid or missing 'password' field in JSON";
        return std::nullopt;
    }

    dto.login = json["login"].toString();
    dto.password = json["password"].toString();

    return dto;
}

QJsonObject toJson(const DTO::Auth::LoginRequest& dto)
{
    QJsonObject obj;

    if (dto.login.isEmpty())
    {
        qDebug() << "serializing empty login";
    }
    if (dto.password.isEmpty())
    {
        qDebug() << "serializing empty password";
    }

    obj["login"] = dto.login;
    obj["password"] = dto.password;

    return obj;
}


// ==========================================
// [DTO::Auth::ConfirmRegisterRequest]
// ==========================================
std::optional<DTO::Auth::ConfirmRegisterRequest> fromJsonConfirmRegisterRequest(const QJsonObject& json)
{
    DTO::Auth::ConfirmRegisterRequest dto;

    if (!json.contains("verificationId") || !json["verificationId"].isString())
    {
        qWarning() << "invalid or missing 'verificationId' field in JSON";
        return std::nullopt;
    }

    if (!json.contains("accessCode") || !json["accessCode"].isDouble())
    {
        qWarning() << "invalid or missing 'accessCode' field in JSON";
        return std::nullopt;
    }
    if (!json.contains("username") || !json["username"].isString())
    {
        qWarning() << "invalid or missing 'username' field in JSON";
        return std::nullopt;
    }
    if (!json.contains("password") || !json["password"].isString())
    {
        qWarning() << "invalid or missing 'password' field in JSON";
        return std::nullopt;
    }

    dto.verificationId = json["verificationId"].toString();
    dto.accessCode = json["accessCode"].toInt();
    dto.username = json["username"].toString();
    dto.password = json["password"].toString();

    return dto;
}

QJsonObject toJson(const DTO::Auth::ConfirmRegisterRequest& dto)
{
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

    obj["verificationId"] = dto.verificationId;
    obj["accessCode"] = dto.accessCode;
    obj["username"] = dto.username;
    obj["password"] = dto.password;

    return obj;
}


// ==========================================
// [DTO::Auth::GeneralResponse]
// ==========================================
std::optional<DTO::Auth::GeneralResponse> fromJsonGeneralResponse(const QJsonObject& json)
{
    DTO::Auth::GeneralResponse dto;

    if (!json.contains("accessToken") || !json["accessToken"].isString())
    {
        qWarning() << "invalid or missing 'accessToken' field in JSON";
        return std::nullopt;
    }
    if (!json.contains("expiresAt") || !json["expiresAt"].isString())
    {
        qWarning() << "invalid or missing 'expiresAt' field in JSON";
        return std::nullopt;
    }

    dto.accessToken = json["accessToken"].toString();
    dto.expiresAt = QDateTime::fromString(json["expiresAt"].toString(), Qt::ISODate);

    if (!dto.expiresAt.isValid())
    {
        qWarning() << "invalid 'expiresAt' date format (expected ISO 8601)";
        return std::nullopt;
    }

    return dto;
}

QJsonObject toJson(const DTO::Auth::GeneralResponse& dto)
{
    QJsonObject obj;

    if (dto.accessToken.isEmpty())
    {
        qDebug() << "serializing empty accessToken";
    }
    if (!dto.expiresAt.isValid())
    {
        qDebug() << "serializing invalid expiresAt date";
    }

    obj["accessToken"] = dto.accessToken;
    obj["expiresAt"] = dto.expiresAt.toString(Qt::ISODate);

    return obj;
}

}
