#include <QCoreApplication>
#include <QDebug>
#include <QString>
#include <QTextStream>

#include "auth/service.h"
#include "database_manager.h"
#include "user_repository.h"
#include "file_repository.h"
#include "token_repository.h"
#include "email/email_sender.h"


class MailServiceSpy : public Service::Email::IEmailSender
{
public:
    int lastSentCode = 0;
    QString lastEmail;

    void sendAccessCode(const QString& email, int code) override
    {
        lastSentCode = code;
        lastEmail = email;
    }
};

int main(int argc, char *argv[])
{
    namespace Model = ::ServiceModel::Auth;
    using AuthResult = ServiceResult<Model::Result, ErrorCode::Auth::ServiceError>;
    using ServiceError = ::ErrorCode::Auth::ServiceError;
    using RegResult = ServiceResult<Model::RegistrationSessionResult, ServiceError>;

    QCoreApplication a(argc, argv);

    qDebug() << "ServiceLayer";

    DatabaseManager db;
    UserRepository userRep(db);
    // FileRepository fileRep(db);
    TokenRepository tokenRep(db);

    QTextStream input(stdin);
    QTextStream output(stdout);

    MailServiceSpy mailSpy;
    Service::Auth::AuthService authService(userRep, tokenRep, mailSpy);

    for (int i = 1; i <= 4; ++i)
    {
        QString email = QString("user%1@gmail.com").arg(i);
        QString username = QString("user_name_%1").arg(i);
        QString password = "password123";

        qDebug() << "--- Registering user" << i << ":" << email << "---";

        RegResult regResult = authService.startRegistrationSession(email);

        if (!regResult.isOk())
        {
            qDebug() << "Failed to start registration for" << email;
            continue;
        }

        AuthResult authResult = authService.completeRegistration(
            regResult.data().varificationId,
            mailSpy.lastSentCode,
            username,
            password
            );

        if (authResult.isOk()) {
            qDebug() << "Successfully registered:" << username;
        } else {
            qDebug() << "Failed to complete registration for" << username;
        }
    }

    return a.exec();
}
