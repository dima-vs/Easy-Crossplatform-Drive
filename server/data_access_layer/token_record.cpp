#include "token_record.h"

TokenRecord::TokenRecord() : m_userId(0), m_isValid(false) {}

TokenRecord::TokenRecord(const QString &id, const QString &hash,
             int userId, const QDateTime &expiresAt)
    : m_id(id), m_tokenHash(hash),
    m_userId(userId), m_expiresAt(expiresAt),
    m_isValid(true) {}
