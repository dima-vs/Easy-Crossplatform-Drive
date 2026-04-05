#include <QCoreApplication>
#include <QDebug>
#include <QString>
#include <QTextStream>

#include "auth/service.h"
#include "database_manager.h"
#include "user_repository.h"
#include "file_repository.h"
#include "token_repository.h"

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

    Service::Auth::AuthService authService(userRep, tokenRep);
    RegResult regResult =
        authService.startRegistrationSession("my_email@gmail.com");

    output << "Enter an access code: ";
    output.flush();

    bool ok;
    int accessCode = input.readLine().toInt(&ok);
    if (!ok)
    {
        qDebug() << "Incorrect input!";
        return -1;
    }

    AuthResult authResult = authService.completeRegistration(
        regResult.data().varificationId, accessCode, "my_user_name", "12345678"
    );

    return 0;
    //return a.exec();
}
