#ifndef SMTP_EMAIL_SENDER_H
#define SMTP_EMAIL_SENDER_H

#include "email/email_sender.h"
#include "config/email_config.h"

namespace Service::Email
{

class SmtpEmailSender : public IEmailSender
{
public:
    SmtpEmailSender(const ::Config::Email::EmailConfig& config);
    void sendAccessCode(const QString& email, int code) override;

private:
    ::Config::Email::EmailConfig m_config;
};

}

#endif // SMTP_EMAIL_SENDER_H
