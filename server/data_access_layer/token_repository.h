#ifndef TOKEN_REPOSITORY_H
#define TOKEN_REPOSITORY_H

#include <QString>
#include <QDateTime>
#include "database_manager.h"
#include "token_record.h"

class TokenRepository
{
private:
    DatabaseManager& m_db;
public:
    TokenRepository(DatabaseManager& db);
    bool add(const TokenRecord& record) const;
    TokenRecord findById(const QString& id) const;
    bool exists(const QString& id) const;
    bool remove(const QString& id) const;
    bool removeByUser(int userId) const;
    bool cleanExpired(QDateTime currentDateTime) const;
};

#endif // TOKEN_REPOSITORY_H
