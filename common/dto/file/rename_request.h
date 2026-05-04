#ifndef RENAME_REQUEST_H
#define RENAME_REQUEST_H

#include <QString>
#include <QVariant>
#include <optional>

namespace DTO::File
{

struct RenameRequest
{
    std::optional<QVariant> newParentId;
    std::optional<QString> newFileName;
};

}

#endif // RENAME_REQUEST_H
