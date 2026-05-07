#ifndef VIEWMODEL_H
#define VIEWMODEL_H

#include <qwindowdefs.h>
#include <QDir>
#include "dto/file/create_empty_response.h"
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
#include "nodata.h"
#include "ViewConfig.h"
#include "filetree.h"
#include <QSettings>
#include <QFileDialog>
#include <QStandardPaths>
#include <QtTypes>

class viewmodel : public QObject
{
    Q_OBJECT
public:
    viewmodel();

private:
    struct TransferContext
    {
        QString name;
        QString uploadId;
        int fileId;
        qint64 chunkSize;
        qint64 startByte;
        qint64 endByte;
        qint64 total;

        bool isEnd;
    };

    TransferContext uc;
    TransferContext dc;
    QFile file;
    FileTree ftr;
    void completeUploadSession();
    QString getToken();
    QString verificationId;
    QString filePath;

public slots:
    //from model
    void on_sendEmailFinished(std::optional<DTO::Auth::RegisterInitResponse> response);
    void on_signupFinished(std::optional<DTO::Auth::GeneralResponse> response);
    void on_loginFinished(std::optional<DTO::Auth::GeneralResponse> response);
    void on_treeRequestFinished(std::optional<QList<DTO::File::TreeNodeResponse>> response);
    void on_renameFinished(std::optional<DTO::File::RenameResponse> response);
    void on_deletionFinished(std::optional<CommonTypes::NoData> nd);
    void on_uploadInitFinished(std::optional<DTO::File::UploadInitResponse> response);
    void on_uploadCompleteFinished(std::optional<DTO::File::UploadCompleteResponse> response);
    void on_uploadFinished(std::optional<CommonTypes::NoData> nd);
    void on_downloadChunkFinished(std::optional<QByteArray> data);
    void on_saveToken(QString accessToken);
    void on_createFolderFinished(std::optional<DTO::File::CreateEmptyResponse> response);
    //from view
    void on_loadSettings();
    void on_userUpload(std::optional<int> id);
    void on_userDownload(int fileId, QString name, qint64 size);
    void on_userEmail(QString email);
    void on_userSignup(QString username, int code, QString password);
    void on_userLogin(QString login, QString password);
    void on_requestTree();
    void on_userCreateFolder(QString name, std::optional<int> id);
    void on_userRequestDeletion(std::optional<int> id);
    void on_userUpdateConfig(CommonTypes::ViewConfig cfg);
    void on_userRequestRename(int fileId, std::optional<int> parentId, std::optional<QString> name);
    //from filetree
    void on_updateTree(QTreeWidget *treeWidget);
signals:
    //to view
    void showMessageBox(QString message);
    void showVerificationDialog();
    void updateTree(QTreeWidget *treeWidget);
    void applySettings(CommonTypes::ViewConfig cfg);

    //to model
    void initUpload(QString fileName,
                    std::optional<int> parentId,
                    qint64 fileSize,
                    bool overwrite);
    void uploadData(QString uploadId, QByteArray chunkData, qint64 startByte, qint64 endByte, qint64 totalBytes);
    void completeUpload(QString uploadId);
    void userSendEmail(QString email);
    void userSignUp(QString verificationId,
                    int accessCode,
                    QString username,
                    QString password);
    void userLogin(QString username, QString password);
    void requestTree();
    void sendToken(QString accessToken);
    void createFolder(QString name, std::optional<int> parentId, bool overwrite);
    void deleteFile(int id);
    void downloadFile(int fileId, qint64 startByte, qint64 endByte);
    void renameFile (int fileId, std::optional<int> parentId, std::optional<QString> name);
};

#endif // VIEWMODEL_H
