#ifndef CREATE_EMPTY_REQUEST_H
#define CREATE_EMPTY_REQUEST_H

#include <QString>
#include <optional>
#include "domain/file_type.h"

namespace DTO::File
{

struct CreateEmptyRequest
{
    QString fileName;
    std::optional<int> parentId; // null = root
    Common::Domain::FileType type;
    bool overwrite;
};

}

#endif // CREATE_EMPTY_REQUEST_H
