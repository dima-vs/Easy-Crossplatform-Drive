#include "token.h"

Token::Token() : m_userId(0), m_isValid(false) {}

Token::Token(const QString &id, const QString &hash,
             int userId, const QDateTime &expiresAt)
    : m_id(id), m_tokenHash(hash),
    m_userId(userId), m_expiresAt(expiresAt),
    m_isValid(true) {}
