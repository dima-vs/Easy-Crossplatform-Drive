#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>
#include <QDebug>

#include "token_repository.h"

TokenRepository::TokenRepository(DatabaseManager& db)
    : m_db(db) {}

bool TokenRepository::addNewToken(const Token& token) const
{
    if (!token.isValid())
    {
        qCritical() << "token is not valid";
        return false;
    }

    QSqlQuery query(m_db.database());
    query.prepare("INSERT INTO tokens (id, token_hash, user_id, expires_at) "
                  "VALUES (:id, :hash, :user_id, :expires_at)");

    query.bindValue(":id", token.id());
    query.bindValue(":hash", token.tokenHash());
    query.bindValue(":user_id", token.userId());
    query.bindValue(":expires_at", token.expiresAt());

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

Token TokenRepository::getToken(const QString &id) const
{
    QSqlQuery query(m_db.database());
    query.prepare("SELECT id, token_hash, user_id, expires_at FROM tokens WHERE id = :id");
    query.bindValue(":id", id);

    if (!query.exec())
    {
        qCritical() << query.lastError().text();
        return Token();
    }

    if (query.next())
    {
        return Token(
            query.value("id").toString(),
            query.value("token_hash").toString(),
            query.value("user_id").toInt(),
            query.value("expires_at").toDateTime()
            );
    }

    qWarning() << "token with id" << id << "not found";
    return Token();
}

bool TokenRepository::deleteToken(const QString &id) const
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

bool TokenRepository::cleanExpiredTokens() const
{
    QSqlQuery query(m_db.database());
    query.prepare("DELETE FROM tokens WHERE expires_at < DATETIME('now')");

    if (!query.exec()) {
        qCritical() << query.lastError().text();
        return false;
    }

    int removed = query.numRowsAffected();
    if (removed > 0) {
        qDebug() << "removed" << removed << "expired sessions.";
    }

    return true;
}


bool TokenRepository::deleteByUser(int userId) const
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
