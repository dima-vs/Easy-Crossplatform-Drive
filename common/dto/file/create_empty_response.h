#ifndef CREATE_EMPTY_RESPONSE_H
#define CREATE_EMPTY_RESPONSE_H

#include <QString>
#include <QtTypes>
#include <QDateTime>
#include <optional>
#include "file_type.h"

namespace DTO::File
{

struct CreateEmptyResponse
{
    QDateTime createdAt;
    int fileId;
    QString fileName;
    std::optional<int> parentId; // null = root
};

}

#endif // CREATE_EMPTY_RESPONSE_H
