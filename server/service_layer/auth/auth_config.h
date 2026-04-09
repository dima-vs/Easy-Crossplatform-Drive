#ifndef AUTH_CONFIG_H
#define AUTH_CONFIG_H

namespace Config::Auth
{

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
    } security;
};

}

#endif // AUTH_CONFIG_H
