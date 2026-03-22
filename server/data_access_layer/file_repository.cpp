#include <QSqlDatabase>
#include "file_repository.h"

FileRepository::FileRepository(DatabaseManager& db):
    m_db(db) {}

bool FileRepository::addNewFile(const File& file) const
{
    if (!file.isValid())
    {
        qCritical() << "[ERROR] FileRepository::addNewFile: could not add a new file. " <<
            "File is not valid";
        return false;
    }

    QSqlQuery query(m_db.database());

    query.prepare("INSERT INTO files (owner_id, type, logical_name, server_name, size, parent_id) "
                  "VALUES (:owner_id, :type, :logical_name, :server_name, :size, :parent_id)");

    query.bindValue(":owner_id", file.ownerId());
    query.bindValue(":type", file.type());
    query.bindValue(":logical_name", file.logicalName());

    query.bindValue(":server_name", file.serverName());
    query.bindValue(":size", file.size());
    query.bindValue(":parent_id", file.parentId());

    if (!query.exec())
    {
        qCritical() << "[ERROR] FileRepository::addNewFile: failed to insert file."
                    << "Database error:" << query.lastError().text();
        return false;
    }

    qDebug() << "[SUCCESS] FileRepository::addNewFile: added" << file.logicalName();
    return true;
}

File FileRepository::getFile(int id) const
{
    QSqlQuery query(m_db.database());
    query.prepare("SELECT id, owner_id, type, logical_name, server_name, size, upload_time, parent_id "
                  "FROM files WHERE id = :id");
    query.bindValue(":id", id);

    if (!query.exec())
    {
        qCritical() << "[ERROR] FileRepository::getFile(id): query failed."
                    << query.lastError().text();
        return File();
    }

    if (query.next())
    {
        int fileId = query.value("id").toInt();
        int ownerId = query.value("owner_id").toInt();
        QString type = query.value("type").toString();
        QString logicalName = query.value("logical_name").toString();
        QVariant serverName = query.value("server_name");
        long long size = query.value("size").toLongLong();
        QDateTime uploadTime = query.value("upload_time").toDateTime();
        QVariant parentId = query.value("parent_id");

        return File(
            fileId,
            ownerId,
            type,
            logicalName,
            serverName,
            size,
            uploadTime,
            parentId
            );
    }

    qDebug() << "[INFO] FileRepository::getFile(id): file with id" << id << "not found.";
    return File();
}

File FileRepository::getFile(int ownerId, const QList<QString> &fullPath) const
{
    int objId = 0;
    if (!getFileId(ownerId, fullPath, objId))
        return File();

    return getFile(objId);
}

bool FileRepository::deleteFile(int ownerId, const QList<QString>& fullPath) const
{
    int rootObjId = 0;
    if (!getFileId(ownerId, fullPath, rootObjId))
        return false;

    return deleteRecursive(rootObjId);
}

bool FileRepository::deleteRecursive(int objId) const
{
    QSqlDatabase db = m_db.database();

    if (!db.transaction())
    {
        qCritical() << "[ERROR] FileRepository::deleteRecursive:" <<
                       "could not start transaction";
        return false;
    }

    QList<QList<int>> hierarchyLevels;
    hierarchyLevels.append(QList<int> { objId });

    while (true)
    {
        QList<int> currentHierarchy = hierarchyLevels.last();
        QList<int> childrenHierarchy;

        for (int currentId : currentHierarchy)
        {
            if (!deleteFileChildren(currentId))
            {
                db.rollback();
                return false;
            }

            if (!saveChildrenDirectoriesId(currentId, childrenHierarchy))
            {
                db.rollback();
                return false;
            }
        }

        if (childrenHierarchy.isEmpty())
            break;

        hierarchyLevels.append(childrenHierarchy);
    }

    for (int i = hierarchyLevels.size() - 1; i >= 0; --i)
    {
        if (!deleteObjects(hierarchyLevels[i]))
        {
            db.rollback();
            return false;
        }
    }

    if (!db.commit())
    {
        qCritical() << "[ERROR] FileRepository::deleteRecursive:" <<
                       "could not commit transaction";
        db.rollback();
        return false;
    }

    return true;
}

bool FileRepository::deleteFileChildren(int parentId) const
{
    QSqlQuery query(m_db.database());
    query.prepare("DELETE FROM files WHERE parent_id = :parent_id AND type = 'file'");
    query.bindValue(":parent_id", parentId);
    if (!query.exec())
    {
        qCritical() << "[ERROR] FileRepository::deleteFileChildren: query failed." <<
            query.lastError().text();
        return false;
    }
    return true;
}

bool FileRepository::saveChildrenDirectoriesId(int parentId, QList<int>& directoriesId) const
{
    QSqlQuery query(m_db.database());
    query.prepare("SELECT id FROM files WHERE"
                  " parent_id = :parent_id AND type = 'directory'");
    query.bindValue(":parent_id", parentId);

    if (!query.exec())
    {
        qCritical() << "[ERROR] FileRepository::saveChildrenDirectoriesId: query failed." <<
            query.lastError().text();
        return false;
    }

    while (query.next())
    {
        int dirId = query.value(0).toInt();
        directoriesId.append(dirId);
    }
    return true;
}

bool FileRepository::deleteObjects(const QList<int>& idListToDelete) const
{
    for (int objId: idListToDelete)
    {
        QSqlQuery query(m_db.database());
        query.prepare("DELETE FROM files WHERE id = :id");
        query.bindValue(":id", objId);
        if (!query.exec())
        {
            qCritical() << "[ERROR] FileRepository::deleteObjects: query failed." <<
                query.lastError().text();
            return false;
        }
    }

    return true;
}

bool FileRepository::getFileId(int ownerId, const QList<QString>& fullPath, int& outId) const
{
    int currentParentId = 0;
    bool isRoot = true;

    for (const QString &objName : fullPath)
    {
        QSqlQuery query(m_db.database());

        if (isRoot) {
            query.prepare("SELECT id FROM files WHERE owner_id = :owner_id "
                          "AND parent_id IS NULL AND logical_name = :name");
        } else {
            query.prepare("SELECT id FROM files WHERE owner_id = :owner_id "
                          "AND parent_id = :parent_id AND logical_name = :name");
            query.bindValue(":parent_id", currentParentId);
        }

        query.bindValue(":owner_id", ownerId);
        query.bindValue(":name", objName);

        if (!query.exec())
        {
            qCritical() << "[ERROR] FileRepository::getFileId: path search failed." << query.lastError().text();
            return false;
        }

        if (query.next()) {
            currentParentId = query.value(0).toInt();
            isRoot = false;
        } else {
            // if one of the path segments not found, file doesn't exist
            qDebug() << "[INFO] FileRepository::getFileId: segment" <<
                objName << "not found";
            return false;
        }
    }

    outId = currentParentId;
    return true;
}
