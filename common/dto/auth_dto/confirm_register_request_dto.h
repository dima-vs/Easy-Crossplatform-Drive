#ifndef CONFIRM_REGISTER_REQUEST_DTO_H
#define CONFIRM_REGISTER_REQUEST_DTO_H

#include <QString>

struct ConfirmRegisterRequestDTO
{
    QString verificationId;
    int accessCode;
    QString username;
    QString password;
};


#endif // CONFIRM_REGISTER_REQUEST_DTO_H
