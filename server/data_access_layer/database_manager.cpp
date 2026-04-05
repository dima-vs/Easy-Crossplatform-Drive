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
        qCritical() << "could not open database 'main_database.db'";

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
    createTokensTable();
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
    {
        qCritical() << "could not create table 'users': "
                    << query.lastError().text();
    } else {
        qInfo() << "table 'users' is ready";
    }
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
               "FOREIGN KEY(parent_id) REFERENCES files(id))");

    if (!ok)
    {
        qCritical() << "could not create table 'files': "
                    << query.lastError().text();
    } else {
        qInfo() << "table 'files' is ready";
    }

    createUniqueRootIndexOnFiles();
}

void DatabaseManager::createUniqueRootIndexOnFiles() const
{
    if (!createChildUniqueIndex() || !createRootUniqueIndex()) {
        qCritical() << "failed to initialize unique indexes for 'files'";
    } else {
        qInfo() << "unique indexes for 'files' initialized";
    }
}

bool DatabaseManager::createChildUniqueIndex() const
{
    QSqlQuery query(m_mainDB);
    bool ok = query.exec("CREATE UNIQUE INDEX IF NOT EXISTS idx_files_unique_child "
                         "ON files(owner_id, parent_id, logical_name) "
                         "WHERE parent_id IS NOT NULL");

    if (!ok) {
        qCritical() << "could not create index 'idx_files_unique_child':" <<
            query.lastError().text();
    }

    return ok;
}

bool DatabaseManager::createRootUniqueIndex() const
{
    QSqlQuery query(m_mainDB);
    bool ok = query.exec("CREATE UNIQUE INDEX IF NOT EXISTS idx_files_unique_root "
                         "ON files(owner_id, logical_name) "
                         "WHERE parent_id IS NULL");

    if (!ok) {
        qCritical() << "could not create index 'idx_files_unique_root':" <<
            query.lastError().text();
    }

    return ok;
}

void DatabaseManager::createTokensTable() const
{
    QSqlQuery query(m_mainDB);
    bool ok = query.exec("CREATE TABLE IF NOT EXISTS tokens("
                         "id TEXT PRIMARY KEY,"
                         "token_hash TEXT NOT NULL,"
                         "user_id INTEGER NOT NULL,"
                         "expires_at DATETIME,"
                         "FOREIGN KEY(user_id) REFERENCES users(id))");

    if (!ok) {
        qCritical() << "could not create table 'tokens':" << query.lastError().text();
    } else {
        qInfo() << "table 'tokens' is ready";
    }
}

QSqlDatabase DatabaseManager::database() const
{
    return m_mainDB;
}

DatabaseManager::~DatabaseManager()
{
    m_mainDB.close();
}
