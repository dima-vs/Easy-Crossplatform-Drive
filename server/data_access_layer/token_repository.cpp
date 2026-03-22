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
        qCritical() << "[ERROR] TokenRepository::addNewToken: token is not valid";
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
        qCritical() << "[ERROR] TokenRepository::addNewToken:" << query.lastError().text();
        return false;
    }

    return true;
}

Token TokenRepository::getToken(QString id) const
{
    QSqlQuery query(m_db.database());
    query.prepare("SELECT id, token_hash, user_id, expires_at FROM tokens WHERE id = :id");
    query.bindValue(":id", id);

    if (!query.exec())
    {
        qCritical() << "[ERROR] TokenRepository::getToken:" << query.lastError().text();
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

    qDebug() << "[INFO] TokenRepository::getToken: Token" << id << "not found";
    return Token();
}

bool TokenRepository::deleteToken(QString id) const
{
    QSqlQuery query(m_db.database());
    query.prepare("DELETE FROM tokens WHERE id = :id");
    query.bindValue(":id", id);

    if (!query.exec())
    {
        qCritical() << "[ERROR] TokenRepository::deleteToken:" << query.lastError().text();
        return false;
    }

    if (query.numRowsAffected() == 0)
    {
        qWarning() << "[WARN] TokenRepository::deleteToken: No token found with id" << id;
        return false;
    }

    return true;
}

bool TokenRepository::cleanExpiredTokens() const
{
    QSqlQuery query(m_db.database());
    query.prepare("DELETE FROM tokens WHERE expires_at < DATETIME('now')");

    if (!query.exec()) {
        qCritical() << "[ERROR] TokenRepository::cleanExpiredTokens:" << query.lastError().text();
        return false;
    }

    int removed = query.numRowsAffected();
    if (removed > 0) {
        qDebug() << "[INFO] TokenRepository::cleanExpiredTokens: removed" << removed << "expired sessions.";
    }

    return true;
}


bool TokenRepository::deleteByUser(int userId) const
{
    QSqlQuery query(m_db.database());
    query.prepare("DELETE FROM tokens WHERE user_id = :user_id");
    query.bindValue(":user_id", userId);

    if (!query.exec()) {
        qCritical() << "[ERROR] TokenRepository::deleteByUser:" << query.lastError().text();
        return false;
    }

    qDebug() << "[INFO] TokenRepository::deleteByUser: all tokens for user" << userId << "deleted.";
    return true;
}
