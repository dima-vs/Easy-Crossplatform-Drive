#include <QSqlQuery>
#include <QVariant>
#include <QDebug>
#include "user_repository.h"

UserRepository::UserRepository(DatabaseManager &db):
    m_db(db) {}

bool UserRepository::add(UserRecord &record) const
{
    if (!record.isValid())
    {
        qCritical() << "could not add a new user:" <<
            "user is not valid";
        return false;
    }

    QSqlQuery query(m_db.database());
    query.prepare("INSERT INTO users(username, email, password_hash) "
                  "VALUES (:username, :email, :password_hash)");
    query.bindValue(":username", record.username());
    query.bindValue(":email", record.email());
    query.bindValue(":password_hash", record.passwordHash());

    bool qResult = query.exec();

    if (qResult)
    {
        // try to update source object's id
        QVariant id = query.lastInsertId();
        if (id.isValid()) {
            qDebug() << "inserted ID:" << id.toInt();
            record.setId(id.toInt());
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

UserRecord UserRepository::findById(int id) const
{
    QSqlQuery query(m_db.database());
    query.prepare("SELECT id, username, email, password_hash FROM users WHERE id = :id");
    query.bindValue(":id", id);

    if (!query.exec())
    {
        qCritical() << query.lastError().text();
        return UserRecord();
    }

    if (query.next())
    {
        return UserRecord(
            query.value("id").toInt(),
            query.value("username").toString(),
            query.value("email").toString(),
            query.value("password_hash").toString()
            );
    }

    qWarning() << "user with id" << id << "not found";
    return UserRecord();
}

UserRecord UserRepository::findByUsername(const QString &username) const
{
    QSqlQuery query(m_db.database());
    query.prepare("SELECT id, username, email, password_hash FROM users WHERE username = :username");
    query.bindValue(":username", username);

    if (!query.exec())
    {
        qCritical() << query.lastError().text();
        return UserRecord();
    }

    if (query.next())
    {
        return UserRecord(
            query.value("id").toInt(),
            query.value("username").toString(),
            query.value("email").toString(),
            query.value("password_hash").toString()
            );
    }

    qWarning() << "user" << username << "not found";
    return UserRecord();
}

QList<UserRecord> UserRepository::findAll() const
{
    QList<UserRecord> users;
    QSqlQuery query(m_db.database());
    query.prepare("SELECT id, username, email, password_hash FROM users");

    if (!query.exec())
    {
        qCritical() << "getAllUsers failed:" << query.lastError().text();
        return users;
    }

    while (query.next())
    {
        users.append(UserRecord(
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

bool UserRepository::existsByEmail(const QString &email) const
{
    QSqlQuery query(m_db.database());
    query.prepare("SELECT 1 FROM users WHERE email = :email LIMIT 1");
    query.bindValue(":email", email);

    if (!query.exec())
    {
        qCritical() << "existsByEmail failed:" << query.lastError().text();
        return false;
    }

    return query.next();
}

bool UserRepository::remove(const QString &username) const
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
