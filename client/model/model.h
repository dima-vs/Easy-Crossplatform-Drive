#ifndef MODEL_H
#define MODEL_H

#include <QObject>
#include <QDir>
#include "dto/file/tree_response.h"
#include "dto/file/upload_init_request.h"
#include "dto/file/upload_init_response.h"
#include "dto/file/upload_complete_response.h"
#include "dto/file/rename_request.h"
#include "dto/file/rename_response.h"
#include "dto/auth/login_request.h"
#include "dto/auth/register_init_request.h"
#include "dto/auth/register_init_response.h"
#include "dto/auth/general_response.h"

class Model : public QObject
{
    Q_OBJECT

private:
    QString authorize(QString accessToken);
    QString token;

    DTO::File::UploadCompleteResponse completeUpload(QString uploadId);

public slots:

    int uploadData(QString uploadId);
    DTO::File::UploadInitResponse uploadInit(QString fileName,
                                             std::optional<int> parentId,
                                             qint64 fileSize,
                                             bool overwrite);
    QByteArray downloadData(int fileId);
    void requestDeletion(int fileId);
    //void changeProperties(QString name, QStringList access);
    DTO::File::RenameResponse renameFile(std::optional<int> parentId, QString name);
    QList<DTO::File::TreeNodeResponse> requestFileTree();
    DTO::Auth::GeneralResponse login(QString username,
                                     QString password);
    DTO::Auth::GeneralResponse signup(QString verificationId,
                                           int accessCode,
                                           QString username,
                                           QString password);
    DTO::Auth::RegisterInitResponse sendEmail(QString email);
};

#endif // MODEL_H
