#include <QDebug>
#include <QIODevice>
#include <QFile>
#include <QFileInfo>
#include <QDir>
#include "file_storage.h"

FileStorage::FileStorage(QString &baseStoragePath):
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
        qCritical() << "[ERROR] FileStorage::writeChunk: cannot open file" <<
            serverName;
        return false;
    }

    if (!file.seek(offset))
    {
        qCritical() << "[ERROR] FileStorage::writeChunk: cannot set" <<
            "position pointer at" << offset << ". File name:" << serverName;
        return false;
    }

    if (file.write(data) == -1)
    {
        qCritical() << "[ERROR] FileStorage::writeChunk: cannot write to file" <<
            serverName;
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
        qCritical() << "[ERROR] FileStorage::readChunk: cannot open file" <<
            serverName;
        return QByteArray();
    }

    qint64 fileSize = file.size();

    if (offset >= fileSize)
    {
        qDebug() << "[INFO] FileStorage::readChunk: offset" << offset
                 << "is beyond file size" << fileSize << ". File name:" << serverName;
        return QByteArray();
    }

    if (!file.seek(offset))
    {
        qCritical() << "[ERROR] FileStorage::readChunk: cannot set" <<
            "position pointer at" << offset << ". File name:" << serverName;
        return QByteArray();
    }

    QByteArray chunkRead = file.read(size);
    if (chunkRead.size() == 0)
    {
        qCritical() << "[ERROR] FileStorage::readChunk: cannot read a chunk of file" <<
            serverName;
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
        qCritical() << "[ERROR] FileStorage::getFileSize: file" <<
            serverName << "doesn't exist";
        return false;
    }

    if (file.remove())
    {
        qDebug() << "[SUCCESS] FileStorage::removeFile: file" <<
            serverName << "removed";
        return true;
    }
    return false;

}

qint64 FileStorage::getFileSize(const QString& serverName) const
{
    QFileInfo fileInf(getSecurePath(serverName));
    if (!fileInf.exists())
    {
        qCritical() << "[ERROR] FileStorage::getFileSize: file" <<
            serverName << "doesn't exist";
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
            qDebug() << "[INFO] FileStorage::getSecurePath: storage directory created at" << m_baseStoragePath;
        } else {
            qCritical() << "[ERROR] FileStorage::getSecurePath: could not create storage directory!";
        }
    }
}
