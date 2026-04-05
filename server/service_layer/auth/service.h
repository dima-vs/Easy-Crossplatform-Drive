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
#include "auth/error_codes.h"

namespace Service::Auth
{

namespace Model = ::ServiceModel::Auth;
using AuthResult = ServiceResult<Model::Result, ErrorCode::Auth::ServiceError>;
using ServiceError = ::ErrorCode::Auth::ServiceError;

class AuthService : public QObject
{
    Q_OBJECT

private:
    UserRepository& m_userRep;
    TokenRepository& m_tokenRep;
    // registration session duration in seconds
    int m_regSessionsDurationSec;
    int m_userSessionsDurationSec;
    int m_codeEntryAttemptsLimit;

    QTimer m_registrationSessionsCleanupTimer;
    QTimer m_tokensCleanupTimer;

    // {uuid: RegistrationSession}
    QMap<QString, Model::RegistrationSession> m_activeRegistrationSessions;
public:
    AuthService(UserRepository& userRep, TokenRepository& tokenRep);

    ServiceResult<Model::RegistrationSessionResult, ServiceError> startRegistrationSession(const QString& email);
    AuthResult completeRegistration(const QString& verificationId,
                         int accessCode,
                         const QString& userName,
                         const QString& password);
    // AuthResult login(const QString& userName, const QString& password);
    // AuthResult authenticateByToken(const QString& tokenString);
private:
    QString hashPassword(const QString& password) const;
    bool verifyPassword(const QString& password, const QString& passwordHash) const;
    QString generateUuid() const;
    QPair<QString, QString> generateIdAndAccessToken(int idSize, int tokenSize) const;
    QByteArray generateRandomBytes(int size) const;
    AuthResult createUserSession(const QString &userName, int userId) const;
private slots:
    void clearExpiredRegistrationSessions();
    void clearExpiredTokens() const;
};

}

#endif // SERVICE_H
