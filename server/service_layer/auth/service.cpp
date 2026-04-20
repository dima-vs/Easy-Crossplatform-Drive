#include <QRandomGenerator>
#include <QUuid>
#include <QCryptographicHash>
#include <sodium.h>
#include <string>
#include <utility>
#include <QStringList>

#include "service.h"
#include "user_record.h"
#include "token.h"

namespace Service::Auth
{

AuthService::AuthService(
    UserRepository &userRep,
    TokenRepository &tokenRep,
    Service::Email::IEmailSender &emailSender,
    Time::ITimeProvider &timeProvider,
    Security::IPasswordHasher &pswHasher,
    const AuthConfig &authConfig
    ) :
    m_userRep(userRep), m_tokenRep(tokenRep),
    m_emailSender(emailSender),
    m_timeProvider(timeProvider),
    m_pswHasher(pswHasher),
    m_authConfig(authConfig)
{

}

void AuthService::clearExpiredRegistrationSessions()
{
    m_activeRegistrationSessions.removeIf(
        [this](std::pair<const QString&, Model::RegistrationSession&> pair) {
            return pair.second.expiresAt < this->m_timeProvider.currentDateTimeUtc();
        }
    );
    qDebug() << "expired registration sessions cleared";
}

void AuthService::clearExpiredTokens() const
{
    m_tokenRep.cleanExpiredTokens(m_timeProvider.currentDateTimeUtc());
    qDebug() << "expired tokens cleared";
}

ServiceResult<Model::RegistrationSessionResult, ServiceError>
    AuthService::startRegistrationSession(const QString& email)
{
    if (m_userRep.existsByEmail(email))
        return ServiceResult<Model::RegistrationSessionResult, ServiceError>
            ::fail(ServiceError::UserAlreadyExists);

    // generate 6 digits access code
    int accessCode = QRandomGenerator::system()->bounded(100000, 1000000);

    // TEST
    qDebug() << "generated access code =" << accessCode << "for email" << email;
    m_emailSender.sendAccessCode(email, accessCode);

    // init registration session
    Model::RegistrationSession regSession;
    regSession.accessCode = accessCode;
    regSession.attemptsCount = 0;
    regSession.email = email;

    QDateTime now = m_timeProvider.currentDateTimeUtc();
    regSession.expiresAt = now.addSecs(m_authConfig.security.regSessionsDurationSec);

    QString verificationId;
    bool idGenerated = false;

    for (int i = 0; i < m_authConfig.uuid.generationAttemptsLimit; ++i)
    {
        QUuid uuid = QUuid::createUuid();
        verificationId = uuid.toString(QUuid::StringFormat::WithoutBraces);
        if (!m_activeRegistrationSessions.contains(verificationId))
        {
            idGenerated = true;
            break;
        }
    }

    if (!idGenerated)
        return ServiceResult<Model::RegistrationSessionResult, ServiceError>::fail(
            ServiceError::UuidAlreadyExists);

    // add session to active sessions map
    m_activeRegistrationSessions[verificationId] = regSession;

    // init registration result
    Model::RegistrationSessionResult regResult;
    regResult.expiresAt = regSession.expiresAt;
    regResult.verificationId = verificationId;

    return ServiceResult<Model::RegistrationSessionResult, ServiceError>::ok(regResult);
}

AuthResult AuthService::completeRegistration(
    const QString& verificationId,
    int accessCode,
    const QString& userName,
    const QString& password)
{
    if (!m_activeRegistrationSessions.contains(verificationId))
        return AuthResult::fail(ServiceError::SessionDoesNotExists);

    if (m_userRep.exists(userName))
        return AuthResult::fail(ServiceError::UserAlreadyExists);

    Model::RegistrationSession& regSession =
        m_activeRegistrationSessions[verificationId];
    // check if registration session expired
    if (regSession.expiresAt < m_timeProvider.currentDateTimeUtc())
    {
        m_activeRegistrationSessions.remove(verificationId);
        return AuthResult::fail(ServiceError::SessionExpired);
    }

    if (regSession.accessCode != accessCode)
    {
        regSession.attemptsCount += 1;

        if (regSession.attemptsCount >= m_authConfig.security.codeEntryAttemptsLimit)
        {
            m_activeRegistrationSessions.remove(verificationId);
            return AuthResult::fail(ServiceError::TooManyAttempts);
        }

        return AuthResult::fail(ServiceError::InvalidCredentials);
    }

    QString passwordHash = m_pswHasher.hashPassword(password);
    if (passwordHash.isEmpty())
        return AuthResult::fail(ServiceError::PasswordHashingFailed);

    UserRecord user(userName, regSession.email, passwordHash);
    if (!m_userRep.addNewUser(user))
        return AuthResult::fail(ServiceError::CannotAddNewUser);

    if (!user.isIDSet())
    {
        qCritical() << "database didn't set user id";
        return AuthResult::fail(ServiceError::IdIsNotSet);
    }

    AuthResult authResult = createUserSession(userName, user.id());
    if (authResult.isOk())
        m_activeRegistrationSessions.remove(verificationId);

    return authResult;
}

AuthResult AuthService::login(const QString& userName, const QString& password)
{
    UserRecord user = m_userRep.getUser(userName);

    if (!user.isValid())
        return AuthResult::fail(ServiceError::UserNotFound);

    if(!m_pswHasher.verifyPassword(password, user.passwordHash()))
        return AuthResult::fail(ServiceError::InvalidCredentials);

    return createUserSession(userName, user.id());
}

AuthResult AuthService::authenticateByToken(const QString& tokenString)
{
    QPair<QString, QString> tokenIdAndSecret = parseToken(tokenString);

    QString tokenId = tokenIdAndSecret.first;
    QString tokenSecret = tokenIdAndSecret.second;
    if (tokenId.isEmpty() || tokenSecret.isEmpty())
        return AuthResult::fail(ServiceError::InvalidCredentials);

    Token token = m_tokenRep.getToken(tokenId);
    if (!token.isValid())
        return AuthResult::fail(ServiceError::TokenNotFound);

    if (token.expiresAt() < m_timeProvider.currentDateTimeUtc())
        return AuthResult::fail(ServiceError::SessionExpired);

    if (hashTokenSecret(tokenSecret) != token.tokenHash())
        return AuthResult::fail(ServiceError::InvalidCredentials);

    UserRecord user = m_userRep.getUser(token.userId());
    if (!user.isValid())
        return AuthResult::fail(ServiceError::UserNotFound);

    Model::Result result;
    result.userName = user.username();
    result.accessToken = tokenString;
    result.expiresAt = token.expiresAt();
    result.userId = user.id();

    return AuthResult::ok(result);
}

AuthResult AuthService::createUserSession(const QString& userName, int userId) const
{
    bool tokenGenerated = false;
    QString tokenId;
    QString tokenSecret;

    for (int i = 0; i < m_authConfig.token.idGenerationAttemptsLimit; ++i)
    {
        QPair<QString, QString> tokenIdAndSecret =
            generateTokenIdAndSecret(m_authConfig.token.tokenIdEntropyBytes, m_authConfig.token.tokenSecretEntropyBytes);
        tokenId = tokenIdAndSecret.first;
        tokenSecret = tokenIdAndSecret.second;

        if (!m_tokenRep.exists(tokenId))
        {
            tokenGenerated = true;
            break;
        }
    }

    if (!tokenGenerated)
        return AuthResult::fail(ServiceError::TokenAlreadyExists);

    QString tokenHash = hashTokenSecret(tokenSecret);

    QDateTime now = m_timeProvider.currentDateTimeUtc();
    QDateTime expiresAt = now.addSecs(m_authConfig.security.userSessionsDurationSec);

    Token token(tokenId, tokenHash, userId, expiresAt);
    if (!m_tokenRep.addNewToken(token))
        return AuthResult::fail(ServiceError::CannotAddNewToken);

    Model::Result result;
    result.userName = userName;
    result.accessToken = composeToken(tokenId, tokenSecret);
    result.expiresAt = expiresAt;
    result.userId = userId;

    return AuthResult::ok(result);
}

QString AuthService::hashTokenSecret(const QString &secret) const
{
    QString tokenHash = QString::fromUtf8(
        QCryptographicHash::hash(
            secret.toUtf8(), QCryptographicHash::Algorithm::Sha256
            ).toHex()
        );

    return tokenHash;
}

QString AuthService::composeToken(const QString& id, const QString& secret) const
{
    // token format: [ID].[SECRET]
    return QString("%1.%2").arg(id, secret);
}

QPair<QString, QString> AuthService::parseToken(const QString& tokenString) const
{
    QStringList parts = tokenString.split('.');

    if (parts.size() != 2)
    {
        return QPair<QString, QString>(QString(), QString());
    }

    // first = ID, second = SECRET
    return QPair<QString, QString>(parts[0], parts[1]);
}

QString AuthService::generateUuid() const
{
    QUuid id = QUuid::createUuid();
    return id.toString(QUuid::StringFormat::WithoutBraces);
}

QPair<QString, QString>
    AuthService::generateTokenIdAndSecret(int idSize, int tokenSize) const
{
    QPair<QString, QString> tokenIdAndSecret;
    // actual size of bytes will be increased due to Base64 conversion
    tokenIdAndSecret.first = QString::fromUtf8(
        generateRandomBytes(idSize).toBase64(
            QByteArray::Base64UrlEncoding | QByteArray::OmitTrailingEquals
            )
    );

    tokenIdAndSecret.second = QString::fromUtf8(
        generateRandomBytes(tokenSize).toBase64(
            QByteArray::Base64UrlEncoding | QByteArray::OmitTrailingEquals
            )
    );

    return tokenIdAndSecret;
}

QByteArray AuthService::generateRandomBytes(int size) const
{
    QByteArray randomBytes;
    randomBytes.resize(size);

    randombytes_buf(static_cast<void*>(randomBytes.data()), size);
    return randomBytes;
}


}
