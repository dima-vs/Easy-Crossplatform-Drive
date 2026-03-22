#ifndef TOKEN_H
#define TOKEN_H

#include <QString>
#include <QDateTime>

struct Token
{
private:
    QString m_id;
    QString m_tokenHash;
    int m_userId;
    QDateTime m_expiresAt;

    bool m_isValid;
public:
    Token();

    Token(const QString &id, const QString &hash, int userId, const QDateTime &expiresAt);

    QString id() const { return m_id; }
    QString tokenHash() const { return m_tokenHash; }
    int userId() const { return m_userId; }
    QDateTime expiresAt() const { return m_expiresAt; }

    bool isValid() const { return m_isValid; }
    bool isExpired() const { return QDateTime::currentDateTime() > m_expiresAt; }
};

#endif // TOKEN_H
