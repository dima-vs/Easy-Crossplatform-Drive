#ifndef USER_H
#define USER_H

#include <QString>

struct User
{
private:
    int m_id;
    QString m_username;
    QString m_email;
    QString m_passwordHash;
    bool m_isIDSet;
    bool m_isValid;
public:
    User();
    User(const QString &username, const QString &email, const QString &passwordHash);
    User(
      int id, const QString &username,
      const QString &email,
      const QString &passwordHash
    );

    bool isValid() const { return m_isValid; }
    int id() const { return m_id; }
    QString username() const { return m_username; }
    QString email() const { return m_email; }
    QString passwordHash() const { return m_passwordHash; }
    bool isIDSet() const { return m_isIDSet; }

    void setId(int id);
};

#endif // USER_H
