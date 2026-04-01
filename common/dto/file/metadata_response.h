#ifndef METADATA_RESPONSE_H
#define METADATA_RESPONSE_H

#include <QString>
#include <QtTypes>
#include <QDateTime>
#include <optional>
#include "file_type.h"

namespace DTO::File
{

struct MetadataResponse
{
    QDateTime createdAt;
    FileType type;
    int fileId;
    QString fileName;
    std::optional<int> parentId; // null = root
    qint64 size;
};

}

#endif // METADATA_RESPONSE_H
