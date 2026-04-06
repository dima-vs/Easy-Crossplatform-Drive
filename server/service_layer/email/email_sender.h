#ifndef EMAIL_SENDER_H
#define EMAIL_SENDER_H

#include <QString>

namespace Service::Email
{

class IEmailSender
{
public:
    virtual ~IEmailSender() = default;
    virtual void sendAccessCode(const QString& email, int code) = 0;
};

}

#endif // EMAIL_SENDER_H
