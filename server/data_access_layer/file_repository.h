#ifndef FILE_REPOSITORY_H
#define FILE_REPOSITORY_H

#include <QString>
#include <QList>
#include <QVariant>
#include <QMetaType>
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

    bool deleteFile(int ownerId, int objId) const;

    bool deleteFile(int ownerId,
                    int objId,
                    QList<QString>& physicalFilesToDeleteOut,
                    int* outObjectsDeleted = nullptr
                    ) const;

    bool deleteFile(int ownerId, const QList<QString>& fullPath) const;
    // delete any object
    bool deleteFile(int ownerId,
                    const QList<QString>& fullPath,
                    QList<QString>& physicalFilesToDeleteOut,
                    int* outObjectsDeleted = nullptr
                    ) const;

    // Returns all nested files and directories, including the root.
    // Directories are ordered from root to deepest.
    // Use reverse order for deletion.
    bool getAllNestedObjects(
        int ownerId,
        const QList<QString>& fullPath,
        QPair<QList<File>, QList<File>>& outFilesAndDirs,
        QVariant maxDepth = QVariant(QMetaType::fromType<int>())
        ) const;

    bool getAllNestedObjects(
        int ownerId,
        QVariant parentId,
        QPair<QList<File>, QList<File>>& outFilesAndDirs,
        QVariant maxDepth = QVariant(QMetaType::fromType<int>())
        ) const;
private:
    File extractFileFromQuery(const QSqlQuery& query) const;

    bool processBFSQueue(
        QQueue<QPair<int, int>>& dirsToProcess,
        QPair<QList<File>, QList<File>>& outFilesAndDirs,
        int maxDepth
        ) const;

    // If maxDepth==-1, depth is unlimited.
    // Depth includes root object, so if depth == 1,
    // only root object will be returned
    bool getAllObjectsRecursive(
        int ownerId,
        int id,
        QPair<QList<File>, QList<File>>& outFilesAndDirs,
        int maxDepth = -1
        ) const;


    bool getFileId(int ownerId, const QList<QString>& fullPath, int& outId) const;
    bool deleteObjects(const QList<int> &idListToDelete) const;
};

#endif // FILE_REPOSITORY_H
