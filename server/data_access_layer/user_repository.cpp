#include <QSqlQuery>
#include <QDebug>
#include "user_repository.h"

UserRepository::UserRepository(DatabaseManager &db):
    m_db(db) {}

bool UserRepository::addNewUser(const User& user) const
{
    if (!user.isValid())
    {
        qCritical() << "[ERROR] UserRepository::addNewUser: could not add a new user. " <<
            "User is not valid";
        return false;
    }

    QSqlQuery query(m_db.database());
    query.prepare("INSERT INTO users(username, email, password_hash) "
                  "VALUES (:username, :email, :password_hash)");
    query.bindValue(":username", user.username());
    query.bindValue(":email", user.email());
    query.bindValue(":password_hash", user.passwordHash());

    bool qResult = query.exec();

    if (!qResult)
        qCritical() << "[ERROR] UserRepository::addNewUser: " << query.lastError().text();
    return qResult;
}

User UserRepository::getUser(int id) const
{
    QSqlQuery query(m_db.database());
    query.prepare("SELECT id, username, email, password_hash FROM users WHERE id = :id");
    query.bindValue(":id", id);

    if (!query.exec())
    {
        qCritical() << "[ERROR] UserRepository::getUser(id): query failed. " <<
            query.lastError().text();
        return User();
    }

    if (query.next())
    {
        return User(
            query.value("id").toInt(),
            query.value("username").toString(),
            query.value("email").toString(),
            query.value("password_hash").toString()
            );
    }

    qDebug() << "[INFO] UserRepository::getUser(id): user with id " <<
        id << " not found.";
    return User();
}

User UserRepository::getUser(QString username) const
{
    QSqlQuery query(m_db.database());
    query.prepare("SELECT id, username, email, password_hash FROM users WHERE username = :username");
    query.bindValue(":username", username);

    if (!query.exec())
    {
        qCritical() << "[ERROR] UserRepository::getUser(username): query failed." <<
            query.lastError().text();
        return User();
    }

    if (query.next())
    {
        return User(
            query.value("id").toInt(),
            query.value("username").toString(),
            query.value("email").toString(),
            query.value("password_hash").toString()
            );
    }

    qDebug() << "[INFO] UserRepository::getUser(username): user " <<
        username << " not found.";
    return User();
}

bool UserRepository::deleteUser(QString username) const
{
    QSqlQuery query(m_db.database());
    query.prepare("DELETE FROM users WHERE username = :username");
    query.bindValue(":username", username);


    bool qResult = query.exec();
    if (!qResult)
    {
        qCritical() << "[ERROR] UserRepository::deleteUser(username): query failed." <<
            query.lastError().text();
        return false;
    }

    if (query.numRowsAffected() == 0)
    {
        qCritical() << "[ERROR] UserRepository::deleteUser(username): " <<
            "could not delete user " << username;
        return false;
    }

    qDebug() << "[SUCCESS] UserRepository::deleteUser(username): user " << username << "deleted.";
    return true;
}
