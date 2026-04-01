#ifndef RENAME_REQUEST_H
#define RENAME_REQUEST_H

#include <QString>
#include <optional>

namespace DTO::File
{

struct RenameRequest
{
    std::optional<int> newParentId; // null = root
    QString newFileName;
};

}

#endif // RENAME_REQUEST_H
