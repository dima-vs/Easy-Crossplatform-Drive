#ifndef UPLOAD_INIT_RESPONSE_H
#define UPLOAD_INIT_RESPONSE_H

#include <QString>
#include <QtTypes>
#include <QDateTime>

namespace DTO::File
{

struct UploadInitResponse
{
    qint64 chunkSize;
    QString uploadId;
    QDateTime expiresAt;
};

}

#endif // UPLOAD_INIT_RESPONSE_H
