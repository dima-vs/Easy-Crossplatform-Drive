#ifndef EMAIL_CONFIG_H
#define EMAIL_CONFIG_H

#include <QString>

namespace Config::Email
{

struct EmailConfig
{
    struct Smtp
    {
        QString host = "smtp.gmail.com";
        int port = 465;
        QString connectionType = "SslConnection";
    } smtp;

    struct Credentials
    {
        QString email = "";
        QString appPassword = "";
    } credentials;

    struct Sender
    {
        QString displayName = "EasyCrossplatformDrive";
    } sender;
};

}

#endif // EMAIL_CONFIG_H
