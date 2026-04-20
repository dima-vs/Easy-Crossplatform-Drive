#include <QSqlDatabase>
#include <QQueue>
#include "file_repository.h"

FileRepository::FileRepository(DatabaseManager& db):
    m_db(db) {}

bool FileRepository::add(FileRecord &record) const
{
    if (!record.isValid())
    {
        qCritical() << "could not add a new file:" <<
            "'file' parameter is not valid";
        return false;
    }

    QSqlQuery query(m_db.database());

    query.prepare("INSERT INTO files (owner_id, type, logical_name, server_name, size, parent_id) "
                  "VALUES (:owner_id, :type, :logical_name, :server_name, :size, :parent_id)");

    query.bindValue(":owner_id", record.ownerId());
    query.bindValue(":type", FileTypeConverter::toString(record.type()));
    query.bindValue(":logical_name", record.logicalName());

    query.bindValue(":server_name", record.serverName());
    query.bindValue(":size", record.size());
    query.bindValue(":parent_id", record.parentId());

    bool qResult = query.exec();

    if (qResult)
    {
        // try to update source object's id
        QVariant id = query.lastInsertId();
        if (id.isValid()) {
            qDebug() << "inserted ID:" << id.toInt();
            record.setId(id.toInt());
        } else {
            qDebug() << "lastInsertId is not supported by this driver";
        }

        qInfo().noquote()
            << FileTypeConverter::toString(record.type())
            << "\"" + record.logicalName() + "\""
            << "added";
    }
    else
    {
        qCritical() << "failed to insert file:"
                    << query.lastError().text();
    }

    return qResult;
}

bool FileRepository::checkPermission(int ownerId, const FileRecord& fileRecord) const
{
    return fileRecord.ownerId() == ownerId;
}

FileRecord FileRepository::findById(int id) const
{
    QSqlQuery query(m_db.database());
    query.prepare("SELECT id, owner_id, type, logical_name, server_name, size, upload_time, parent_id "
                  "FROM files WHERE id = :id");
    query.bindValue(":id", id);

    if (!query.exec())
    {
        qCritical() << query.lastError().text();
        return FileRecord();
    }

    if (query.next())
    {
        return extractRecord(query);
    }

    qWarning() << "file object with id" << id << "not found";
    return FileRecord();
}

FileRecord FileRepository::findByPath(int ownerId, const QList<QString> &fullPath) const
{
    int objId = 0;
    if (!findRecordIdByPath(ownerId, fullPath, objId))
        return FileRecord();

    return findById(objId);
}

FileRecord FileRepository::findByName(
    int ownerId,
    QVariant parentId,
    const QString recordName
    ) const
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
    query.bindValue(":logical_name", recordName);

    if (!query.exec())
    {
        qCritical() << query.lastError().text();
        return FileRecord();
    }

    if (query.next())
    {
        return extractRecord(query);
    }

    qWarning() << "file object not found";
    return FileRecord();
}

