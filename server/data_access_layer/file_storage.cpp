#include <QDebug>
#include <QIODevice>
#include <QFile>
#include <QFileInfo>
#include <QDir>
#include "file_storage.h"

FileStorage::FileStorage(const QString &baseStoragePath):
    m_baseStoragePath(baseStoragePath)
{
    ensureStorageExists();
}

bool FileStorage::writeChunk(const QString& serverName, qint64 offset,
                const QByteArray& data) const
{
    QFile file(getSecurePath(serverName));

    if (!file.open(QIODevice::ReadWrite))
    {
        qCritical() << "cannot open file" << serverName;
        return false;
    }

    if (!file.seek(offset))
    {
        qCritical() << "cannot set position pointer at" <<
            offset << "in file" << serverName;
        return false;
    }

    if (file.write(data) == -1)
    {
        qCritical() << "cannot write to file" << serverName;
        return false;
    }

    file.close();
    return true;
}

QByteArray FileStorage::readChunk(const QString& serverName, qint64 offset, qint64 size) const
{
    QFile file(getSecurePath(serverName));

    if (!file.open(QIODevice::ReadOnly))
    {
        qCritical() << "cannot open file" << serverName;
        return QByteArray();
    }

    qint64 fileSize = file.size();

    if (offset >= fileSize)
    {
        qWarning() << "offset" << offset << "is beyond file size"
                   << fileSize << "of file" << serverName;
        return QByteArray();
    }

    if (!file.seek(offset))
    {
        qCritical() << "cannot set position pointer at" << offset
                    << "in file" << serverName;
        return QByteArray();
    }

    QByteArray chunkRead = file.read(size);
    if (chunkRead.size() == 0)
    {
        qCritical() << "cannot read a chunk of file" << serverName;
        return QByteArray();
    }

    file.close();
    return chunkRead;
}

bool FileStorage::removeFile(const QString& serverName) const
{
    QFile file(getSecurePath(serverName));
    if (!file.exists())
    {
        qWarning() << "file" << serverName << "doesn't exist";
        return false;
    }

    if (file.remove())
    {
        qInfo() << "file" << serverName << "removed";
        return true;
    }
    return false;

}

qint64 FileStorage::getFileSize(const QString& serverName) const
{
    QFileInfo fileInf(getSecurePath(serverName));
    if (!fileInf.exists())
    {
        qCritical() << "file" << serverName << "doesn't exist";
        return 0;
    }

    return fileInf.size();
}

QString FileStorage::getSecurePath(const QString& serverName) const
{
    QString cleanFileName = QFileInfo(serverName).fileName();
    return QDir(m_baseStoragePath).absoluteFilePath(cleanFileName);
}

void FileStorage::ensureStorageExists()
{
    QDir dir(m_baseStoragePath);
    if (!dir.exists())
    {
        if (dir.mkpath("."))
        {
            qInfo() << "storage directory created at" << m_baseStoragePath;
        } else {
            qCritical() << "could not create storage directory" << m_baseStoragePath;
        }
    }
}

bool FileStorage::exists(const QString& serverName) const
{
    return QFile::exists(getSecurePath(serverName));
}
