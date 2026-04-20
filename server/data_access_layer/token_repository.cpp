#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>
#include <QDebug>

#include "token_repository.h"

TokenRepository::TokenRepository(DatabaseManager& db)
    : m_db(db) {}

bool TokenRepository::add(const TokenRecord& record) const
{
    if (!record.isValid())
    {
        qCritical() << "token is not valid";
        return false;
    }

    QSqlQuery query(m_db.database());
    query.prepare("INSERT INTO tokens (id, token_hash, user_id, expires_at) "
                  "VALUES (:id, :hash, :user_id, :expires_at)");

    query.bindValue(":id", record.id());
    query.bindValue(":hash", record.tokenHash());
    query.bindValue(":user_id", record.userId());
    query.bindValue(":expires_at", record.expiresAt());

    if (!query.exec())
    {
        qCritical() << query.lastError().text();
        return false;
    }

    return true;
}

bool TokenRepository::exists(const QString &id) const
{
    QSqlQuery query(m_db.database());

    query.prepare("SELECT 1 FROM tokens WHERE id = :id LIMIT 1");
    query.bindValue(":id", id);

    if (!query.exec())
    {
        qCritical() << query.lastError().text();
        return false;
    }

    return query.next();
}

TokenRecord TokenRepository::findById(const QString &id) const
{
    QSqlQuery query(m_db.database());
    query.prepare("SELECT id, token_hash, user_id, expires_at FROM tokens WHERE id = :id");
    query.bindValue(":id", id);

    if (!query.exec())
    {
        qCritical() << query.lastError().text();
        return TokenRecord();
    }

    if (query.next())
    {
        return TokenRecord(
            query.value("id").toString(),
            query.value("token_hash").toString(),
            query.value("user_id").toInt(),
            query.value("expires_at").toDateTime()
            );
    }

    qWarning() << "token with id" << id << "not found";
    return TokenRecord();
}

bool TokenRepository::remove(const QString &id) const
{
    QSqlQuery query(m_db.database());
    query.prepare("DELETE FROM tokens WHERE id = :id");
    query.bindValue(":id", id);

    if (!query.exec())
    {
        qCritical() << query.lastError().text();
        return false;
    }

    if (query.numRowsAffected() == 0)
    {
        qWarning() << "token with id" << id << "not found";
        return false;
    }

    qInfo() << "token with id" << id << "deleted";

    return true;
}

bool TokenRepository::cleanExpired(QDateTime currentDateTime) const
{
    QSqlQuery query(m_db.database());
    query.prepare("DELETE FROM tokens WHERE expires_at < :now");
    query.bindValue(":now", currentDateTime);

    if (!query.exec())
    {
        qCritical() << query.lastError().text();
        return false;
    }

    int removed = query.numRowsAffected();
    if (removed > 0) {
        qDebug() << "removed" << removed << "expired sessions.";
    }

    return true;
}

bool TokenRepository::removeByUser(int userId) const
{
    QSqlQuery query(m_db.database());
    query.prepare("DELETE FROM tokens WHERE user_id = :user_id");
    query.bindValue(":user_id", userId);

    if (!query.exec()) {
        qCritical() << query.lastError().text();
        return false;
    }

    qInfo() << "all tokens for user" << userId << "deleted.";
    return true;
}
