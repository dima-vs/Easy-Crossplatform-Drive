#ifndef REGISTER_INIT_RESPONSE_H
#define REGISTER_INIT_RESPONSE_H

#include <QString>
#include <QDateTime>

namespace DTO::Auth
{

struct RegisterInitResponse
{
    QString verificationId;
    QDateTime expiresAt;
};

}

#endif // REGISTER_INIT_RESPONSE_H
