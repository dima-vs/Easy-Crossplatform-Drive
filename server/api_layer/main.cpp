#include <QCoreApplication>
#include <QDebug>
#include <QHttpServer>
#include <QTcpServer>
#include <QString>
#include <QHostAddress>

// database repositories
#include "database_manager.h"
#include "user_repository.h"
#include "token_repository.h"
#include "file_repository.h"

// services
#include "email/email_sender.h"
#include "datetime/system_time_provider.h"
#include "security/sodium_password_hasher.h"
#include "config/security_config.h"

// auth
#include "auth/service.h"
#include "config/auth_config.h"

// file
#include "file/service.h"
#include "config/file_config.h"

// config
#include "config/service.h"

// email
#include "email/smtp_email_sender.h"

// controllers
#include "auth/auth_controller.h"
#include "file/file_controller.h"

namespace Service::Mock
{
class MockEmailSender : public Service::Email::IEmailSender
{

public:
    void sendAccessCode(const QString& email, int code) override
    {
        qDebug() << "--------------------";
        qDebug() << "[MOCK EMAIL SENDER]";
        qDebug() << "To:" << email;
        qDebug() << "Access Code:" << code;
        qDebug() << "--------------------";
    }
};

}


int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    qDebug() << "APILayer";

    Service::Config::ConfigService cfgService("server_config.json");
    cfgService.saveDefaultsIfNotExist();

    if (!cfgService.load())
    {
        qCritical() << "failed to load configuration file! Exiting.";
        return -1;
    }

    DatabaseManager dbManager;

    UserRepository userRepository(dbManager);
    TokenRepository tokenRepository(dbManager);
    FileRepository fileRepository(dbManager);
    FileStorage fileStorage(cfgService.file().storage.baseStoragePath);

    Service::Time::SystemTimeProvider timeProvider;
    // Service::Mock::MockEmailSender mockEmailSender;
    Service::Email::SmtpEmailSender smtpEmailSender(cfgService.email());

    Service::Security::SodiumPasswordHasher passwordHasher(
        cfgService.security().passwordHashing
        );

    Service::Auth::AuthService authService(
        userRepository,
        tokenRepository,
        // mockEmailSender,
        smtpEmailSender,
        timeProvider,
        passwordHasher,
        cfgService.auth()
        );

    Service::File::FileService fileService(
        fileStorage,
        fileRepository,
        timeProvider,
        cfgService.file()
        );

    QHttpServer httpServer;

    Api::Controllers::AuthController authController(httpServer, authService);
    authController.registerEndpoints();

    Api::Controllers::FileController fileController(
        httpServer,
        fileService,
        authService
        );
    fileController.registerEndpoints();

    httpServer.route("/", [] () {
        return "EasyCrossplatformDrive server is running!";
    });

    Config::App::AppConfig appCfg = cfgService.app();
    const int port = appCfg.port;
    QTcpServer* tcpServer = new QTcpServer();

    if (!tcpServer->listen(QHostAddress(appCfg.host), port) ||
        !httpServer.bind(tcpServer))
    {
        qCritical() << "failed to start HTTP server or bind to port" << port;
        delete tcpServer;
        return -1;
    }

    qDebug().noquote().nospace() << "HTTP Server successfully started at "
                                 << appCfg.protocol << "://"
                                 << tcpServer->serverAddress().toString()
                                 << ":" << port;

    return a.exec();
}
