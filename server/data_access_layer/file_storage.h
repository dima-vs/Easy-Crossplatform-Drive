#ifndef FILE_STORAGE_H
#define FILE_STORAGE_H

#include <QtTypes>
#include <QByteArray>
#include <QString>

class FileStorage
{
private:
    QString& m_baseStoragePath;
public:
    FileStorage(QString& baseStoragePath);

    bool writeChunk(const QString& serverName, qint64 offset,
                    const QByteArray& data) const;
    QByteArray readChunk(const QString& serverName, qint64 offset, qint64 size) const;
    bool removeFile(const QString& serverName) const;
    qint64 getFileSize(const QString& serverName) const;
private:
    QString getSecurePath(const QString& serverName) const;
    void ensureStorageExists();
};

#endif // FILE_STORAGE_H
