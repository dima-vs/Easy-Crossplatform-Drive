#ifndef FILE_REPOSITORY_H
#define FILE_REPOSITORY_H

#include <QString>
#include <QList>
#include <QVariant>
#include <QMetaType>
#include <QPair>
#include <memory>
#include "domain/file_type.h"
#include "converter/file_type_converter.h"
#include "database_manager.h"
#include "file_record.h"

class FileRepository
{
private:
    using FileType = Common::Domain::FileType;
    using FileTypeConverter = Common::Converter::FileTypeConverter;

    DatabaseManager& m_db;
public:
    FileRepository(DatabaseManager& db);
    bool add(FileRecord& record) const;
    bool checkPermission(int ownerId, const FileRecord& fileRecord) const;
    FileRecord findById(int id) const;
    FileRecord findByName(
        int ownerId,
        QVariant parentId,
        const QString recordName
        ) const;
    FileRecord findByPath(int ownerId, const QList<QString>& fullPath) const;
    QList<FileRecord> findByOwner(int ownerId) const;

    bool remove(int ownerId, int objId) const;

    bool remove(int ownerId,
                    int objId,
                    QList<QString>& physicalFilesToDeleteOut,
                    int* outObjectsDeleted = nullptr
                    ) const;

    bool remove(int ownerId, const QList<QString>& fullPath) const;
    // delete any object
    bool remove(int ownerId,
                    const QList<QString>& fullPath,
                    QList<QString>& physicalFilesToDeleteOut,
                    int* outObjectsDeleted = nullptr
                    ) const;

    // Returns all nested files and directories, including the root.
    // Directories are ordered from root to deepest.
    // Use reverse order for deletion.
    bool getAllNested(
        int ownerId,
        const QList<QString>& fullPath,
        QPair<QList<FileRecord>, QList<FileRecord>>& outFilesAndDirs,
        QVariant maxDepth = QVariant(QMetaType::fromType<int>())
        ) const;

    bool getAllNested(
        int ownerId,
        QVariant parentId,
        QPair<QList<FileRecord>, QList<FileRecord>>& outFilesAndDirs,
        QVariant maxDepth = QVariant(QMetaType::fromType<int>())
        ) const;
private:
    FileRecord extractRecord(const QSqlQuery& query) const;

    bool processBFSQueue(
        QQueue<QPair<int, int>>& dirsToProcess,
        QPair<QList<FileRecord>, QList<FileRecord>>& outFilesAndDirs,
        int maxDepth
        ) const;

    // If maxDepth==-1, depth is unlimited.
    // Depth includes root object, so if depth == 1,
    // only root object will be returned
    bool getAllRecordsRecursive(
        int ownerId,
        int id,
        QPair<QList<FileRecord>, QList<FileRecord>>& outFilesAndDirs,
        int maxDepth = -1
        ) const;

    bool findRecordIdByPath(
        int ownerId,
        const QList<QString>& fullPath,
        int& outId
        ) const;

    bool removeRecords(const QList<int> &idListToDelete) const;
};

#endif // FILE_REPOSITORY_H
