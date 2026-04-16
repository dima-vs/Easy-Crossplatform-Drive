#ifndef FILETREE_H
#define FILETREE_H

#include <QTreeWidget>
#include <QTreeWidgetItem>
#include "dto/file/tree_response.h"

class FileTree : public QTreeWidget
{
    Q_OBJECT
public:
    FileTree(QList<DTO::File::TreeNodeResponse> &tree);
    QList<QTreeWidgetItem*> createTree(const QList<DTO::File::TreeNodeResponse>& nodes);
    void onLoad();

private:
    QList<DTO::File::TreeNodeResponse> &fileTree;

signals:
    void initializedProgram(QTreeWidget *treeWidget);

public slots:

};

#endif // FILETREE_H
