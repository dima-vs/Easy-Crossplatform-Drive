#ifndef MODELS_H
#define MODELS_H

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
    QString userName;
    QString accessToken;
    QDateTime expiresAt;
};

}

#endif // MODELS_H
