#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <QUuid>
#include <QString>
#include <QDateTime>

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


class AuthServiceTest : public testing::Test
{
protected:
    DatabaseManager m_DBManager;
    UserRepository m_userRep;
    TokenRepository m_tokenRep;

    MailServiceSpy m_mailSpy;
    MockTimeProvider m_timeProvider;
    Service::Auth::AuthService m_authService;

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

    EXPECT_TRUE(m_userRep.exists(username));
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
