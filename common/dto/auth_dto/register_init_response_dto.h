#ifndef REGISTER_INIT_RESPONSE_DTO_H
#define REGISTER_INIT_RESPONSE_DTO_H

#include <QString>
#include <QDateTime>

struct RegisterInitResponseDTO
{
    QString verificationId;
    QDateTime expiresAt;
};

#endif // REGISTER_INIT_RESPONSE_DTO_H
