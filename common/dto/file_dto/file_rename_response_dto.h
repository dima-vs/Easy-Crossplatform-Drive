#ifndef FILE_RENAME_RESPONSE_DTO_H
#define FILE_RENAME_RESPONSE_DTO_H

#include <QString>
#include <optional>

struct FileRenameResponseDTO
{
    int fileId;
    QString fileName;
    std::optional<int> parentId; // null = root
};

#endif // FILE_RENAME_RESPONSE_DTO_H
