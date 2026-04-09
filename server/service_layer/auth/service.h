#ifndef SERVICE_H
#define SERVICE_H

#include <QDateTime>
#include <QString>
#include <QObject>
#include <QByteArray>
#include <QMap>
#include <QPair>
#include <QTimer>
#include "user_repository.h"
#include "token_repository.h"
#include "service_result.h"
#include "auth/models.h"
#include "auth/auth_config.h"
#include "auth/error_codes.h"
#include "email/email_sender.h"
#include "datetime/time_provider_interface.h"
#include "security/password_hasher_interface.h"

namespace Service::Auth
{

namespace Model = ::ServiceModel::Auth;
using AuthResult = ServiceResult<Model::Result, ErrorCode::Auth::ServiceError>;
using ServiceError = ::ErrorCode::Auth::ServiceError;
using AuthConfig = Config::Auth::AuthConfig;

class AuthService
{
private:
    UserRepository& m_userRep;
    TokenRepository& m_tokenRep;
    Service::Email::IEmailSender& m_emailSender;
    Service::Time::ITimeProvider& m_timeProvider;
    Service::Security::IPasswordHasher& m_pswHasher;

    AuthConfig m_authConfig;

    // {uuid: RegistrationSession}
    QMap<QString, Model::RegistrationSession> m_activeRegistrationSessions;
public:
    AuthService(
        UserRepository& userRep,
        TokenRepository& tokenRep,
        Service::Email::IEmailSender& emailSender,
        Service::Time::ITimeProvider& timeProvider,
        Service::Security::IPasswordHasher& pswHasher,
        const AuthConfig& authConfig
        );

    ServiceResult<Model::RegistrationSessionResult, ServiceError> startRegistrationSession(const QString& email);
    AuthResult completeRegistration(const QString& verificationId,
                         int accessCode,
                         const QString& userName,
                         const QString& password);
    AuthResult login(const QString& userName, const QString& password);
    AuthResult authenticateByToken(const QString& tokenString);

    void clearExpiredRegistrationSessions();
    void clearExpiredTokens() const;
private:
    QString generateUuid() const;
    QPair<QString, QString> generateTokenIdAndSecret(int idSize, int tokenSize) const;
    QByteArray generateRandomBytes(int size) const;
    AuthResult createUserSession(const QString &userName, int userId) const;
    QString composeToken(const QString& id, const QString& secret) const;
    QPair<QString, QString> parseToken(const QString& tokenString) const;
    QString hashTokenSecret(const QString &secret) const;
};

}

#endif // SERVICE_H
