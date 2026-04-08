#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <QUuid>
#include <QString>
#include <QStringList>
#include <QDateTime>
#include <QPair>

#include "database_manager.h"
#include "user_repository.h"
#include "token_repository.h"
#include "auth/service.h"


class MailServiceSpy : public Service::Email::IEmailSender
{
public:
    int m_lastSentCode = 0;
    QString m_lastEmail;

    MOCK_METHOD(void, sendAccessCode, (const QString& email, int code), (override));
};

class MockTimeProvider : public Service::Time::ITimeProvider
{
public:
    QDateTime m_manualTime;

    MOCK_METHOD(QDateTime, currentDateTimeUtc, (), (const, override));
};

namespace Model = ::ServiceModel::Auth;

class AuthServiceTest : public testing::Test
{
protected:
    using AuthResult = ServiceResult<Model::Result, ErrorCode::Auth::ServiceError>;
    using ServiceError = ::ErrorCode::Auth::ServiceError;

    DatabaseManager m_DBManager;
    UserRepository m_userRep;
    TokenRepository m_tokenRep;

    MailServiceSpy m_mailSpy;
    testing::NiceMock<MockTimeProvider> m_timeProvider;
    Service::Auth::AuthService m_authService;

    // registration data
    QString m_email;
    QString m_username;
    QString m_password;

    AuthServiceTest() :
        m_DBManager(":memory:", QUuid::createUuid().toString()),
        m_userRep(m_DBManager),
        m_tokenRep(m_DBManager),
        m_authService(m_userRep, m_tokenRep, m_mailSpy, m_timeProvider)
    {
        m_timeProvider.m_manualTime = QDateTime::currentDateTimeUtc();
    }

    void SetUp() override
    {
        using ::testing::_;
        using ::testing::DoAll;
        using ::testing::SaveArg;

        ON_CALL(m_mailSpy, sendAccessCode(_, _))
            .WillByDefault(
                DoAll(
                    SaveArg<0>(&m_mailSpy.m_lastEmail),
                    SaveArg<1>(&m_mailSpy.m_lastSentCode)
                    )
                );

        ON_CALL(m_timeProvider, currentDateTimeUtc())
            .WillByDefault([this]() {
                return this->m_timeProvider.m_manualTime;
            });
    }

    void expectEmailSent(int times = 1)
    {
        using ::testing::_;

        EXPECT_CALL(m_mailSpy, sendAccessCode(_, _)).Times(times);
    }

    int accessTokenPartsCount(const QString& accessToken)
    {
        QStringList parts = accessToken.split('.');
        return parts.size();
    }

    QPair<QString, QString> parseToken(const QString& accessToken)
    {
        QStringList parts = accessToken.split('.');

        if (parts.size() != 2)
        {
            return QPair<QString, QString>(QString(), QString());
        }

        // first = ID, second = SECRET
        return QPair<QString, QString>(parts[0], parts[1]);
    }

    AuthResult registerUser(const QString& email,
                        const QString& username,
                        const QString& password)
    {
        auto startRes = m_authService.startRegistrationSession(email);
        EXPECT_TRUE(startRes.isOk());

        QString verificationId = startRes.data().verificationId;

        auto completeRes = m_authService.completeRegistration(
            verificationId, m_mailSpy.m_lastSentCode, username, password
            );

        EXPECT_TRUE(completeRes.isOk());
        return completeRes;
    }

    void updateRegistrationData(int userIndex = 1)
    {
        m_email = QString("newuser%1@gmail.com").arg(userIndex);
        m_username = QString("new_user_%1").arg(userIndex);
        m_password = QString("strong_password%1").arg(userIndex);
    }
};


TEST_F(AuthServiceTest, SuccessfulRegistrationFlow)
{
    expectEmailSent();

    QString email = "newuser@gmail.com";
    QString username = "new_user_1";
    QString password = "strong_password";

    auto startRes = m_authService.startRegistrationSession(email);
    ASSERT_TRUE(startRes.isOk());

    EXPECT_EQ(m_mailSpy.m_lastEmail, email);
    // code must consist of 6 digits
    EXPECT_GT(m_mailSpy.m_lastSentCode, 99999);
    EXPECT_LT(m_mailSpy.m_lastSentCode, 1000000);

    QString verificationId = startRes.data().verificationId;
    EXPECT_FALSE(verificationId.isEmpty());

    auto completeRes = m_authService.completeRegistration(
        verificationId, m_mailSpy.m_lastSentCode, username, password
        );

    ASSERT_TRUE(completeRes.isOk());

    EXPECT_EQ(completeRes.data().userName, username);
    EXPECT_FALSE(completeRes.data().accessToken.isEmpty());
    // access token must consist of 2 parts: [ID] and [TOKEN]
    EXPECT_EQ(accessTokenPartsCount(completeRes.data().accessToken), 2);

    EXPECT_TRUE(m_userRep.exists(username));
}

