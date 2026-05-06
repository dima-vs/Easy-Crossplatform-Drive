#include "viewmodel.h"
#include <qcoreapplication.h>

viewmodel::viewmodel()
{
    QObject::connect(&ftr, &FileTree::updateTree, this, &viewmodel::on_updateTree);
}

void viewmodel::completeUploadSession()
{
    file.close();
    emit completeUpload(uc.uploadId);
    uc.uploadId.clear();
    uc.name.clear();
}

void viewmodel::on_sendEmailFinished(std::optional<DTO::Auth::RegisterInitResponse> response)
{
    if (!response.has_value())
    {
        emit showMessageBox("Failed to send email!");
    }
    else
    {
        verificationId = response.value().verificationId;
        emit showVerificationDialog();
    }
}

void viewmodel::on_signupFinished(std::optional<DTO::Auth::GeneralResponse> response)
{
    if (!response.has_value())
    {
        emit showMessageBox("Failed to signup!");
    }
    else
    {
        emit requestTree();
        emit showMessageBox(QString("Your session expires at: %1 UTC").arg(response.value().expiresAt.toString("dd-MM-yyyy hh:mm:ss")));
    }
}

void viewmodel::on_loginFinished(std::optional<DTO::Auth::GeneralResponse> response)
{
    if (!response.has_value())
    {
        emit showMessageBox("Failed to login!");
    }
    else
    {
        emit requestTree();
        emit showMessageBox(QString("Your session expires at: %1 UTC").arg(response.value().expiresAt.toString("dd-MM-yyyy hh:mm:ss")));
    }
}

void viewmodel::on_treeRequestFinished(std::optional<QList<DTO::File::TreeNodeResponse>> response)
{
    if (!response.has_value())
    {
        emit showMessageBox("Failed to receive the filetree!");
    }
    else
    {
        ftr.onLoad(response.value());
    }
}

QString getToken()
{
    QString configPath = QCoreApplication::applicationDirPath() + "/config.cfg";

    QSettings settings(configPath, QSettings::IniFormat);

    settings.beginGroup("User");

    QString token = settings.value("accessToken", "").toString();

    settings.endGroup();

    return token;
}

void viewmodel::on_uploadInitFinished(std::optional<DTO::File::UploadInitResponse> response)
{
    if (!response.has_value())
    {
        emit showMessageBox("Response empty!");
        file.close();
        return;
    }
    uc.chunkSize = response.value().chunkSize;
    uc.uploadId = response.value().uploadId;
    uc.endByte = uc.chunkSize-1;
    qDebug() << uc.chunkSize;
    CommonTypes::NoData nd;
    on_uploadFinished(nd);
}

void viewmodel::on_uploadCompleteFinished(std::optional<DTO::File::UploadCompleteResponse> response)
{
    if (!response.has_value())
    {
        emit showMessageBox("Upload failed!");
        return;
    }
}

void viewmodel::on_uploadFinished(std::optional<CommonTypes::NoData> nd)
{
    if (uc.isEnd)
    {
        qDebug() << "upload end";
        completeUploadSession();
        return;
    }
    qDebug() << "start " << uc.startByte << " end " << uc.endByte << " total " << uc.total;
    QByteArray chunk;
    if (file.seek(uc.startByte))
    {
        chunk = file.read(uc.chunkSize);
        uc.startByte += uc.chunkSize;
        uc.endByte += std::min(uc.chunkSize, chunk.size());
    }
    if (uc.endByte >= uc.total)
    {
        uc.isEnd = true;
    }

    emit uploadData(uc.uploadId, chunk, uc.startByte, uc.endByte, uc.total);
}

void viewmodel::on_saveToken(QString accessToken)
{
    QSettings settings("config.cfg", QSettings::IniFormat);
    settings.beginGroup("User");
    settings.setValue("accessToken", accessToken);
    settings.endGroup();
    settings.sync();
}

void viewmodel::on_createFolderFinished(std::optional<DTO::File::CreateEmptyResponse> response)
{
    emit requestTree();
}

void viewmodel::on_userUpload()
{
    QString fileName = QFileDialog::getOpenFileName(
        NULL,
        tr("Open File"),
        "",
        tr("All (*.*)")
        );
    uc.name = fileName;
    bool overwrite = false;
    std::optional<int> parentId = std::nullopt;
    if (true)
    {
        overwrite = true;
    }
    else
    {

    }

    if (fileName.isEmpty())
    {
        emit showMessageBox("Filename is empty!");
        return;
    }
    file.setFileName(fileName);
    if (!file.open(QIODevice::ReadOnly))
    {
        emit showMessageBox("Can't open file!");
        return;
    }

    QFileInfo fileInfo(fileName);
    qint64 sizeInBytes = fileInfo.size();
    uc.total = fileInfo.size();
    uc.startByte = 0;
    uc.endByte = 0;
    uc.isEnd = false;
    emit initUpload(uc.name, parentId, sizeInBytes, overwrite);
    //file.close();
}

void viewmodel::on_userEmail(QString email)
{
    emit userSendEmail(email);
}

void viewmodel::on_userSignup(QString username, int code, QString password)
{
    emit userSignUp(verificationId, code, username, password);
}

void viewmodel::on_userLogin(QString login, QString password)
{
    emit userLogin(login, password);
}

void viewmodel::on_requestTree()
{
    emit requestTree();
}

void viewmodel::on_userCreateFolder(QString name, std::optional<int> id)
{
    emit createFolder(name, id, true);
}

void viewmodel::on_updateTree(QTreeWidget *treeWidget)
{
    emit updateTree(treeWidget);
}
