#ifndef TOKEN_REPOSITORY_H
#define TOKEN_REPOSITORY_H

#include <QString>
#include <QDateTime>
#include "database_manager.h"
#include "token.h"

class TokenRepository
{
private:
    DatabaseManager& m_db;
public:
    TokenRepository(DatabaseManager& db);
    bool addNewToken(const Token& token) const;
    Token getToken(const QString& id) const;
    bool exists(const QString& id) const;
    bool deleteToken(const QString& id) const;
    bool cleanExpiredTokens(QDateTime currentDateTime) const;
    bool deleteByUser(int userId) const;
};

#endif // TOKEN_REPOSITORY_H
