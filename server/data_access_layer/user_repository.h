#ifndef USER_REPOSITORY_H
#define USER_REPOSITORY_H

#include <QString>
#include <QList>
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
    User getUser(const QString& username) const;
    QList<User> getAllUsers() const;
    bool exists(int id) const;
    bool exists(const QString &username) const;
    bool existsByEmail(const QString &email) const;
    bool deleteUser(const QString& username) const;
};

#endif // USER_REPOSITORY_H
