#ifndef TREE_RESPONSE_H
#define TREE_RESPONSE_H

#include <QtTypes>
#include <QList>
#include <optional>

namespace DTO::File
{

struct TreeNodeResponse
{
    int fileId = 0;
    QString name;
    bool isDirectory = false;

    std::optional<qint64> size = std::nullopt;
    std::optional<QList<TreeNodeResponse>> children = std::nullopt;
};

}

#endif // TREE_RESPONSE_H
