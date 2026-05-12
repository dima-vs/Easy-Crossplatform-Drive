#ifndef SECURITY_CONFIG_H
#define SECURITY_CONFIG_H

#include <sodium.h>
#include <QtTypes>

namespace Config::Security
{

struct SecurityConfig
{
    struct PasswordHashing
    {
        struct Argon2
        {
            // maximum amount of computations to perform hashing
            qint64 opsLimit = crypto_pwhash_OPSLIMIT_INTERACTIVE;
            // maximum amount of RAM in bytes that the hashing will use
            qint64 memLimit = crypto_pwhash_MEMLIMIT_INTERACTIVE;
        } argon2;
    } passwordHashing;
};

}

#endif // SECURITY_CONFIG_H
