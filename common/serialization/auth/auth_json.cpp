#include <QDebug>
#include <QDateTime>
#include <QString>
#include "serialization/auth/auth_json.h"

namespace Serialization::Auth
{

// ==========================================
// [RegisterInitRequestDTO]
// ==========================================
std::optional<RegisterInitRequestDTO> fromJsonRegisterInitRequest(const QJsonObject& json)
{
    RegisterInitRequestDTO dto;

    if (json.contains("email") && json["email"].isString())
    {
        dto.email = json["email"].toString();
        return dto;
    } else {
        qWarning() << "[Serialization] invalid or missing 'email' field in JSON";
        return std::nullopt;
    }
}

QJsonObject toJson(const RegisterInitRequestDTO& dto)
{
    QJsonObject obj;

    if (dto.email.isEmpty())
    {
        qDebug() << "[Serialization] serializing empty email";
    }

    obj["email"] = dto.email;
    return obj;
}


// ==========================================
// [RegisterInitResponseDTO]
// ==========================================
std::optional<RegisterInitResponseDTO> fromJsonRegisterInitResponse(const QJsonObject& json)
{
    RegisterInitResponseDTO dto;

    if (!json.contains("verificationId") || !json["verificationId"].isString())
    {
        qWarning() << "[Serialization] invalid or missing 'verificationId' field in JSON";
        return std::nullopt;
    }

    if (!json.contains("expiresAt") || !json["expiresAt"].isString())
    {
        qWarning() << "[Serialization] invalid or missing 'expiresAt' field in JSON";
        return std::nullopt;
    }

    dto.verificationId = json["verificationId"].toString();
    dto.expiresAt = QDateTime::fromString(json["expiresAt"].toString(), Qt::ISODate);

    if (!dto.expiresAt.isValid())
    {
        qWarning() << "[Serialization] invalid 'expiresAt' date format (expected ISO 8601)";
        return std::nullopt;
    }

    return dto;
}

QJsonObject toJson(const RegisterInitResponseDTO& dto)
{
    QJsonObject obj;

    if (dto.verificationId.isEmpty())
    {
        qDebug() << "[Serialization] serializing empty verificationId";
    }
    if (!dto.expiresAt.isValid())
    {
        qDebug() << "[Serialization] serializing invalid expiresAt date";
    }

    obj["verificationId"] = dto.verificationId;
    obj["expiresAt"] = dto.expiresAt.toString(Qt::ISODate);

    return obj;
}


// ==========================================
// [LoginRequestDTO]
// ==========================================
std::optional<LoginRequestDTO> fromJsonLoginRequest(const QJsonObject& json)
{
    LoginRequestDTO dto;

    if (!json.contains("login") || !json["login"].isString())
    {
        qWarning() << "[Serialization] invalid or missing 'login' field in JSON";
        return std::nullopt;
    }
    if (!json.contains("password") || !json["password"].isString())
    {
        qWarning() << "[Serialization] invalid or missing 'password' field in JSON";
        return std::nullopt;
    }

    dto.login = json["login"].toString();
    dto.password = json["password"].toString();

    return dto;
}

QJsonObject toJson(const LoginRequestDTO& dto)
{
    QJsonObject obj;

    if (dto.login.isEmpty())
    {
        qDebug() << "[Serialization] serializing empty login";
    }
    if (dto.password.isEmpty())
    {
        qDebug() << "[Serialization] serializing empty password";
    }

    obj["login"] = dto.login;
    obj["password"] = dto.password;

    return obj;
}


// ==========================================
// [ConfirmRegisterRequestDTO]
// ==========================================
std::optional<ConfirmRegisterRequestDTO> fromJsonConfirmRegisterRequest(const QJsonObject& json)
{
    ConfirmRegisterRequestDTO dto;

    if (!json.contains("verificationId") || !json["verificationId"].isString())
    {
        qWarning() << "[Serialization] invalid or missing 'verificationId' field in JSON";
        return std::nullopt;
    }

    if (!json.contains("accessCode") || !json["accessCode"].isDouble())
    {
        qWarning() << "[Serialization] invalid or missing 'accessCode' field in JSON";
        return std::nullopt;
    }
    if (!json.contains("username") || !json["username"].isString())
    {
        qWarning() << "[Serialization] invalid or missing 'username' field in JSON";
        return std::nullopt;
    }
    if (!json.contains("password") || !json["password"].isString())
    {
        qWarning() << "[Serialization] invalid or missing 'password' field in JSON";
        return std::nullopt;
    }

    dto.verificationId = json["verificationId"].toString();
    dto.accessCode = json["accessCode"].toInt();
    dto.username = json["username"].toString();
    dto.password = json["password"].toString();

    return dto;
}

QJsonObject toJson(const ConfirmRegisterRequestDTO& dto)
{
    QJsonObject obj;

    if (dto.verificationId.isEmpty())
    {
        qDebug() << "[Serialization] serializing empty verificationId";
    }
    if (dto.username.isEmpty())
    {
        qDebug() << "[Serialization] serializing empty username";
    }
    if (dto.password.isEmpty())
    {
        qDebug() << "[Serialization] serializing empty password";
    }

    obj["verificationId"] = dto.verificationId;
    obj["accessCode"] = dto.accessCode;
    obj["username"] = dto.username;
    obj["password"] = dto.password;

    return obj;
}


// ==========================================
// [AuthResponseDTO]
// ==========================================
std::optional<AuthResponseDTO> fromJsonAuthResponse(const QJsonObject& json)
{
    AuthResponseDTO dto;

    if (!json.contains("accessToken") || !json["accessToken"].isString())
    {
        qWarning() << "[Serialization] invalid or missing 'accessToken' field in JSON";
        return std::nullopt;
    }
    if (!json.contains("expiresAt") || !json["expiresAt"].isString())
    {
        qWarning() << "[Serialization] invalid or missing 'expiresAt' field in JSON";
        return std::nullopt;
    }

    dto.accessToken = json["accessToken"].toString();
    dto.expiresAt = QDateTime::fromString(json["expiresAt"].toString(), Qt::ISODate);

    if (!dto.expiresAt.isValid())
    {
        qWarning() << "[Serialization] invalid 'expiresAt' date format (expected ISO 8601)";
        return std::nullopt;
    }

    return dto;
}

QJsonObject toJson(const AuthResponseDTO& dto)
{
    QJsonObject obj;

    if (dto.accessToken.isEmpty())
    {
        qDebug() << "[Serialization] serializing empty accessToken";
    }
    if (!dto.expiresAt.isValid())
    {
        qDebug() << "[Serialization] serializing invalid expiresAt date";
    }

    obj["accessToken"] = dto.accessToken;
    obj["expiresAt"] = dto.expiresAt.toString(Qt::ISODate);

    return obj;
}

}
