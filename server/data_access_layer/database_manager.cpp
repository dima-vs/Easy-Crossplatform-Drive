#include <QDebug>

#include "database_manager.h"

DatabaseManager::DatabaseManager(): m_isOpen(false)
{
    initDatabase();
}

void DatabaseManager::initDatabase()
{
    m_mainDB = QSqlDatabase::addDatabase("QSQLITE");
    m_mainDB.setDatabaseName("main_database.db");
    m_isOpen = m_mainDB.open();

    if (!m_isOpen)
        qDebug() << "[ERROR]: could not open database 'main_database.db'";

    createTables();
}

bool DatabaseManager::isDBOpen()
{
    return m_isOpen;
}

void DatabaseManager::createTables()
{
    createUsersTable();
    createFilesTable();
}

void DatabaseManager::createUsersTable()
{
    QSqlQuery query(m_mainDB);
    bool ok = query.exec("CREATE TABLE IF NOT EXISTS users("
               "id INTEGER PRIMARY KEY AUTOINCREMENT,"
               "username TEXT UNIQUE NOT NULL,"
               "email TEXT UNIQUE,"
               "password TEXT NOT NULL)");

    if (!ok)
        qDebug() << "[ERROR]: could not create table 'users'";
}

void DatabaseManager::createFilesTable()
{
    QSqlQuery query(m_mainDB);
    bool ok = query.exec("CREATE TABLE IF NOT EXISTS files("
               "id INTEGER PRIMARY KEY AUTOINCREMENT,"
               "owner_id INTEGER NOT NULL,"
               "type TEXT NOT NULL,"
               "logical_name TEXT NOT NULL,"
               "server_name TEXT NOT NULL,"
               "size INTEGER,"
               "upload_time DATETIME DEFAULT CURRENT_TIMESTAMP,"
               "parent_id INTEGER DEFAULT 0,"
               "FOREIGN KEY(owner_id) REFERENCES users(id))");

    if (!ok)
        qDebug() << "[ERROR]: could not create table 'files'";
}

QSqlDatabase DatabaseManager::database()
{
    return m_mainDB;
}
