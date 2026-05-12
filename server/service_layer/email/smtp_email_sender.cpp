#include "smtp_email_sender.h"
#include "SmtpMime"
#include <QDebug>
#include <QObject>

namespace Service::Email
{

class AsyncMailTask : public QObject
{
public:
    AsyncMailTask(
        const ::Config::Email::EmailConfig& config,
        const QString& recipient,
        int code
        )
        : m_config(config), m_recipient(recipient)
    {

        EmailAddress sender(
            m_config.credentials.email,
            m_config.sender.displayName
            );
        m_message.setSender(sender);

        EmailAddress to(recipient, "");
        m_message.addRecipient(to);
        m_message.setSubject("Your Verification Code");

        QString body = QString("Welcome to EasyCrossplatformDrive!\n\n"
                               "Your 6-digit verification code is: %1\n\n"
                               "This code will expire soon. Do not share it.")
                           .arg(code, 6, 10, QLatin1Char('0'));
        m_text.setText(body);
        m_message.addPart(&m_text);

        SmtpClient::ConnectionType connType = SmtpClient::SslConnection;
        if (config.smtp.connectionType == "TlsConnection")
            connType = SmtpClient::TlsConnection;
        else if (config.smtp.connectionType == "TcpConnection")
            connType = SmtpClient::TcpConnection;

        m_client = new SmtpClient(
            config.smtp.host,
            config.smtp.port,
            connType
            );

        connect(m_client, &SmtpClient::readyConnected, this, [this]() {
            m_client->login(
                m_config.credentials.email,
                m_config.credentials.appPassword
                );
        });

        connect(m_client, &SmtpClient::authenticated, this, [this]() {
            m_client->sendMail(m_message);
        });

        connect(m_client, &SmtpClient::mailSent, this, [this]() {
            qInfo() << "async mail sent successfully to:" << m_recipient;
            m_client->quit();
            this->deleteLater();
        });

        connect(m_client, &SmtpClient::error, this, [this](SmtpClient::SmtpError e) {
            qCritical() << "SMTP Async Error:" << SmtpClient::string(e) <<
                "for" << m_recipient;
            m_client->quit();
            this->deleteLater();
        });
    }

    ~AsyncMailTask() {
        delete m_client;
    }

    void start() {
        m_client->connectToHost();
    }

private:
    ::Config::Email::EmailConfig m_config;
    QString m_recipient;
    MimeMessage m_message;
    MimeText m_text;
    SmtpClient* m_client;
};


SmtpEmailSender::SmtpEmailSender(const ::Config::Email::EmailConfig& config)
    : m_config(config)
{
}

void SmtpEmailSender::sendAccessCode(const QString& email, int code)
{
    if (m_config.credentials.email.isEmpty() ||
        m_config.credentials.appPassword.isEmpty())
    {
        qWarning() << "empty SMTP credentials. Cannot send email to:" << email;
        return;
    }

    // will be cleared on deleteLater call
    auto* task = new AsyncMailTask(m_config, email.trimmed(), code);
    task->start();
}

}
