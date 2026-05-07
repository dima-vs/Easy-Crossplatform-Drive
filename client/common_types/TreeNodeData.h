#ifndef TREENODEDATA_H
#define TREENODEDATA_H
#include <qobject.h>
#include <QMetaType>

namespace CommonTypes
{
struct TreeNodeData
{
    int fileId;
    qint64 size;
    bool isDirectory;
};

}
Q_DECLARE_METATYPE(CommonTypes::TreeNodeData);
#endif // TREENODEDATA_H
