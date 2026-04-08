#ifndef AUTH_CONFIG_H
#define AUTH_CONFIG_H

#include <sodium.h>

namespace Config::Auth
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

struct AuthConfig
{
    struct Token
    {
        int tokenIdEntropyBytes = 16;
        int tokenSecretEntropyBytes = 32;
        // max count of iterations to generate token in case of
        // it's id already exists
        int idGenerationAttemptsLimit = 8;
    } token;

    struct Uuid
    {
        // max count of iterations to generate uuid in case of
        // it already exists
        int generationAttemptsLimit = 8;
    } uuid;

    struct Security
    {
        int regSessionsDurationSec = 180; // 3 min
        int userSessionsDurationSec = 604800; // 1 week
        int codeEntryAttemptsLimit = 3;
        int maxActiveSessionsPerUser = 5;

        PasswordHashing passwordHashing;
    } security;
};

}

#endif // AUTH_CONFIG_H
