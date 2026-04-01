#ifndef LOGIN_REQUEST_H
#define LOGIN_REQUEST_H

#include <QString>

namespace DTO::Auth
{

struct LoginRequest
{
    QString login;
    QString password;
};

}

#endif // LOGIN_REQUEST_H
