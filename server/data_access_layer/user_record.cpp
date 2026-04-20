#include "user_record.h"

UserRecord::UserRecord() : m_id(0), m_isIDSet(false), m_isValid(false) {}

UserRecord::UserRecord(const QString &username, const QString &email, const QString &passwordHash)
    : m_id(0),
    m_username(username),
    m_email(email),
    m_passwordHash(passwordHash),
    m_isIDSet(false),
    // User can be valid without id; e.g. when creating a new user,
    // the id can be omited, because it will be assigned automatically (AUTOINCREMENT)
    m_isValid(true)
    {}

UserRecord::UserRecord(
    int id,
    const QString &username,
    const QString &email,
    const QString &passwordHash
    )
    : m_id(id),
    m_username(username),
    m_email(email),
    m_passwordHash(passwordHash),
    m_isIDSet(true),
    m_isValid(true) {}

void UserRecord::setId(int id)
{
    m_id = id;
    m_isIDSet = true;
}
