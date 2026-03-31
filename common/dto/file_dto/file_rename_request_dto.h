#ifndef FILE_RENAME_REQUEST_DTO_H
#define FILE_RENAME_REQUEST_DTO_H

#include <QString>
#include <optional>

struct FileRenameRequestDTO
{
    std::optional<int> newParentId; // null = root
    QString newFileName;
};

#endif // FILE_RENAME_REQUEST_DTO_H
