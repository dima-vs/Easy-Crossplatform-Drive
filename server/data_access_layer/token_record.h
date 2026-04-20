#ifndef TOKEN_RECORD_H
#define TOKEN_RECORD_H

#include <QString>
#include <QDateTime>

struct TokenRecord
{
private:
    QString m_id;
    QString m_tokenHash;
    int m_userId;
    QDateTime m_expiresAt;

    bool m_isValid;
public:
    TokenRecord();

    TokenRecord(const QString &id, const QString &hash, int userId, const QDateTime &expiresAt);

    QString id() const { return m_id; }
    QString tokenHash() const { return m_tokenHash; }
    int userId() const { return m_userId; }
    QDateTime expiresAt() const { return m_expiresAt; }

    bool isValid() const { return m_isValid; }
    bool isExpired(const QDateTime& current) const {
        return current > m_expiresAt;
    }
};

#endif // TOKEN_RECORD_H
