#ifndef FILE_TREE_RESPONSE_DTO_H
#define FILE_TREE_RESPONSE_DTO_H

#include <QtTypes>
#include <QList>
#include <optional>

struct FileNodeDTO
{
    int fileId;
    QString name;
    bool isDirectory;

    std::optional<qint64> size;
    std::optional<QList<FileNodeDTO>> children;
};

#endif // FILE_TREE_RESPONSE_DTO_H
