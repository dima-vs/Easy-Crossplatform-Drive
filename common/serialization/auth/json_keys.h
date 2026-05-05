#ifndef JSON_KEYS_AUTH_H
#define JSON_KEYS_AUTH_H

namespace JsonKeys::Auth
{

struct ConfirmRegister
{
    static constexpr auto VerificationId = "verificationId";
    static constexpr auto AccessCode = "accessCode";
    static constexpr auto Username = "username";
    static constexpr auto Password = "password";
};

struct General
{
    static constexpr auto AccessToken = "accessToken";
    static constexpr auto ExpiresAt = "expiresAt";
};

struct Login
{
    static constexpr auto LoginField = "login";
    static constexpr auto Password = "password";
};

struct RegisterInit
{
    static constexpr auto Email = "email";
    static constexpr auto VerificationId = "verificationId";
    static constexpr auto ExpiresAt = "expiresAt";
};

}

#endif // JSON_KEYS_AUTH_H
