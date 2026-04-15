#ifndef AUTH_MODELS_H
#define AUTH_MODELS_H

#include <QDateTime>
#include <QString>

namespace ServiceModel::Auth
{

struct RegistrationSession
{
    int accessCode;
    QString email;
    QDateTime expiresAt;
    int attemptsCount;
};

struct RegistrationSessionResult
{
    QString verificationId;
    QDateTime expiresAt;
};

struct Result
{
    int userId;
    QString userName;
    QString accessToken;
    QDateTime expiresAt;
};

}

#endif // AUTH_MODELS_H
