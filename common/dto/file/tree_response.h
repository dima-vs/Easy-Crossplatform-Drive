#ifndef TREE_RESPONSE_H
#define TREE_RESPONSE_H

#include <QtTypes>
#include <QList>
#include <optional>

namespace DTO::File
{

struct TreeNodeResponse
{
    int fileId;
    QString name;
    bool isDirectory;

    std::optional<qint64> size;
    std::optional<QList<TreeNodeResponse>> children;
};

}

#endif // TREE_RESPONSE_H
