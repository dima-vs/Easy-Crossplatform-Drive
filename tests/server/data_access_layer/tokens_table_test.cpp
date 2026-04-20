#include <gtest/gtest.h>
#include <QUuid>
#include <QString>
#include <QDateTime>

#include "database_manager.h"
#include "user_repository.h"
#include "token_repository.h"
#include "user_record.h"
#include "token.h"

class TokenRepositoryTest : public testing::Test
{
protected:
    DatabaseManager m_DBManager;
    UserRepository m_userRep;
    TokenRepository m_tokenRep;

    int m_testUserId;

    TokenRepositoryTest() :
        m_DBManager(":memory:", QUuid::createUuid().toString()),
        m_userRep(m_DBManager),
        m_tokenRep(m_DBManager)
    {
        UserRecord user("testuser", "test@gmail.com", "12345");
        m_userRep.addNewUser(user);
        m_testUserId = user.id();
    }

    bool createToken(const QString& tokenId, int offsetSecs = 3600, int userId = -1)
    {
        if (userId == -1)
            userId = m_testUserId;

        QDateTime expiresAt = QDateTime::currentDateTimeUtc().addSecs(offsetSecs);
        Token token(tokenId, "hash_" + tokenId, userId, expiresAt);

        return m_tokenRep.addNewToken(token);
    }
};

TEST_F(TokenRepositoryTest, TokenStorageWorks)
{
    EXPECT_FALSE(m_tokenRep.exists("token_1"));

    QDateTime expires = QDateTime::currentDateTimeUtc().addDays(1);
    Token t("token_1", "hash_abc", m_testUserId, expires);

    EXPECT_TRUE(m_tokenRep.addNewToken(t));
    EXPECT_TRUE(m_tokenRep.exists("token_1"));

    Token fetched = m_tokenRep.getToken("token_1");
    EXPECT_TRUE(fetched.isValid());
    EXPECT_FALSE(fetched.isExpired(expires.addDays(-1)));
    EXPECT_EQ(fetched.id(), QString("token_1"));
    EXPECT_EQ(fetched.tokenHash(), QString("hash_abc"));
    EXPECT_EQ(fetched.userId(), m_testUserId);

    EXPECT_EQ(fetched.expiresAt(), expires);
}

TEST_F(TokenRepositoryTest, PrimaryKeyConstraint)
{
    EXPECT_TRUE(createToken("not_unique_id_1"));
    EXPECT_FALSE(createToken("not_unique_id_1"))
        << "Should not allow duplicate token IDs";
}

TEST_F(TokenRepositoryTest, ForeignKeyConstraintFailsOnBadUser)
{
    int badUserId = 9999;
    EXPECT_FALSE(createToken("bad_token", 3600, badUserId));
}

TEST_F(TokenRepositoryTest, DeleteTokenWorks)
{
    createToken("tok_1");
    EXPECT_TRUE(m_tokenRep.exists("tok_1"));

    EXPECT_TRUE(m_tokenRep.deleteToken("tok_1"));
    EXPECT_FALSE(m_tokenRep.exists("tok_1"));

    EXPECT_FALSE(m_tokenRep.deleteToken("tok_1"));
}

TEST_F(TokenRepositoryTest, DeleteByUserWorks)
{
    UserRecord user2("second_user", "second@gmail.com", "pass");
    ASSERT_TRUE(m_userRep.addNewUser(user2));
    int secondUserId = user2.id();

    createToken("t_user1_a", 3600, m_testUserId);
    createToken("t_user1_b", 3600, m_testUserId);
    createToken("t_user2_a", 3600, secondUserId);

    EXPECT_TRUE(m_tokenRep.deleteByUser(m_testUserId));

    EXPECT_FALSE(m_tokenRep.exists("t_user1_a"));
    EXPECT_FALSE(m_tokenRep.exists("t_user1_b"));

    EXPECT_TRUE(m_tokenRep.exists("t_user2_a"));
}

TEST_F(TokenRepositoryTest, CleanExpiredTokensWorks)
{
    QDateTime now = QDateTime::currentDateTimeUtc();

    // tokens from the future
    Token future1("future_1", "h1", m_testUserId, now.addDays(1));
    Token future2("future_2", "h2", m_testUserId, now.addSecs(60));

    // tokens from the past
    Token past1("past_1", "h3", m_testUserId, now.addDays(-1));
    Token past2("past_2", "h4", m_testUserId, now.addSecs(-60));

    // token from now
    Token border("border", "h5", m_testUserId, now);

    ASSERT_TRUE(m_tokenRep.addNewToken(future1));
    ASSERT_TRUE(m_tokenRep.addNewToken(future2));
    ASSERT_TRUE(m_tokenRep.addNewToken(past1));
    ASSERT_TRUE(m_tokenRep.addNewToken(past2));
    ASSERT_TRUE(m_tokenRep.addNewToken(border));

    EXPECT_TRUE(m_tokenRep.cleanExpiredTokens(now));

    EXPECT_TRUE(m_tokenRep.exists("future_1"));
    EXPECT_TRUE(m_tokenRep.exists("future_2"));

    EXPECT_FALSE(m_tokenRep.exists("past_1"));
    EXPECT_FALSE(m_tokenRep.exists("past_2"));

    // if current time = expiresAt token still must remain
    EXPECT_TRUE(m_tokenRep.exists("border"))
        << "Border token should not be deleted due to strict '<' operator in SQL";
}
