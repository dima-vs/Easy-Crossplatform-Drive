#ifndef UPLOAD_COMPLETE_RESPONSE_H
#define UPLOAD_COMPLETE_RESPONSE_H

#include <QString>
#include <QtTypes>
#include <QDateTime>
#include <optional>

namespace DTO::File
{

struct UploadCompleteResponse
{
    QDateTime createdAt;
    int fileId;
    QString fileName;
    std::optional<int> parentId; // null = root
    qint64 size;
};

}

#endif // UPLOAD_COMPLETE_RESPONSE_H
