#include "filetree.h"

FileTree::FileTree(QList<DTO::File::TreeNodeResponse> &tree) : fileTree(tree)
{

}

QList<QTreeWidgetItem*> FileTree::createTree(const QList<DTO::File::TreeNodeResponse>& nodes)
{
    QList<QTreeWidgetItem*> itemList;

    for (const auto& node : nodes)
    {
        QTreeWidgetItem* item = new QTreeWidgetItem();
        item->setText(0, node.name);

        if (node.isDirectory && node.children.has_value())
        {
            auto childrenItems = createTree(node.children.value());
            item->addChildren(childrenItems);
        }

        itemList.append(item);
    }

    return itemList;
}

void FileTree::onLoad()
{
    QList<DTO::File::TreeNodeResponse> result;

    DTO::File::TreeNodeResponse rootdir;
    rootdir.fileId = 1;
    rootdir.isDirectory = true;
    rootdir.name = "ROOT";

    DTO::File::TreeNodeResponse child;
    child.fileId = 2;
    child.isDirectory = false;
    child.name = "file.doc";
    child.size = 123;

    rootdir.children = QList<DTO::File::TreeNodeResponse>();
    rootdir.children->append(child);

    result.append(rootdir);

    QList<QTreeWidgetItem*> list = createTree(result);

    QTreeWidget* treeWidget = new QTreeWidget();

    for (int i = 0; i < list.size(); ++i)
    {
        treeWidget->insertTopLevelItem(i, list[i]);
    }

    emit initializedProgram(treeWidget);
}


