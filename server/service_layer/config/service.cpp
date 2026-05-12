#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDebug>

#include "config/json_keys.h"
#include "config/service.h"

namespace Service::Config
{

ConfigService::ConfigService(const QString& configFilePath)
    : m_filePath(configFilePath)
{
}

bool ConfigService::saveDefaultsIfNotExist()
{
    QFile file(m_filePath);
    if (file.exists())
    {
        return true;
    }

    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        qCritical() << "failed to create default config file:" <<
            m_filePath;
        return false;
    }

    using Keys = JsonKeys::Root;
    QJsonObject root;

    root[Keys::App] = serializeApp();
    root[Keys::Auth] = serializeAuth();
    root[Keys::File] = serializeFile();
    root[Keys::Email] = serializeEmail();
    root[Keys::SystemSecurity] = serializeSecurity();

    QJsonDocument doc(root);
    file.write(doc.toJson(QJsonDocument::Indented));
    file.close();

    qInfo() << "default config file generated at:" <<
        m_filePath;
    return true;
}

bool ConfigService::load()
{
    QFile file(m_filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qCritical() << "failed to open config file:" << m_filePath;
        return false;
    }

    QByteArray data = file.readAll();
    file.close();

    QJsonParseError parseError;
    QJsonDocument doc = QJsonDocument::fromJson(data, &parseError);

    if (parseError.error != QJsonParseError::NoError)
    {
        qCritical() << "JSON parse error in config file:" <<
            parseError.errorString();
        return false;
    }

    if (!doc.isObject())
    {
        qCritical() << "config root must be a JSON object.";
        return false;
    }

    QJsonObject root = doc.object();
    using Keys = JsonKeys::Root;

    QJsonObject appObj;
    if (requireObject(root, Keys::App, appObj))
    {
        parseApp(appObj);
    }

    QJsonObject authObj;
    if (requireObject(root, Keys::Auth, authObj))
    {
        parseAuth(authObj);
    }

    QJsonObject fileObj;
    if (requireObject(root, Keys::File, fileObj))
    {
        parseFile(fileObj);
    }

    QJsonObject emailObj;
    if (requireObject(root, Keys::Email, emailObj))
    {
        parseEmail(emailObj);
    }

    QJsonObject securityObj;
    if (requireObject(root, Keys::SystemSecurity, securityObj))
    {
        parseSecurity(securityObj);
    }

    qInfo() << "configuration loaded successfully.";
    return true;
}


void ConfigService::parseApp(const QJsonObject& json)
{
    using Keys = JsonKeys::App;

    requireString(json, Keys::Host, m_app.host);
    requireInt(json, Keys::Port, m_app.port);
    requireString(json, Keys::Protocol, m_app.protocol);
}


void ConfigService::parseEmail(const QJsonObject& json)
{
    using Keys = JsonKeys::Email;

    QJsonObject smtpObj;
    if (requireObject(json, Keys::Smtp, smtpObj))
    {
        requireString(smtpObj, Keys::SmtpKeys::Host, m_email.smtp.host);
        requireInt(smtpObj, Keys::SmtpKeys::Port, m_email.smtp.port);
        requireString(
            smtpObj, Keys::SmtpKeys::ConnectionType,
            m_email.smtp.connectionType);
    }

    QJsonObject credsObj;
    if (requireObject(json, Keys::Credentials, credsObj))
    {
        requireString(
            credsObj, Keys::CredentialsKeys::Email,
            m_email.credentials.email);
        requireString(
            credsObj, Keys::CredentialsKeys::AppPassword,
            m_email.credentials.appPassword);
    }

    QJsonObject senderObj;
    if (requireObject(json, Keys::Sender, senderObj))
    {
        requireString(
            senderObj, Keys::SenderKeys::DisplayName,
            m_email.sender.displayName);
    }
}

