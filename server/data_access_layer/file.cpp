#include "file.h"

File::File()
    : m_id(0), m_ownerId(0), m_size(0), m_isIDSet(false), m_isValid(false)
{}

File::File(int ownerId, const QString &type, const QString &logicalName,
           QVariant serverName, qint64 size, QVariant parentId)
    : m_id(0),
    m_ownerId(ownerId),
    m_type(type),
    m_logicalName(logicalName),
    m_serverName(serverName),
    m_size(size),
    m_parentId(parentId),
    m_isIDSet(false),
    m_isValid(true)
{
    m_uploadTime = QDateTime::currentDateTime();
}

File::File(int id, int ownerId, const QString &type, const QString &logicalName,
           QVariant serverName, qint64 size, QDateTime uploadTime, QVariant parentId)
    : m_id(id),
    m_ownerId(ownerId),
    m_type(type),
    m_logicalName(logicalName),
    m_serverName(serverName),
    m_size(size),
    m_uploadTime(uploadTime),
    m_parentId(parentId),
    m_isIDSet(true),
    m_isValid(true)
{}

void File::setId(int id)
{
    m_id = id;
    m_isIDSet = true;
}
