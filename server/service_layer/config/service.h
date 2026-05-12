#ifndef CONFIG_SERVICE_H
#define CONFIG_SERVICE_H

#include <QString>
#include <QJsonObject>
#include "config/app_config.h"
#include "config/auth_config.h"
#include "config/email_config.h"
#include "config/file_config.h"
#include "config/security_config.h"

namespace Service::Config
{

class ConfigService
{
private:
    QString m_filePath;

    ::Config::Auth::AuthConfig m_auth;
    ::Config::File::FileConfig m_file;
    ::Config::App::AppConfig m_app;
    ::Config::Email::EmailConfig m_email;
    ::Config::Security::SecurityConfig m_security;
public:
    ConfigService(const QString& configFilePath);

    bool load();
    bool saveDefaultsIfNotExist();

    const ::Config::Auth::AuthConfig& auth() const { return m_auth; }
    const ::Config::File::FileConfig& file() const { return m_file; }
    const ::Config::Email::EmailConfig& email() const { return m_email; }
    const ::Config::App::AppConfig& app() const { return m_app; }
    const ::Config::Security::SecurityConfig& security() const { return m_security; }

private:
    void parseAuth(const QJsonObject& json);
    void parseFile(const QJsonObject& json);
    void parseEmail(const QJsonObject& json);
    void parseApp(const QJsonObject& json);
    void parseSecurity(const QJsonObject& json);

    bool requireString(
        const QJsonObject& json,
        const char* key,
        QString& outValue
        );

    bool requireInt(
        const QJsonObject& json,
        const char* key,
        int& outValue
        );

    bool requireInt64(
        const QJsonObject& json,
        const char* key,
        qint64& outValue
        );

    bool requireObject(
        const QJsonObject& json,
        const char* key,
        QJsonObject& outValue
        );

    QJsonObject serializeAuth() const;
    QJsonObject serializeFile() const;
    QJsonObject serializeApp() const;
    QJsonObject serializeEmail() const;
    QJsonObject serializeSecurity() const;
};

}

#endif // CONFIG_SERVICE_H