TEST_F(AuthServiceTest, TooManyAttemptsErrorOccurs)
{
    int attemptsLimit = 3;
    // do 5 more times than attempts limit
    int additionalAttempts = 5;
    expectEmailSent(1);

    auto startRes = m_authService.startRegistrationSession(m_email);
    ASSERT_TRUE(startRes.isOk());

    updateRegistrationData(1);

    for (int i = 1; i <= attemptsLimit + additionalAttempts; ++i)
    {
        auto completeRes = m_authService.completeRegistration(
            startRes.data().verificationId,
            m_mailSpy.m_lastSentCode + 1, // wrong code
            m_username, m_password
            );

        EXPECT_FALSE(completeRes.isOk());
        if (i < attemptsLimit)
            EXPECT_EQ(completeRes.error(), ServiceError::InvalidCredentials);
        else if (i == attemptsLimit)
            EXPECT_EQ(completeRes.error(), ServiceError::TooManyAttempts);
        else
            EXPECT_EQ(completeRes.error(), ServiceError::SessionDoesNotExists);

    }
}

TEST_F(AuthServiceTest, RegistrationFailsIfUserAlreadyExists)
{
    expectEmailSent(2);

    QString email = "newuser@gmail.com";
    QString username = "new_user_1";
    QString password = "strong_password";

    // first user registration
    auto startRes = m_authService.startRegistrationSession(email);
    ASSERT_TRUE(startRes.isOk());

    auto completeRes = m_authService.completeRegistration(
        startRes.data().verificationId, m_mailSpy.m_lastSentCode, username, password
        );
    ASSERT_TRUE(completeRes.isOk());
    EXPECT_TRUE(m_userRep.exists(username));

    // === Email duplicate ===
    auto startResDuplicateEmail = m_authService.startRegistrationSession(email);

    EXPECT_FALSE(startResDuplicateEmail.isOk());
    EXPECT_EQ(startResDuplicateEmail.error(), ErrorCode::Auth::ServiceError::UserAlreadyExists);

    // === Username duplicate ===
    QString email2 = "another_email@gmail.com";

    auto startRes2 = m_authService.startRegistrationSession(email2);
    ASSERT_TRUE(startRes2.isOk());

    auto completeResDuplicateUsername = m_authService.completeRegistration(
        startRes2.data().verificationId, m_mailSpy.m_lastSentCode, username, "another_password"
        );

    EXPECT_FALSE(completeResDuplicateUsername.isOk());
    EXPECT_EQ(completeResDuplicateUsername.error(), ErrorCode::Auth::ServiceError::UserAlreadyExists);
}

TEST_F(AuthServiceTest, CompleteRegistrationFailsOnWrongCode)
{
    expectEmailSent();

    auto startRes = m_authService.startRegistrationSession("test@gmail.com");
    ASSERT_TRUE(startRes.isOk());

    int wrongCode = m_mailSpy.m_lastSentCode + 1;

    auto completeRes = m_authService.completeRegistration(
        startRes.data().verificationId, wrongCode, "user1", "pass123"
        );

    EXPECT_FALSE(completeRes.isOk());
    EXPECT_EQ(completeRes.error(), ErrorCode::Auth::ServiceError::InvalidCredentials);

    EXPECT_FALSE(m_userRep.exists("user1"));
}

TEST_F(AuthServiceTest, CompleteRegistrationFailsOnInvalidSessionId)
{
    auto completeRes = m_authService.completeRegistration(
        "some-fake-uuid", 123456, "hacker", "pass"
        );

    EXPECT_FALSE(completeRes.isOk());
    EXPECT_EQ(completeRes.error(), ErrorCode::Auth::ServiceError::SessionDoesNotExists);
}

