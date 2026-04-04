#ifndef USER_REPOSITORY_H
#define USER_REPOSITORY_H

#include <QString>
#include "database_manager.h"
#include "user.h"

class UserRepository
{
private:
    DatabaseManager& m_db;
public:
    UserRepository(DatabaseManager& db);
    bool addNewUser(User& user) const;
    User getUser(int id) const;
    User getUser(QString username) const;
    bool deleteUser(QString username) const;
};

#endif // USER_REPOSITORY_H
