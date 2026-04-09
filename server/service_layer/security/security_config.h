#ifndef SECURITY_CONFIG_H
#define SECURITY_CONFIG_H

#include <sodium.h>

namespace Config::Security
{

struct PasswordHashing
{
    struct Argon2
    {
        // maximum amount of computations to perform hashing
        int opsLimit = crypto_pwhash_OPSLIMIT_INTERACTIVE;
        // maximum amount of RAM in bytes that the hashing will use
        int memLimit = crypto_pwhash_MEMLIMIT_INTERACTIVE;
    } argon2;
};

}

#endif // SECURITY_CONFIG_H
