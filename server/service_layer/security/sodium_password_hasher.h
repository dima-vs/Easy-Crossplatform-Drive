#ifndef SODIUM_PASSWORD_HASHER_H
#define SODIUM_PASSWORD_HASHER_H

#include <QString>
#include "security/password_hasher_interface.h"
#include "security/security_config.h"

namespace Service::Security
{

class SodiumPasswordHasher : public IPasswordHasher
{
private:
    Config::Security::PasswordHashing m_config;
public:
    SodiumPasswordHasher(const Config::Security::PasswordHashing& config);

    QString hashPassword(const QString& password) const override;
    bool verifyPassword(const QString& password, const QString& passwordHash) const override;
};

}

#endif // SODIUM_PASSWORD_HASHER_H
