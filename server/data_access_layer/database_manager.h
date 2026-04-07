#ifndef DATABASE_MANAGER_H
#define DATABASE_MANAGER_H

#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QString>

class DatabaseManager
{
private:
    QSqlDatabase m_mainDB;
    bool m_isOpen;
public:
    DatabaseManager(
        const QString& dbName = "main_database.db",
        const QString& connectionName = "main_connection.db"
        );
    ~DatabaseManager();
    void initDatabase(const QString& dbName, const QString& connectionName);
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