void ConfigService::parseAuth(const QJsonObject& json)
{
    using Keys = JsonKeys::Auth;

    QJsonObject tokenObj;
    if (requireObject(json, Keys::Token, tokenObj))
    {
        requireInt(
            tokenObj,
            Keys::TokenKeys::TokenIdEntropyBytes,
            m_auth.token.tokenIdEntropyBytes);
        requireInt(
            tokenObj,
            Keys::TokenKeys::TokenSecretEntropyBytes,
            m_auth.token.tokenSecretEntropyBytes);
        requireInt(
            tokenObj,
            Keys::TokenKeys::IdGenerationAttemptsLimit,
            m_auth.token.idGenerationAttemptsLimit
            );
    }

    QJsonObject uuidObj;
    if (requireObject(json, Keys::Uuid, uuidObj))
    {
        requireInt(
            uuidObj,
            Keys::UuidKeys::GenerationAttemptsLimit,
            m_auth.uuid.generationAttemptsLimit
            );
    }

    QJsonObject secObj;
    if (requireObject(json, Keys::Security, secObj))
    {
        requireInt(
            secObj,
            Keys::SecurityKeys::RegSessionsDurationSec,
            m_auth.security.regSessionsDurationSec
            );
        requireInt(
            secObj,
            Keys::SecurityKeys::UserSessionsDurationSec,
            m_auth.security.userSessionsDurationSec
            );
        requireInt(
            secObj,
            Keys::SecurityKeys::CodeEntryAttemptsLimit,
            m_auth.security.codeEntryAttemptsLimit
            );
        requireInt(
            secObj,
            Keys::SecurityKeys::MaxActiveSessionsPerUser,
            m_auth.security.maxActiveSessionsPerUser
            );
    }
}

void ConfigService::parseFile(const QJsonObject& json)
{
    using Keys = JsonKeys::File;

    QJsonObject uploadObj;
    if (requireObject(json, Keys::Upload, uploadObj))
    {
        requireInt64(
            uploadObj,
            Keys::TransferLimits::MaxChunkSize,
            m_file.upload.maxChunkSize
            );
        requireInt64(
            uploadObj,
            Keys::TransferLimits::MaxFileSize,
            m_file.upload.maxFileSize
            );
    }

    QJsonObject downloadObj;
    if (requireObject(json, Keys::Download, downloadObj))
    {
        requireInt64(
            downloadObj,
            Keys::TransferLimits::MaxChunkSize,
            m_file.download.maxChunkSize
            );
        requireInt64(
            downloadObj,
            Keys::TransferLimits::MaxFileSize,
            m_file.download.maxFileSize
            );
    }

    QJsonObject storageObj;
    if (requireObject(json, Keys::Storage, storageObj))
    {
        requireInt64(
            storageObj,
            Keys::StorageKeys::MaxTotalStoragePerUser,
            m_file.storage.maxTotalStoragePerUser
            );
        requireString(
            storageObj,
            Keys::StorageKeys::BaseStoragePath,
            m_file.storage.baseStoragePath
            );
    }

    QJsonObject sessionObj;
    if (requireObject(json, Keys::Session, sessionObj))
    {
        requireInt(
            sessionObj,
            Keys::SessionKeys::UploadSessionsDurationSec,
            m_file.session.uploadSessionsDurationSec
            );
    }

    QJsonObject uuidObj;
    if (requireObject(json, Keys::Uuid, uuidObj))
    {
        requireInt(
            uuidObj,
            Keys::UuidKeys::GenerationAttemptsLimit,
            m_file.uuid.generationAttemptsLimit
            );
    }
}

void ConfigService::parseSecurity(const QJsonObject& json)
{
    using Keys = Config::JsonKeys::SystemSecurity;

    QJsonObject pwHashObj;
    if (requireObject(json, Keys::PasswordHashing, pwHashObj))
    {
        QJsonObject argonObj;
        if (requireObject(pwHashObj, Keys::Argon2, argonObj))
        {
            requireInt64(
                argonObj,
                Keys::Argon2Keys::OpsLimit,
                m_security.passwordHashing.argon2.opsLimit
                );
            requireInt64(
                argonObj,
                Keys::Argon2Keys::MemLimit,
                m_security.passwordHashing.argon2.memLimit
                );
        }
    }
}

bool ConfigService::requireString(
    const QJsonObject& json,
    const char* key,
    QString& outValue
    )
{
    if (!json.contains(key) || !json[key].isString())
        return false;
    outValue = json[key].toString();
    return true;
}

bool ConfigService::requireInt(
    const QJsonObject& json,
    const char* key,
    int& outValue
    )
{
    if (!json.contains(key) || !json[key].isDouble())
        return false;
    outValue = json[key].toInt();
    return true;
}

bool ConfigService::requireInt64(
    const QJsonObject& json,
    const char* key,
    qint64& outValue
    )
{
    if (!json.contains(key) || !json[key].isDouble())
        return false;
    outValue = json[key].toInteger();
    return true;
}

bool ConfigService::requireObject(
    const QJsonObject& json,
    const char* key,
    QJsonObject& outValue
    )
{
    if (!json.contains(key) || !json[key].isObject())
        return false;
    outValue = json[key].toObject();
    return true;
}