TEST_F(AuthServiceTest, CompleteRegistrationFailsOnExpiredSession)
{
    expectEmailSent();

    auto startRes = m_authService.startRegistrationSession("some_user@gmail.com");
    ASSERT_TRUE(startRes.isOk());

    // skip 2 hours to make registration session expired
    m_timeProvider.m_manualTime = m_timeProvider.m_manualTime.addSecs(2 * 60 * 60);

    auto completeRes = m_authService.completeRegistration(
        startRes.data().verificationId, m_mailSpy.m_lastSentCode, "some_user", "pass"
        );

    EXPECT_FALSE(completeRes.isOk());
    EXPECT_EQ(completeRes.error(), ErrorCode::Auth::ServiceError::SessionExpired);
}

TEST_F(AuthServiceTest, ClearExpiredRegistrationSessionsWorks)
{
    auto startRes = m_authService.startRegistrationSession("forgotten@gmail.com");
    ASSERT_TRUE(startRes.isOk());

    // skip 2 hours to make registration session expired
    m_timeProvider.m_manualTime = m_timeProvider.m_manualTime.addSecs(2 * 60 * 60);

    m_authService.clearExpiredRegistrationSessions();

    auto completeRes = m_authService.completeRegistration(
        startRes.data().verificationId, m_mailSpy.m_lastSentCode, "forgotten", "pass"
        );

    EXPECT_FALSE(completeRes.isOk());
    EXPECT_EQ(completeRes.error(), ErrorCode::Auth::ServiceError::SessionDoesNotExists);
}

TEST_F(AuthServiceTest, SuccessfulLoginFlow)
{
    expectEmailSent();

    updateRegistrationData(1);
    registerUser(m_email, m_username, m_password);

    auto loginRes = m_authService.login(m_username, m_password);

    ASSERT_TRUE(loginRes.isOk());
    EXPECT_EQ(loginRes.data().userName, m_username);
    EXPECT_FALSE(loginRes.data().accessToken.isEmpty());
    // access token must consist of 2 parts: [ID] and [TOKEN]
    EXPECT_EQ(accessTokenPartsCount(loginRes.data().accessToken), 2);

    QString tokenId = parseToken(loginRes.data().accessToken).first;
    EXPECT_TRUE(m_tokenRep.exists(tokenId));
}

// test that login still works after failed authentication
TEST_F(AuthServiceTest, AuthenticateAndLoginWorks)
{
    expectEmailSent();

    updateRegistrationData(1);
    auto regRes = registerUser(m_email, m_username, m_password);
    auto authResCorrect = m_authService.authenticateByToken(
        regRes.data().accessToken);

    EXPECT_TRUE(authResCorrect.isOk());
    EXPECT_EQ(authResCorrect.data().userName, m_username);

    // skip 15 days to make user session expired
    m_timeProvider.m_manualTime = m_timeProvider.m_manualTime.addDays(15);

    auto authResExpired = m_authService.authenticateByToken(
        regRes.data().accessToken);

    EXPECT_FALSE(authResExpired.isOk());
    EXPECT_EQ(authResExpired.error(), ErrorCode::Auth::ServiceError::SessionExpired);

    auto loginRes = m_authService.login(m_username, m_password);

    ASSERT_TRUE(loginRes.isOk());
    EXPECT_EQ(loginRes.data().userName, m_username);
    EXPECT_FALSE(loginRes.data().accessToken.isEmpty());
    // access token must consist of 2 parts: [ID] and [TOKEN]
    EXPECT_EQ(accessTokenPartsCount(loginRes.data().accessToken), 2);

    QString tokenId = parseToken(loginRes.data().accessToken).first;
    EXPECT_TRUE(m_tokenRep.exists(tokenId));
}


TEST_F(AuthServiceTest, AuthenticateByTokenFailsOnWrongSecret)
{
    expectEmailSent();

    updateRegistrationData(1);
    auto regRes = registerUser(m_email, m_username, m_password);

    auto tokenParts = parseToken(regRes.data().accessToken);
    QString tokenId = tokenParts.first;
    QString tokenSecret = tokenParts.second;
    auto authRes = m_authService.authenticateByToken(
        tokenId + ".WrongSecret" + tokenSecret);

    EXPECT_FALSE(authRes.isOk());
    EXPECT_EQ(authRes.error(), ErrorCode::Auth::ServiceError::InvalidCredentials);
}

TEST_F(AuthServiceTest, AuthenticateByTokenFailsOnInvalidFormat)
{
    auto authRes = m_authService.authenticateByToken("some_random_string");

    EXPECT_FALSE(authRes.isOk());
    EXPECT_EQ(authRes.error(), ErrorCode::Auth::ServiceError::InvalidCredentials);
}
