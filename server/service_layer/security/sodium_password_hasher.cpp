#include <QDebug>
#include <QByteArray>
#include <sodium.h>
#include "sodium_password_hasher.h"

namespace Service::Security
{

SodiumPasswordHasher::SodiumPasswordHasher(const Config::Security::PasswordHashing &config):
    m_config(config)
{
    if (sodium_init() < 0)
    {
        qFatal("Sodium initialization failed! The server cannot start.");
    }
}

QString SodiumPasswordHasher::hashPassword(const QString& password) const
{
    char out_hash[crypto_pwhash_STRBYTES] = {0};
    QByteArray pwBytes = password.toUtf8();

    if (crypto_pwhash_str(
            out_hash,
            pwBytes.constData(),
            pwBytes.size(),
            m_config.argon2.opsLimit,
            m_config.argon2.memLimit
            ) != 0)
    {
        qCritical() << "sodium: out of memory or internal error";
        return QString();
    }

    return QString::fromUtf8(out_hash);
}

bool SodiumPasswordHasher::verifyPassword(const QString& password, const QString& passwordHash) const
{
    if (passwordHash.isEmpty()) return false;

    QByteArray pwBytes = password.toUtf8();
    // QByteArray garantees zero-terminated string
    QByteArray hashBytes = passwordHash.toLatin1();

    int result = crypto_pwhash_str_verify(
        hashBytes.constData(),
        pwBytes.constData(),
        pwBytes.size()
        );

    return result == 0;
}

}
