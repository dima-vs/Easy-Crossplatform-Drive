#ifndef PASSWORD_HASHER_INTERFACE_H
#define PASSWORD_HASHER_INTERFACE_H

#include <QString>

namespace Service::Security
{

class IPasswordHasher
{
public:
    virtual ~IPasswordHasher() = default;

    virtual QString hashPassword(const QString& password) const = 0;
    virtual bool verifyPassword(const QString& password, const QString& hash) const = 0;
};

}

#endif // PASSWORD_HASHER_INTERFACE_H
