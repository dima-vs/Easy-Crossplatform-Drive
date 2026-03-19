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
    //QSqlQuery m_mainDBQuery;
public:
    DatabaseManager();
    ~DatabaseManager() {};
    void initDatabase();
    QSqlDatabase database();
    bool isDBOpen();
private:
    void createTables();
    void createUsersTable();
    void createFilesTable();
};

#endif // DATABASE_MANAGER_H
