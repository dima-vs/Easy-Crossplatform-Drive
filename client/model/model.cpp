#include "model.h"

QString Model::authorize(QString accessToken)
{
    return "0";
}

int Model::uploadData(QString uploadId)
{
    return 0;
}

DTO::File::UploadInitResponse Model::uploadInit(QString fileName, std::optional<int> parentId, qint64 fileSize, bool overwrite)
{
    DTO::File::UploadInitResponse response;
    response.uploadId = "1";
    response.chunkSize = 1;
    response.expiresAt = QDateTime();
    return response;
}

QByteArray Model::downloadData(int fileId)
{
    QByteArray a;
    return a;
}

void Model::requestDeletion(int fileId) { }

DTO::File::RenameResponse Model::renameFile(std::optional<int> parentId, QString name)
{
    DTO::File::RenameResponse response;
    response.fileId = 1;
    response.fileName = "file";
    response.parentId = 0;
    return response;
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

    rootdir.children = QList<DTO::File::TreeNodeResponse>();
    rootdir.children->append(child);

    result.append(rootdir);

    return result;
}

DTO::Auth::GeneralResponse Model::login(QString username, QString password)
{
    DTO::Auth::GeneralResponse response;
    response.accessToken = "0";
    response.expiresAt = QDateTime();
    return response;
}

DTO::Auth::GeneralResponse Model::signup(QString verificationId, int accessCode, QString username, QString password)
{
    DTO::Auth::GeneralResponse response;
    response.accessToken = "0";
    response.expiresAt = QDateTime();
    return response;
}

DTO::Auth::RegisterInitResponse Model::sendEmail(QString email)
{
    DTO::Auth::RegisterInitResponse response;
    response.expiresAt = QDateTime();
    response.verificationId = "123456";
    return response;
}
