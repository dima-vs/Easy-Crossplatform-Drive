#include <QCoreApplication>
#include <QDebug>
#include <QString>
#include <QTextStream>
#include <QDateTime>
#include <QDate>
#include <QTime>

#include "auth/service.h"
#include "database_manager.h"
#include "user_repository.h"
#include "file_repository.h"
#include "token_repository.h"
#include "email/email_sender.h"
#include "datetime/time_provider_interface.h"
#include "security/password_hasher_interface.h"
#include "security/sodium_password_hasher.h"
#include "security/security_config.h"


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

class MockTimeProvider : public Service::Time::ITimeProvider
{
public:
    QDateTime manualTime;

    QDateTime currentDateTimeUtc() const override
    {
        return manualTime;
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
    MockTimeProvider timeProvider;
    Service::Security::SodiumPasswordHasher pswHasher(
        Config::Security::PasswordHashing {}
        );

    Service::Auth::AuthService authService(
        userRep, tokenRep,
        mailSpy, timeProvider,
        pswHasher,
        Config::Auth::AuthConfig());

    timeProvider.manualTime = QDateTime(QDate(2026, 4, 6), QTime(12, 0, 0));

    for (int i = 1; i <= 4; ++i)
    {
        if (i == 4)
        {
            // skip a month for fourth user
            timeProvider.manualTime = timeProvider.manualTime.addMonths(1);
        }

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
            regResult.data().verificationId,
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

    timeProvider.manualTime = timeProvider.manualTime
                              .addMonths(-1)
                              .addDays(8);
    // fourth user's token must remain
    authService.clearExpiredTokens();

    return 0;
}
