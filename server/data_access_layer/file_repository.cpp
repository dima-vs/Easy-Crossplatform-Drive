#include <QSqlDatabase>
#include <QQueue>
#include "file_repository.h"

FileRepository::FileRepository(DatabaseManager& db):
    m_db(db) {}

bool FileRepository::addNewFile(File &file) const
{
    if (!file.isValid())
    {
        qCritical() << "could not add a new file:" <<
            "'file' parameter is not valid";
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

    bool qResult = query.exec();

    if (qResult)
    {
        // try to update source object's id
        QVariant id = query.lastInsertId();
        if (id.isValid()) {
            qDebug() << "inserted ID:" << id.toInt();
            file.setId(id.toInt());
        } else {
            qDebug() << "lastInsertId is not supported by this driver";
        }

        qInfo().noquote()
            << file.type()
            << "\"" + file.logicalName() + "\""
            << "added";
    }
    else
    {
        qCritical() << "failed to insert file:"
                    << query.lastError().text();
    }

    return qResult;
}

bool FileRepository::checkPermission(int ownerId, const File& file) const
{
    return file.ownerId() == ownerId;
}

File FileRepository::getFile(int id) const
{
    QSqlQuery query(m_db.database());
    query.prepare("SELECT id, owner_id, type, logical_name, server_name, size, upload_time, parent_id "
                  "FROM files WHERE id = :id");
    query.bindValue(":id", id);

    if (!query.exec())
    {
        qCritical() << query.lastError().text();
        return File();
    }

    if (query.next())
    {
        return extractFileFromQuery(query);
    }

    qWarning() << "file object with id" << id << "not found";
    return File();
}

File FileRepository::getFile(int ownerId, const QList<QString> &fullPath) const
{
    int objId = 0;
    if (!getFileId(ownerId, fullPath, objId))
        return File();

    return getFile(objId);
}

File FileRepository::getFile(int ownerId, QVariant parentId, const QString fileName) const
{
    QSqlQuery query(m_db.database());

    if (parentId.isNull())
    {
        query.prepare("SELECT id, owner_id, type, logical_name, "
                      "server_name, size, upload_time, parent_id "
                      "FROM files WHERE owner_id = :owner_id AND "
                      "logical_name = :logical_name AND "
                      "parent_id IS NULL");
    }
    else
    {
        query.prepare("SELECT id, owner_id, type, logical_name, "
                      "server_name, size, upload_time, parent_id "
                      "FROM files WHERE owner_id = :owner_id AND "
                      "logical_name = :logical_name AND "
                      "parent_id IS :parent_id");
        query.bindValue(":parent_id", parentId);
    }

    query.bindValue(":owner_id", ownerId);
    query.bindValue(":logical_name", fileName);

    if (!query.exec())
    {
        qCritical() << query.lastError().text();
        return File();
    }

    if (query.next())
    {
        return extractFileFromQuery(query);
    }

    qWarning() << "file object not found";
    return File();
}

File FileRepository::extractFileFromQuery(const QSqlQuery& query) const
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

bool FileRepository::getAllNestedObjects(
    int ownerId,
    const QList<QString>& fullPath,
    QPair<QList<File>, QList<File>>& outFilesAndDirs,
    QVariant maxDepth
    ) const
{
    int maxDepthNum = maxDepth.isNull() ? -1 : maxDepth.toInt();
    int rootObjId = 0;
    if (!getFileId(ownerId, fullPath, rootObjId))
        return false;

    return getAllObjectsRecursive(
        ownerId,
        rootObjId,
        outFilesAndDirs,
        maxDepthNum
        );
}

bool FileRepository::getAllNestedObjects(
    int ownerId,
    QVariant parentId,
    QPair<QList<File>, QList<File>>& outFilesAndDirs,
    QVariant maxDepth
    ) const
{
    int maxDepthNum = maxDepth.isNull() ? -1 : maxDepth.toInt();

    // === Concrete object ===
    if (!parentId.isNull())
    {
        return getAllObjectsRecursive(
            ownerId,
            parentId.toInt(),
            outFilesAndDirs,
            maxDepthNum);
    }

    // === Root ===
    if (maxDepthNum <= 0 && maxDepthNum != -1)
        return false;

    QQueue<QPair<int, int>> dirsToProcess;
    QSqlQuery query(m_db.database());
    query.prepare("SELECT id, owner_id, type, logical_name, server_name, "
                  "size, upload_time, parent_id FROM files "
                  "WHERE owner_id = :owner_id AND parent_id IS NULL");
    query.bindValue(":owner_id", ownerId);

    if (!query.exec())
        return false;

    while (query.next())
    {
        File child = extractFileFromQuery(query);
        if (child.type() == "file")
        {
            // add to file list
            outFilesAndDirs.first.append(child);
        }
        else
        {
            // add to dir list
            outFilesAndDirs.second.append(child);
            dirsToProcess.enqueue({child.id(), 1});
        }
    }

    return processBFSQueue(dirsToProcess, outFilesAndDirs, maxDepthNum);
}

bool FileRepository::getAllObjectsRecursive(
    int ownerId,
    int rootId,
    QPair<QList<File>, QList<File>>& outFilesAndDirs,
    int maxDepth
    ) const
{
    if (maxDepth <= 0 && maxDepth != -1)
        return false;

    File rootObj = getFile(rootId);
    if (!rootObj.isValid())
        return false;

    if (rootObj.ownerId() != ownerId)
    {
        qWarning() << "security: user" << ownerId
                   << "tried to access file" << rootId;
        return false;
    }

    if (rootObj.type() == "file")
    {
        outFilesAndDirs.first.append(rootObj);
        return true;
    }

    outFilesAndDirs.second.append(rootObj);

    // <dirId, dirDepth>
    QQueue<QPair<int, int>> dirsToProcess;
    dirsToProcess.enqueue({ rootId, 1 });

    return processBFSQueue(dirsToProcess, outFilesAndDirs, maxDepth);
}


bool FileRepository::processBFSQueue(
    QQueue<QPair<int, int>>& dirsToProcess, // <dirId, dirDepth>
    QPair<QList<File>, QList<File>>& outFilesAndDirs,
    int maxDepth
    ) const
{
    while (!dirsToProcess.isEmpty())
    {
        auto currentItem = dirsToProcess.dequeue();
        int currentParentId = currentItem.first;
        int currentDepth = currentItem.second;

        if ((maxDepth > 0) && (currentDepth >= maxDepth))
        {
            continue;
        }

        QSqlQuery query(m_db.database());
        query.prepare("SELECT id, owner_id, type, logical_name, server_name, "
                      "size, upload_time, parent_id FROM files "
                      "WHERE parent_id = :parent_id");
        query.bindValue(":parent_id", currentParentId);

        if (!query.exec())
        {
            qCritical() << query.lastError().text();
            return false;
        }

        while (query.next())
        {
            File child = extractFileFromQuery(query);
            if (child.type() == "file")
            {
                // add to file list
                outFilesAndDirs.first.append(child);
            }
            else
            {
                // add to dir list
                outFilesAndDirs.second.append(child);
                // children of this dir have depth=currentDepth+1
                dirsToProcess.enqueue({child.id(), currentDepth + 1});
            }
        }
    }
    return true;
}

bool FileRepository::deleteFile(int ownerId, int objId) const
{
    QList<QString> notUsed;
    return deleteFile(ownerId, objId, notUsed);
}

bool FileRepository::deleteFile(
    int ownerId,
    int objId,
    QList<QString>& physicalFilesToDeleteOut,
    int* outObjectsDeleted
    ) const
{
    QPair<QList<File>, QList<File>> filesAndDirs;

    if (!getAllNestedObjects(ownerId, QVariant(objId), filesAndDirs))
        return false;

    QSqlDatabase db = m_db.database();

    if (!db.transaction())
    {
        qCritical() << "could not start transaction:" << db.lastError().text();
        return false;
    }

    // collect file IDs and server names to delete
    QList<int> fileIdsToDelete;
    for (const File& file : filesAndDirs.first)
    {
        fileIdsToDelete.append(file.id());
        physicalFilesToDeleteOut.append(file.serverName().toString());
    }

    // delete all the files
    if (!fileIdsToDelete.isEmpty() && !deleteObjects(fileIdsToDelete))
    {
        db.rollback();
        return false;
    }

    // collect directory IDs in reverse order to delete
    QList<int> dirIdsToDelete;
    for (int i = filesAndDirs.second.size() - 1; i >= 0; --i)
    {
        dirIdsToDelete.append(filesAndDirs.second[i].id());
    }

    // delete all the directories
    if (!dirIdsToDelete.isEmpty() && !deleteObjects(dirIdsToDelete))
    {
        db.rollback();
        return false;
    }

    if (!db.commit())
    {
        qCritical() << "could not commit transaction:" << db.lastError().text();
        return false;
    }

    // set count of deleted objects
    if (outObjectsDeleted != nullptr)
    {
        *outObjectsDeleted = filesAndDirs.first.size() +
                             filesAndDirs.second.size();
    }

    qInfo() << "file object with id" << objId << "deleted";
    return true;
}


bool FileRepository::deleteFile(int ownerId, const QList<QString>& fullPath) const
{
    QList<QString> notUsed;
    return deleteFile(ownerId, fullPath, notUsed);
}


bool FileRepository::deleteFile(
    int ownerId,
    const QList<QString>& fullPath,
    QList<QString>& physicalFilesToDeleteOut,
    int* outObjectsDeleted
    ) const
{
    int objId = 0;
    if (!getFileId(ownerId, fullPath, objId))
        return false;

    return deleteFile(ownerId, objId, physicalFilesToDeleteOut, outObjectsDeleted);
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
            qCritical() << query.lastError().text();
            return false;
        }
    }

    return true;
}

bool FileRepository::getFileId(int ownerId, const QList<QString>& fullPath, int& outId) const
{
    if (fullPath.isEmpty())
    {
        qWarning() << "cannot get file: path is empty";
        return false;
    }

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
            qCritical() << "path search query failed:" << query.lastError().text();
            return false;
        }

        if (query.next()) {
            currentParentId = query.value(0).toInt();
            isRoot = false;
        }
        else
        {
            // if one of the path segments not found, file doesn't exist
            qWarning() << "segment" << objName << "not found";
            return false;
        }
    }

    outId = currentParentId;
    return true;
}

QList<File> FileRepository::getFilesByOwner(int ownerId) const
{
    QSqlQuery query(m_db.database());
    query.prepare("SELECT id, owner_id, type, logical_name, server_name, "
                  "size, upload_time, parent_id FROM files WHERE owner_id = :owner_id");
    query.bindValue(":owner_id", ownerId);

    if (!query.exec())
    {
        qCritical() << query.lastError().text();
        return QList<File>();
    }

    QList<File> result;
    while (query.next())
    {
        result.append(extractFileFromQuery(query));
    }

    return result;
}
