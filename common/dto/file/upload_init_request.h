#ifndef UPLOAD_INIT_REQUEST_H
#define UPLOAD_INIT_REQUEST_H

#include <QString>
#include <QtTypes>
#include <optional>

namespace DTO::File
{

struct UploadInitRequest
{
    QString fileName;
    std::optional<int> parentId; // null = root
    qint64 fileSize;
    bool overwrite;
};

}

#endif // UPLOAD_INIT_REQUEST_H
