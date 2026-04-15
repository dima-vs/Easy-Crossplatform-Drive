#ifndef FILE_MODELS_H
#define FILE_MODELS_H

#include <QString>
#include <QVariant>
#include <QDateTime>
#include <QList>
#include <list>

namespace ServiceModel::File
{

struct FileNode
{
    int id;
    QString logicalName;
    QString type;
    QVariant size;
    QDateTime uploadTime;
    std::list<FileNode> children;
};

}

#endif // FILE_MODELS_H
