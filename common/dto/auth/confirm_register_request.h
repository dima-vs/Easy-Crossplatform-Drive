#ifndef CONFIRM_REGISTER_REQUEST_H
#define CONFIRM_REGISTER_REQUEST_H

#include <QString>

namespace DTO::Auth
{

struct ConfirmRegisterRequest
{
    QString verificationId;
    int accessCode;
    QString username;
    QString password;
};

}

#endif // CONFIRM_REGISTER_REQUEST_H
