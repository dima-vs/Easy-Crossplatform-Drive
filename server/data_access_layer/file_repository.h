#ifndef FILE_REPOSITORY_H
#define FILE_REPOSITORY_H

#include <QString>
#include <QList>
#include <QVariant>
#include <QPair>
#include <memory>
#include "database_manager.h"
#include "file.h"

class FileRepository
{
private:
    DatabaseManager& m_db;
public:
    FileRepository(DatabaseManager& db);
    bool addNewFile(File& file) const;
    File getFile(int id) const;
    File getFile(int ownerId, QVariant parentId, const QString fileName) const;
    File getFile(int ownerId, const QList<QString>& fullPath) const;
    QList<File> getFilesByOwner(int ownerId) const;

    bool deleteFile(int ownerId, const QList<QString>& fullPath) const;
    // delete any object
    bool deleteFile(int ownerId,
                    const QList<QString>& fullPath,
                    QList<QString>& physicalFilesToDeleteOut,
                    int* outObjectsDeleted = nullptr
                    ) const;

    bool getAllNestedObjects(
        int ownerId,
        const QList<QString>& fullPath,
        QPair<QList<File>, QList<File>>& outFilesAndDirs
        ) const;
private:
    File extractFileFromQuery(const QSqlQuery& query) const;
    bool getAllObjectsRecursive(
        int id,
        QPair<QList<File>, QList<File>>& outFilesAndDirs
        ) const;

    bool getFileId(int ownerId, const QList<QString>& fullPath, int& outId) const;
    bool deleteObjects(const QList<int> &idListToDelete) const;
};

#endif // FILE_REPOSITORY_H