FileRecord FileRepository::extractRecord(const QSqlQuery& query) const
{
    int fileId = query.value("id").toInt();
    int ownerId = query.value("owner_id").toInt();
    FileType type = FileTypeConverter::fromString(
        query.value("type").toString());
    QString logicalName = query.value("logical_name").toString();
    QVariant serverName = query.value("server_name");
    long long size = query.value("size").toLongLong();
    QDateTime uploadTime = query.value("upload_time").toDateTime();
    QVariant parentId = query.value("parent_id");

    return FileRecord(
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

bool FileRepository::getAllNested(
    int ownerId,
    const QList<QString>& fullPath,
    QPair<QList<FileRecord>, QList<FileRecord>>& outFilesAndDirs,
    QVariant maxDepth
    ) const
{
    int maxDepthNum = maxDepth.isNull() ? -1 : maxDepth.toInt();
    int rootObjId = 0;
    if (!findRecordIdByPath(ownerId, fullPath, rootObjId))
        return false;

    return getAllRecordsRecursive(
        ownerId,
        rootObjId,
        outFilesAndDirs,
        maxDepthNum
        );
}

bool FileRepository::getAllNested(
    int ownerId,
    QVariant parentId,
    QPair<QList<FileRecord>, QList<FileRecord>>& outFilesAndDirs,
    QVariant maxDepth
    ) const
{
    int maxDepthNum = maxDepth.isNull() ? -1 : maxDepth.toInt();

    // === Concrete object ===
    if (!parentId.isNull())
    {
        return getAllRecordsRecursive(
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
        FileRecord child = extractRecord(query);
        if (child.type() == FileType::File)
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

bool FileRepository::getAllRecordsRecursive(
    int ownerId,
    int rootId,
    QPair<QList<FileRecord>, QList<FileRecord>>& outFilesAndDirs,
    int maxDepth
    ) const
{
    if (maxDepth <= 0 && maxDepth != -1)
        return false;

    FileRecord rootObj = findById(rootId);
    if (!rootObj.isValid())
        return false;

    if (rootObj.ownerId() != ownerId)
    {
        qWarning() << "security: user" << ownerId
                   << "tried to access file" << rootId;
        return false;
    }

    if (rootObj.type() == FileType::File)
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
    QPair<QList<FileRecord>, QList<FileRecord>>& outFilesAndDirs,
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
            FileRecord child = extractRecord(query);
            if (child.type() == FileType::File)
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

bool FileRepository::remove(int ownerId, int objId) const
{
    QList<QString> notUsed;
    return remove(ownerId, objId, notUsed);
}

bool FileRepository::remove(
    int ownerId,
    int objId,
    QList<QString>& physicalFilesToDeleteOut,
    int* outObjectsDeleted
    ) const
{
    QPair<QList<FileRecord>, QList<FileRecord>> filesAndDirs;

    if (!getAllNested(ownerId, QVariant(objId), filesAndDirs))
        return false;

    QSqlDatabase db = m_db.database();

    if (!db.transaction())
    {
        qCritical() << "could not start transaction:" << db.lastError().text();
        return false;
    }

    // collect file IDs and server names to delete
    QList<int> fileIdsToDelete;
    for (const FileRecord& file : filesAndDirs.first)
    {
        fileIdsToDelete.append(file.id());
        physicalFilesToDeleteOut.append(file.serverName().toString());
    }

    // delete all the files
    if (!fileIdsToDelete.isEmpty() && !removeRecords(fileIdsToDelete))
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
    if (!dirIdsToDelete.isEmpty() && !removeRecords(dirIdsToDelete))
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


bool FileRepository::remove(int ownerId, const QList<QString>& fullPath) const
{
    QList<QString> notUsed;
    return remove(ownerId, fullPath, notUsed);
}


bool FileRepository::remove(
    int ownerId,
    const QList<QString>& fullPath,
    QList<QString>& physicalFilesToDeleteOut,
    int* outObjectsDeleted
    ) const
{
    int objId = 0;
    if (!findRecordIdByPath(ownerId, fullPath, objId))
        return false;

    return remove(ownerId, objId, physicalFilesToDeleteOut, outObjectsDeleted);
}

bool FileRepository::removeRecords(const QList<int>& idListToDelete) const
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

bool FileRepository::findRecordIdByPath(int ownerId, const QList<QString>& fullPath, int& outId) const
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

QList<FileRecord> FileRepository::findByOwner(int ownerId) const
{
    QSqlQuery query(m_db.database());
    query.prepare("SELECT id, owner_id, type, logical_name, server_name, "
                  "size, upload_time, parent_id FROM files WHERE owner_id = :owner_id");
    query.bindValue(":owner_id", ownerId);

    if (!query.exec())
    {
        qCritical() << query.lastError().text();
        return QList<FileRecord>();
    }

    QList<FileRecord> result;
    while (query.next())
    {
        result.append(extractRecord(query));
    }

    return result;
}
