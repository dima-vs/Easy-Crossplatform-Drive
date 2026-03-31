#ifndef FILE_CREATE_EMPTY_REQUEST_H
#define FILE_CREATE_EMPTY_REQUEST_H

#include <QString>
#include <optional>
#include "file_type_enum.h"

struct FileCreateEmptyRequestDTO
{
    QString fileName;
    std::optional<int> parentId; // null = root
    FileType type;
    bool overwrite;
};

#endif // FILE_CREATE_EMPTY_REQUEST_H
