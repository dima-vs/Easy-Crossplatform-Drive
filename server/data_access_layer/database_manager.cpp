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
        qCritical() << "[ERROR]: could not open database 'main_database.db'";

    createTables();
}

bool DatabaseManager::isDBOpen() const
{
    return m_isOpen;
}

void DatabaseManager::createTables() const
{
    createUsersTable();
    createFilesTable();
}

void DatabaseManager::createUsersTable() const
{
    QSqlQuery query(m_mainDB);
    bool ok = query.exec("CREATE TABLE IF NOT EXISTS users("
               "id INTEGER PRIMARY KEY AUTOINCREMENT,"
               "username TEXT UNIQUE NOT NULL,"
               "email TEXT UNIQUE NOT NULL,"
               "password_hash TEXT NOT NULL)");

    if (!ok)
        qCritical() << "[ERROR]: could not create table 'users'";
}

void DatabaseManager::createFilesTable() const
{
    QSqlQuery query(m_mainDB);
    bool ok = query.exec("CREATE TABLE IF NOT EXISTS files("
               "id INTEGER PRIMARY KEY AUTOINCREMENT,"
               "owner_id INTEGER NOT NULL,"
               "type TEXT NOT NULL,"
               "logical_name TEXT NOT NULL,"
               "server_name TEXT UNIQUE,"
               "size INTEGER NOT NULL,"
               "upload_time DATETIME DEFAULT CURRENT_TIMESTAMP,"
               "parent_id INTEGER DEFAULT NULL,"
               "FOREIGN KEY(owner_id) REFERENCES users(id)"
               "FOREIGN KEY(parent_id) REFERENCES files(id)"
               "UNIQUE(owner_id, parent_id, logical_name))");

    if (!ok)
        qCritical() << "[ERROR]: could not create table 'files'";
}

QSqlDatabase DatabaseManager::database() const
{
    return m_mainDB;
}

DatabaseManager::~DatabaseManager()
{
    m_mainDB.close();
}
