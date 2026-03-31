#ifndef UPLOAD_INIT_REQUEST_DTO_H
#define UPLOAD_INIT_REQUEST_DTO_H

#include <QString>
#include <QtTypes>
#include <optional>

struct UploadInitRequestDTO
{
    QString fileName;
    std::optional<int> parentId; // null = root
    qint64 fileSize;
    bool overwrite;
};

#endif // UPLOAD_INIT_REQUEST_DTO_H