QJsonObject ConfigService::serializeApp() const
{
    using Keys = JsonKeys::App;
    QJsonObject obj;

    obj[Keys::Host] = m_app.host;
    obj[Keys::Port] = m_app.port;
    obj[Keys::Protocol] = m_app.protocol;

    return obj;
}

QJsonObject ConfigService::serializeEmail() const
{
    using Keys = JsonKeys::Email;
    QJsonObject root;

    QJsonObject smtpObj;
    smtpObj[Keys::SmtpKeys::Host] = m_email.smtp.host;
    smtpObj[Keys::SmtpKeys::Port] = m_email.smtp.port;
    smtpObj[Keys::SmtpKeys::ConnectionType] =
        m_email.smtp.connectionType;

    QJsonObject credsObj;
    credsObj[Keys::CredentialsKeys::Email] =
        m_email.credentials.email;
    credsObj[Keys::CredentialsKeys::AppPassword] =
        m_email.credentials.appPassword;

    QJsonObject senderObj;
    senderObj[Keys::SenderKeys::DisplayName] =
        m_email.sender.displayName;

    root[Keys::Smtp] = smtpObj;
    root[Keys::Credentials] = credsObj;
    root[Keys::Sender] = senderObj;

    return root;
}

QJsonObject ConfigService::serializeAuth() const
{
    using Keys = JsonKeys::Auth;
    QJsonObject root;

    QJsonObject token;
    token[Keys::TokenKeys::TokenIdEntropyBytes] =
        m_auth.token.tokenIdEntropyBytes;
    token[Keys::TokenKeys::TokenSecretEntropyBytes] =
        m_auth.token.tokenSecretEntropyBytes;
    token[Keys::TokenKeys::IdGenerationAttemptsLimit] =
        m_auth.token.idGenerationAttemptsLimit;

    QJsonObject uuid;
    uuid[Keys::UuidKeys::GenerationAttemptsLimit] =
        m_auth.uuid.generationAttemptsLimit;

    QJsonObject security;
    security[Keys::SecurityKeys::RegSessionsDurationSec] =
        m_auth.security.regSessionsDurationSec;
    security[Keys::SecurityKeys::UserSessionsDurationSec] =
        m_auth.security.userSessionsDurationSec;
    security[Keys::SecurityKeys::CodeEntryAttemptsLimit] =
        m_auth.security.codeEntryAttemptsLimit;
    security[Keys::SecurityKeys::MaxActiveSessionsPerUser] =
        m_auth.security.maxActiveSessionsPerUser;

    root[Keys::Token] = token;
    root[Keys::Uuid] = uuid;
    root[Keys::Security] = security;

    return root;
}

QJsonObject ConfigService::serializeFile() const
{
    using Keys = JsonKeys::File;
    QJsonObject root;

    QJsonObject upload;
    upload[Keys::TransferLimits::MaxChunkSize] =
        m_file.upload.maxChunkSize;
    upload[Keys::TransferLimits::MaxFileSize] =
        m_file.upload.maxFileSize;

    QJsonObject download;
    download[Keys::TransferLimits::MaxChunkSize] =
        m_file.download.maxChunkSize;
    download[Keys::TransferLimits::MaxFileSize] =
        m_file.download.maxFileSize;

    QJsonObject storage;
    storage[Keys::StorageKeys::MaxTotalStoragePerUser] =
        m_file.storage.maxTotalStoragePerUser;
    storage[Keys::StorageKeys::BaseStoragePath] =
        m_file.storage.baseStoragePath;

    QJsonObject session;
    session[Keys::SessionKeys::UploadSessionsDurationSec] =
        m_file.session.uploadSessionsDurationSec;

    QJsonObject uuid;
    uuid[Keys::UuidKeys::GenerationAttemptsLimit] =
        m_file.uuid.generationAttemptsLimit;

    root[Keys::Upload] = upload;
    root[Keys::Download] = download;
    root[Keys::Storage] = storage;
    root[Keys::Session] = session;
    root[Keys::Uuid] = uuid;

    return root;
}

QJsonObject ConfigService::serializeSecurity() const
{
    using Keys = Config::JsonKeys::SystemSecurity;
    QJsonObject root;

    QJsonObject argonObj;
    argonObj[Keys::Argon2Keys::OpsLimit] =
        m_security.passwordHashing.argon2.opsLimit;
    argonObj[Keys::Argon2Keys::MemLimit] =
        m_security.passwordHashing.argon2.memLimit;

    QJsonObject pwHashObj;
    pwHashObj[Keys::Argon2] = argonObj;

    root[Keys::PasswordHashing] = pwHashObj;

    return root;
}


}
