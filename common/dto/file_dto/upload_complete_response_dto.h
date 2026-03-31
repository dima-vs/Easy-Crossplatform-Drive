#ifndef UPLOAD_COMPLETE_RESPONSE_DTO_H
#define UPLOAD_COMPLETE_RESPONSE_DTO_H

#include <QString>
#include <QtTypes>
#include <QDateTime>
#include <optional>

struct UploadCompleteResponseDTO
{
    QDateTime createdAt;
    int fileId;
    QString fileName;
    std::optional<int> parentId; // null = root
    qint64 size;
};

#endif // UPLOAD_COMPLETE_RESPONSE_DTO_H
