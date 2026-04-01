#ifndef GENERAL_RESPONSE_H
#define GENERAL_RESPONSE_H

#include <QString>
#include <QDateTime>

namespace DTO::Auth
{

struct GeneralResponse
{
    QString accessToken;
    QDateTime expiresAt;
};

}

#endif // GENERAL_RESPONSE_H
