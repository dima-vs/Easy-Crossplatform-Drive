#ifndef FILE_REPOSITORY_H
#define FILE_REPOSITORY_H

#include <QString>
#include <QList>
#include "database_manager.h"
#include "file.h"

class FileRepository
{
private:
    DatabaseManager& m_db;
public:
    FileRepository(DatabaseManager& db);
    bool addNewFile(const File& file) const;
    File getFile(int id) const;
    File getFile(int ownerId, const QList<QString>& fullPath) const;
    //QList<File> getFilesByOwner(int ownerId);
    // delete any object
    bool deleteFile(int ownerId, const QList<QString>& fullPath) const;
private:
    bool getFileId(int ownerId, const QList<QString>& fullPath, int& outId) const;
    bool deleteRecursive(int objId) const;
    bool deleteObjects(const QList<int> &idListToDelete) const;
    // delete child objects with type='file'
    bool deleteFileChildren(int parentId) const;
    // save child objects with type='directory'
    bool saveChildrenDirectoriesId(int parentId, QList<int>& directoriesId) const;
};

#endif // FILE_REPOSITORY_H
