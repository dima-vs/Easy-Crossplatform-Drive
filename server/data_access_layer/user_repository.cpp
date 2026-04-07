#include <QSqlQuery>
#include <QVariant>
#include <QDebug>
#include "user_repository.h"

UserRepository::UserRepository(DatabaseManager &db):
    m_db(db) {}

bool UserRepository::addNewUser(User &user) const
{
    if (!user.isValid())
    {
        qCritical() << "could not add a new user:" <<
            "user is not valid";
        return false;
    }

    QSqlQuery query(m_db.database());
    query.prepare("INSERT INTO users(username, email, password_hash) "
                  "VALUES (:username, :email, :password_hash)");
    query.bindValue(":username", user.username());
    query.bindValue(":email", user.email());
    query.bindValue(":password_hash", user.passwordHash());

    bool qResult = query.exec();

    if (qResult)
    {
        // try to update source object's id
        QVariant id = query.lastInsertId();
        if (id.isValid()) {
            qDebug() << "inserted ID:" << id.toInt();
            user.setId(id.toInt());
        } else {
            qDebug() << "lastInsertId is not supported by this driver";
        }
    }
    else
    {
        qCritical() << query.lastError().text();
    }

    return qResult;
}

User UserRepository::getUser(int id) const
{
    QSqlQuery query(m_db.database());
    query.prepare("SELECT id, username, email, password_hash FROM users WHERE id = :id");
    query.bindValue(":id", id);

    if (!query.exec())
    {
        qCritical() << query.lastError().text();
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

    qWarning() << "user with id" << id << "not found";
    return User();
}

User UserRepository::getUser(const QString &username) const
{
    QSqlQuery query(m_db.database());
    query.prepare("SELECT id, username, email, password_hash FROM users WHERE username = :username");
    query.bindValue(":username", username);

    if (!query.exec())
    {
        qCritical() << query.lastError().text();
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

    qWarning() << "user" << username << "not found";
    return User();
}

QList<User> UserRepository::getAllUsers() const
{
    QList<User> users;
    QSqlQuery query(m_db.database());
    query.prepare("SELECT id, username, email, password_hash FROM users");

    if (!query.exec())
    {
        qCritical() << "getAllUsers failed:" << query.lastError().text();
        return users;
    }

    while (query.next())
    {
        users.append(User(
            query.value("id").toInt(),
            query.value("username").toString(),
            query.value("email").toString(),
            query.value("password_hash").toString()
            ));
    }

    return users;
}

bool UserRepository::exists(int id) const
{
    QSqlQuery query(m_db.database());

    query.prepare("SELECT 1 FROM users WHERE id = :id LIMIT 1");
    query.bindValue(":id", id);

    if (!query.exec())
    {
        qCritical() << "exists(id) failed:" << query.lastError().text();
        return false;
    }

    return query.next();
}

bool UserRepository::exists(const QString &username) const
{
    QSqlQuery query(m_db.database());
    query.prepare("SELECT 1 FROM users WHERE username = :username LIMIT 1");
    query.bindValue(":username", username);

    if (!query.exec())
    {
        qCritical() << "exists(username) failed:" << query.lastError().text();
        return false;
    }

    return query.next();
}

bool UserRepository::deleteUser(const QString &username) const
{
    QSqlQuery query(m_db.database());
    query.prepare("DELETE FROM users WHERE username = :username");
    query.bindValue(":username", username);


    bool qResult = query.exec();
    if (!qResult)
    {
        qCritical() << query.lastError().text();
        return false;
    }

    if (query.numRowsAffected() == 0)
    {
        qCritical() << "could not delete user " << username;
        return false;
    }

    qInfo() << "user" << username << "deleted";
    return true;
}
