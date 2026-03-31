#ifndef AUTH_RESPONSE_DTO_H
#define AUTH_RESPONSE_DTO_H

#include <QString>
#include <QDateTime>

struct AuthResponseDTO
{
    QString accessToken;
    QDateTime expiresAt;
};

#endif // AUTH_RESPONSE_DTO_H
