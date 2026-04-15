#include "model.h"

QString Model::authorize(QString accessToken)
{

}

int Model::uploadData(QString uploadId)
{

}

QByteArray Model::downloadData(int fileId)
{

}

void Model::requestDeletion(int fileId)
{

}

QList<DTO::File::TreeNodeResponse> Model::requestFileTree()
{
    QList<DTO::File::TreeNodeResponse> result;
    DTO::File::TreeNodeResponse rootdir;
    rootdir.fileId = 1;
    rootdir.isDirectory = true;
    rootdir.name = "ROOT";

    DTO::File::TreeNodeResponse child;
    child.fileId = 2;
    child.isDirectory = false;
    child.name = "file.doc";
    child.size = 123;

    rootdir.children->append(child);

    result.append(rootdir);

    return result;
}

DTO::Auth::GeneralResponse Model::login(QString username, QString password)
{

}

DTO::Auth::GeneralResponse Model::signup(QString verificationId, int accessCode, QString username, QString password)
{

}

DTO::Auth::RegisterInitResponse Model::sendEmail(QString email)
{

}

