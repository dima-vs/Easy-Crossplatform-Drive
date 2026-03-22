#ifndef TOKEN_REPOSITORY_H
#define TOKEN_REPOSITORY_H

#include <QString>
#include "database_manager.h"
#include "token.h"

class TokenRepository
{
private:
    DatabaseManager& m_db;
public:
    TokenRepository(DatabaseManager& db);
    bool addNewToken(const Token& token) const;
    Token getToken(QString id) const;
    bool deleteToken(QString id) const;
    bool cleanExpiredTokens() const;
    bool deleteByUser(int userId) const;
};

#endif // TOKEN_REPOSITORY_H
