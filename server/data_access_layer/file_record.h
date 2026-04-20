#ifndef FILE_RECORD_H
#define FILE_RECORD_H

#include <QString>
#include <QDateTime>
#include <QVariant>
#include <QMetaType>
#include <QtTypes>
#include "domain/file_type.h"

struct FileRecord
{
private:
    using FileType = Common::Domain::FileType;

    int m_id;
    int m_ownerId;
    FileType m_type;
    QString m_logicalName;
    QVariant m_serverName;
    qint64 m_size;
    QDateTime m_uploadTime;
    QVariant m_parentId;

    bool m_isIDSet;
    bool m_isValid;

public:
    FileRecord();

    FileRecord(
        int ownerId,
        FileType type,
        const QString &logicalName,
        QVariant serverName,
        qint64 size,
        QVariant parentId = QVariant(QMetaType::fromType<int>())
        );

    FileRecord(int id, int ownerId, FileType type, const QString &logicalName,
         QVariant serverName, qint64 size, QDateTime uploadTime, QVariant parentId);

    int id() const { return m_id; }
    int ownerId() const { return m_ownerId; }
    FileType type() const { return m_type; }
    QString logicalName() const { return m_logicalName; }
    QVariant serverName() const { return m_serverName; }
    qint64 size() const { return m_size; }
    QDateTime uploadTime() const { return m_uploadTime; }
    QVariant parentId() const { return m_parentId; }

    bool isValid() const { return m_isValid; }
    bool isIDSet() const { return m_isIDSet; }

    void setId(int id);
};

#endif // FILE_RECORD_H
