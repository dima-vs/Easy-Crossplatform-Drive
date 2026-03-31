#ifndef FILE_METADATA_RESPONSE_DTO_H
#define FILE_METADATA_RESPONSE_DTO_H

#include <QString>
#include <QtTypes>
#include <QDateTime>
#include <optional>
#include "file_type_enum.h"

struct FileMetadataResponseDTO
{
    QDateTime createdAt;
    FileType type;
    int fileId;
    QString fileName;
    std::optional<int> parentId; // null = root
    qint64 size;
};

#endif // FILE_METADATA_RESPONSE_DTO_H
