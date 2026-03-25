#ifndef DATABASE_MANAGER_H
#define DATABASE_MANAGER_H

#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>

class DatabaseManager
{
private:
    QSqlDatabase m_mainDB;
    bool m_isOpen;
public:
    DatabaseManager();
    ~DatabaseManager();
    void initDatabase();
    QSqlDatabase database() const;
    bool isDBOpen() const;
private:
    void createTables() const;
    void createUsersTable() const;
    void createFilesTable() const;
    void createTokensTable() const;
    void createUniqueRootIndexOnFiles() const;
    bool createChildUniqueIndex() const;
    bool createRootUniqueIndex() const;
};

#endif // DATABASE_MANAGER_H
