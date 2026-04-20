#include <gtest/gtest.h>

#include <QUuid>
#include <QString>
#include <QList>
#include "database_manager.h"
#include "user_repository.h"
#include "user_record.h"

class UserRepositoryTest : public testing::Test
{
protected:
    DatabaseManager m_DBManager;
    UserRepository m_userRep;

    UserRepositoryTest() :
        m_DBManager(":memory:", QUuid::createUuid().toString()),
        m_userRep(m_DBManager) {}

    bool addUser(int index)
    {
        UserRecord user(
            QString("user%1").arg(index),
            QString("user%1@gmail.com").arg(index),
            QString("password%1").arg(index)
        );
        return m_userRep.addNewUser(user);
    }

    bool deleteUser(int index)
    {
        return m_userRep.deleteUser(QString("user%1").arg(index));
    }

    bool addUsersRange(int start, int end)
    {
        if (start > end)
            return false;
        for (int i = start; i <= end; ++i)
        {
            if (!addUser(i))
            {
                return false;
            }
        }
        return true;
    }

    bool deleteUsersRange(int start, int end)
    {
        if (start > end)
            return false;
        for (int i = start; i <= end; ++i)
        {
            if (!deleteUser(i))
            {
                return false;
            }
        }
        return true;
    }
};

TEST_F(UserRepositoryTest, UserStorageWorks)
{
    // at the beginning size has to be 0
    EXPECT_EQ(m_userRep.getAllUsers().size(), 0);
    EXPECT_FALSE(m_userRep.getUser(1).isValid());
    ASSERT_TRUE(addUsersRange(1, 10)); // add 10 users

    EXPECT_EQ(m_userRep.getAllUsers().size(), 10); // size has to be 10

    QString userName = m_userRep.getUser("user3").username();
    EXPECT_STREQ(userName.toStdString().c_str(), "user3");

    EXPECT_TRUE(deleteUsersRange(8, 9)); // remove two users
    EXPECT_EQ(m_userRep.getAllUsers().size(), 8); // now size is 8

    EXPECT_TRUE(m_userRep.exists("user1"));
    EXPECT_TRUE(m_userRep.exists(5));
    EXPECT_TRUE(m_userRep.existsByEmail("user5@gmail.com"));

    EXPECT_FALSE(m_userRep.exists("user100"));
    EXPECT_FALSE(m_userRep.exists(50));
    EXPECT_FALSE(m_userRep.existsByEmail("user9@gmail.com"));
}

TEST_F(UserRepositoryTest, UniqueConstraintWork)
{
    ASSERT_TRUE(addUsersRange(1, 5));

    EXPECT_FALSE(addUsersRange(1, 5));
    EXPECT_FALSE(addUser(3));

    EXPECT_TRUE(addUser(7));
    EXPECT_TRUE(addUsersRange(20, 30));

    EXPECT_EQ(m_userRep.getAllUsers().size(), 17);

    EXPECT_FALSE(addUsersRange(19, 33));
}

TEST_F(UserRepositoryTest, DeleteNotExisting)
{
    ASSERT_TRUE(addUsersRange(1, 10));
    EXPECT_EQ(m_userRep.getAllUsers().size(), 10);

    EXPECT_FALSE(deleteUser(20));
    EXPECT_FALSE(deleteUsersRange(-1, 100));
    EXPECT_EQ(m_userRep.getAllUsers().size(), 10);

    EXPECT_TRUE(deleteUsersRange(1, 10));
    EXPECT_EQ(m_userRep.getAllUsers().size(), 0);

    EXPECT_FALSE(deleteUsersRange(1, 10));
    EXPECT_FALSE(deleteUser(4));
    EXPECT_EQ(m_userRep.getAllUsers().size(), 0);
}

TEST_F(UserRepositoryTest, UniqueEmailAndUserNameConstraint)
{
    ASSERT_TRUE(addUser(1)); // user1, user1@gmail.com

    UserRecord duplicateEmail("user99", "user1@gmail.com", "psw");
    EXPECT_FALSE(m_userRep.addNewUser(duplicateEmail));

    UserRecord duplicateName("user1", "user99@gmail.com", "psw");
    EXPECT_FALSE(m_userRep.addNewUser(duplicateName));

    EXPECT_EQ(m_userRep.getAllUsers().size(), 1);
}
