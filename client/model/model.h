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
#include <optional>

#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QUrl>
#include <QUrlQuery>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>
#include <QJsonArray>
#include "serialization/auth/json.h"
#include "serialization/file/json.h"
#include "serialization/file/json_keys.h"
#include "domain/file_type.h"
#include "nodata.h"

class Model : public QObject
{
    Q_OBJECT

private:
    bool authorize(QHttpHeaders &headersOut);
    QString getBaseUrl() { return protocol + "://" + hostStr + port; }
    void setSpecificHeaders(QHttpHeaders &headersOut);
    void setNgrokSpecificHeaders(QHttpHeaders &headersOut);

    QString token;

    QString protocol = "http";
    QString hostStr = "127.0.0.1";
    QString port = ":8080";
    QString baseUrl;

    QNetworkAccessManager manager = QNetworkAccessManager(this);

public slots:
    void uploadData(QString uploadId, QByteArray chunkData, qint64 startByte, qint64 endByte, qint64 totalBytes);
    void uploadInit(QString fileName,
                    std::optional<int> parentId,
                    qint64 fileSize,
                    bool overwrite);
    void downloadData(int fileId, qint64 startByte, qint64 endByte);
    void completeUpload(QString uploadId);
    void requestDeletion(int fileId);
    void renameFile(int fileId, std::optional<int> parentId, std::optional<QString> name);
    void requestFileTree();
    void login(QString username,
               QString password);
    void signup(QString verificationId,
                int accessCode,
                QString username,
                QString password);
    void sendEmail(QString email);
    void createFolder(QString fileName, std::optional<int> parentId, bool overwrite);
    void getSavedToken(QString accessToken);


signals:
    void sendEmailFinished(std::optional<DTO::Auth::RegisterInitResponse>);
    void signupFinished(std::optional<DTO::Auth::GeneralResponse>);
    void loginFinished(std::optional<DTO::Auth::GeneralResponse>);
    void treeRequestFinished(std::optional<QList<DTO::File::TreeNodeResponse>>);
    void renameFinished(std::optional<DTO::File::RenameResponse>);
    void deletionFinished(std::optional<CommonTypes::NoData>);
    void uploadInitFinished(std::optional<DTO::File::UploadInitResponse>);
    void uploadCompleteFinished(std::optional<DTO::File::UploadCompleteResponse>);
    void uploadFinished(std::optional<CommonTypes::NoData>);
    void downloadChunkFinished(std::optional<QByteArray>);
    void createFolderFinished(std::optional<DTO::File::CreateEmptyResponse>);
    void saveToken(QString accessToken);
};

#endif // MODEL_H
