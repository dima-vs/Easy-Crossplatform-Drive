#include <QRandomGenerator>
#include <QUuid>
#include <QCryptographicHash>
#include <sodium.h>
#include <string>
#include <utility>

#include "service.h"
#include "user.h"
#include "token.h"

namespace Service::Auth
{

AuthService::AuthService(UserRepository &userRep, TokenRepository &tokenRep):
    m_userRep(userRep), m_tokenRep(tokenRep),
    m_regSessionsDurationSec(180), // 3 min
    m_userSessionsDurationSec(604800), // 1 week
    m_codeEntryAttemptsLimit(3)
{
    m_registrationSessionsCleanupTimer.setInterval(600000); // 10 min
    m_tokensCleanupTimer.setInterval(86400000 * 1); // 1 day

    connect(
        &m_registrationSessionsCleanupTimer,
        &QTimer::timeout,
        this,
        &AuthService::clearExpiredRegistrationSessions
        );

    connect(
        &m_tokensCleanupTimer,
        &QTimer::timeout,
        this,
        &AuthService::clearExpiredTokens
        );

    m_registrationSessionsCleanupTimer.start();
    m_tokensCleanupTimer.start();
}

void AuthService::clearExpiredRegistrationSessions()
{
    m_activeRegistrationSessions.removeIf(
        [](std::pair<const QString&, Model::RegistrationSession&> pair) {
            return pair.second.expiresAt < QDateTime::currentDateTimeUtc();
        }
    );
}

void AuthService::clearExpiredTokens() const
{
    m_tokenRep.cleanExpiredTokens();
}

ServiceResult<Model::RegistrationSessionResult, ServiceError>
    AuthService::startRegistrationSession(const QString& email)
{
    // generate 6 digits access code
    int accessCode = QRandomGenerator::system()->bounded(100000, 1000000);

    // TEST
    qDebug() << "generated access code =" << accessCode << "for email" << email;

    // init registration session
    Model::RegistrationSession regSession;
    regSession.accessCode = accessCode;
    regSession.attemptsCount = 0;
    regSession.email = email;

    QDateTime now = QDateTime::currentDateTimeUtc();
    regSession.expiresAt = now.addSecs(m_regSessionsDurationSec);

    QUuid uuid = QUuid::createUuid();
    QString verificationId = uuid.toString(QUuid::StringFormat::WithoutBraces);
    // add session to active sessions map
    m_activeRegistrationSessions[verificationId] = regSession;

    // init registration result
    Model::RegistrationSessionResult regResult;
    regResult.expiresAt = regSession.expiresAt;
    regResult.varificationId = verificationId;

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

    Model::RegistrationSession& regSession =
        m_activeRegistrationSessions[verificationId];
    // check if registration session expired
    if (regSession.expiresAt < QDateTime::currentDateTimeUtc())
    {
        m_activeRegistrationSessions.remove(verificationId);
        return AuthResult::fail(ServiceError::SessionExpired);
    }

    if (regSession.accessCode != accessCode)
    {
        regSession.attemptsCount += 1;

        if (regSession.attemptsCount >= m_codeEntryAttemptsLimit)
        {
            m_activeRegistrationSessions.remove(verificationId);
            return AuthResult::fail(ServiceError::TooManyAttempts);
        }

        return AuthResult::fail(ServiceError::InvalidCredentials);
    }

    QString passwordHash = hashPassword(password);
    if (passwordHash.isEmpty())
        return AuthResult::fail(ServiceError::PasswordHashingFailed);

    User user(userName, regSession.email, passwordHash);
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

AuthResult AuthService::createUserSession(const QString& userName, int userId) const
{
    const int idSize = 16;
    const int tokenSize = 32;
    // max count of iterations to generate token in case of
    // it's id already exists
    const int token_generation_limit = 8;

    bool tokenGenerated = false;
    QString id;
    QString accessToken;

    for (int i = 0; i < token_generation_limit; ++i)
    {
        QPair<QString, QString> idAndToken = generateIdAndAccessToken(idSize, tokenSize);
        id = idAndToken.first;
        accessToken = idAndToken.second;

        if (!m_tokenRep.exists(id))
        {
            tokenGenerated = true;
            break;
        }
    }

    if (!tokenGenerated)
        return AuthResult::fail(ServiceError::TokenAlreadyExists);

    QString tokenHash = QString::fromUtf8(
        QCryptographicHash::hash(
            accessToken.toUtf8(), QCryptographicHash::Algorithm::Sha256
        ).toHex()
    );

    QDateTime now = QDateTime::currentDateTimeUtc();
    QDateTime expiresAt = now.addSecs(m_userSessionsDurationSec);

    Token token(id, tokenHash, userId, expiresAt);
    if (!m_tokenRep.addNewToken(token))
        return AuthResult::fail(ServiceError::CannotAddNewToken);

    Model::Result result;
    result.userName = userName;
    result.accessToken = accessToken;
    result.expiresAt = expiresAt;

    return AuthResult::ok(result);
}

QString AuthService::hashPassword(const QString& password) const
{
    char out_hash[crypto_pwhash_STRBYTES] = {0};
    QByteArray pwBytes = password.toUtf8();

    if (crypto_pwhash_str(
            out_hash,
            pwBytes.constData(),
            pwBytes.size(),
            crypto_pwhash_OPSLIMIT_INTERACTIVE,
            crypto_pwhash_MEMLIMIT_INTERACTIVE
            ) != 0)
    {
        qCritical() << "sodium: out of memory or internal error";
        return QString();
    }

    return QString::fromUtf8(out_hash);
}

bool AuthService::verifyPassword(const QString& password, const QString& passwordHash) const
{
    if (passwordHash.isEmpty()) return false;

    QByteArray pwBytes = password.toUtf8();
    // QByteArray garantees zero-terminated string
    QByteArray hashBytes = passwordHash.toLatin1();

    int result = crypto_pwhash_str_verify(
        hashBytes.constData(),
        pwBytes.constData(),
        pwBytes.size()
        );

    return result == 0;
}

QString AuthService::generateUuid() const
{
    QUuid id = QUuid::createUuid();
    return id.toString(QUuid::StringFormat::WithoutBraces);
}

QPair<QString, QString>
    AuthService::generateIdAndAccessToken(int idSize, int tokenSize) const
{
    QPair<QString, QString> idAndToken;
    // actual size of bytes will be increased due to Base64 conversion
    idAndToken.first = QString::fromUtf8(
        generateRandomBytes(idSize).toBase64(
            QByteArray::Base64UrlEncoding | QByteArray::OmitTrailingEquals
            )
    );
    idAndToken.second = QString::fromUtf8(
        generateRandomBytes(tokenSize).toBase64(
            QByteArray::Base64UrlEncoding | QByteArray::OmitTrailingEquals
            )
    );

    return idAndToken;
}

QByteArray AuthService::generateRandomBytes(int size) const
{
    QByteArray randomBytes;
    randomBytes.resize(size);

    randombytes_buf(static_cast<void*>(randomBytes.data()), size);
    return randomBytes;
}


}
