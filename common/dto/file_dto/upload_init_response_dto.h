#ifndef UPLOAD_INIT_RESPONSE_DTO_H
#define UPLOAD_INIT_RESPONSE_DTO_H

#include <QString>
#include <QtTypes>
#include <QDateTime>

struct UploadInitResponseDTO
{
    qint64 chunkSize;
    QString uploadId;
    QDateTime expiresAt;
};

#endif // UPLOAD_INIT_RESPONSE_DTO_H
