#include <QCoreApplication>
#include <QDebug>
#include <QString>
#include <QJsonDocument>
#include <QFile>
#include "serialization/auth/auth_json.h"
#include "dto/auth_dto/confirm_register_request_dto.h"

using namespace Serialization;

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    qDebug() << "Common";

    ConfirmRegisterRequestDTO confRegDto;
    confRegDto.accessCode = 434543;
    confRegDto.password = "123345";
    confRegDto.username = "username";
    confRegDto.verificationId = "ffrgtet-5454hg3-g5445h-hywf3d";

    QString fileName = "dto.json";
    QFile file(fileName);
    QJsonObject jsObj = Auth::toJson(confRegDto);
    QJsonDocument jsDoc = QJsonDocument(jsObj);

    file.open(QIODevice::WriteOnly);
    file.write(jsDoc.toJson());

    qDebug() << "[AuthResponse] has value: " <<
        Auth::fromJsonAuthResponse(jsObj)
                            .has_value(); // false
    qDebug() << "[RegisterInitResponse] has value: " <<
        Auth::fromJsonRegisterInitResponse(jsObj)
                            .has_value(); // false
    qDebug() << "[ConfirmRegisterRequest] has value: " <<
        Auth::fromJsonConfirmRegisterRequest(jsObj)
                            .has_value(); // true

    ConfirmRegisterRequestDTO otherConfRegDto =
        Auth::fromJsonConfirmRegisterRequest(jsObj).value();

    file.close();

    return 0;
    //return a.exec();
}
