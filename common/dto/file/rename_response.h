#ifndef RENAME_RESPONSE_H
#define RENAME_RESPONSE_H

#include <QString>
#include <optional>

namespace DTO::File
{

struct RenameResponse
{
    int fileId;
    QString fileName;
    std::optional<int> parentId; // null = root
};

}

#endif // RENAME_RESPONSE_H
