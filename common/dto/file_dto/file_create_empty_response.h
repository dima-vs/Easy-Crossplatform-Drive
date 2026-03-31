#ifndef FILE_CREATE_EMPTY_RESPONSE_H
#define FILE_CREATE_EMPTY_RESPONSE_H

#include <QString>
#include <QtTypes>
#include <QDateTime>
#include <optional>
#include "file_type_enum.h"

struct FileCreateEmptyResponseDTO
{
    QDateTime createdAt;
    int fileId;
    QString fileName;
    std::optional<int> parentId; // null = root
};

#endif // FILE_CREATE_EMPTY_RESPONSE_H
