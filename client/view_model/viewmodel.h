#ifndef VIEWMODEL_H
#define VIEWMODEL_H

#include <qwindowdefs.h>
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
#include "nodata.h"
#include <QSettings>

class viewmodel
{
public:
    viewmodel();

public slots:
    void on_sendEmailFinished(std::optional<DTO::Auth::RegisterInitResponse>);
    void on_signupFinished(std::optional<DTO::Auth::GeneralResponse>);
    void on_loginFinished(std::optional<DTO::Auth::GeneralResponse>);
    void on_treeRequestFinished(std::optional<QList<DTO::File::TreeNodeResponse>>);
    void on_renameFinished(std::optional<DTO::File::RenameResponse>);
    void on_deletionFinished(std::optional<CommonTypes::NoData>);
    void on_uploadInitFinished(std::optional<DTO::File::UploadInitResponse>);
    void on_uploadCompleteFinished(std::optional<DTO::File::UploadCompleteResponse>);
    void on_uploadFinished(std::optional<CommonTypes::NoData>);
    void on_downloadChunkFinished(std::optional<QByteArray>);

    void on_saveToken(QString accessToken);
};

#endif // VIEWMODEL_H
