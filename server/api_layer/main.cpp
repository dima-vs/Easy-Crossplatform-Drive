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
#include "security/security_config.h"

// auth
#include "auth/service.h"
#include "auth/auth_config.h"

// file
#include "file/service.h"
#include "file/file_config.h"

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

    DatabaseManager dbManager;

    UserRepository userRepository(dbManager);
    TokenRepository tokenRepository(dbManager);
    FileRepository fileRepository(dbManager);
    FileStorage fileStorage("storage");

    Service::Time::SystemTimeProvider timeProvider;
    Service::Mock::MockEmailSender mockEmailSender;

    Config::Security::PasswordHashing pswConfig;
    Service::Security::SodiumPasswordHasher passwordHasher(pswConfig);

    Config::Auth::AuthConfig authConfig;
    Config::File::FileConfig fileConfig;

    Service::Auth::AuthService authService(
        userRepository,
        tokenRepository,
        mockEmailSender,
        timeProvider,
        passwordHasher,
        authConfig
        );

    Service::File::FileService fileService(
        fileStorage,
        fileRepository,
        timeProvider,
        fileConfig
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

    const int port = 8080;
    QTcpServer* tcpServer = new QTcpServer();

    if (!tcpServer->listen(QHostAddress::LocalHost, port) ||
        !httpServer.bind(tcpServer))
    {
        qCritical() << "Failed to start HTTP server or bind to port" << port;
        delete tcpServer;
        return -1;
    }
    qDebug().noquote().nospace() << "HTTP Server successfully started " <<
                tcpServer->serverAddress().toString() << ":" << port;

    return a.exec();
}
