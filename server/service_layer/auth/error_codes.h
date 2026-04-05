#ifndef ERROR_CODES_H
#define ERROR_CODES_H

namespace ErrorCode::Auth
{

enum class ServiceError
{
    InvalidCredentials,
    SessionExpired,
    SessionDoesNotExists,
    PasswordHashingFailed,
    CannotAddNewUser,
    TokenAlreadyExists,
    CannotAddNewToken,
    IdIsNotSet,
    TooManyAttempts
};

}

#endif // ERROR_CODES_H
