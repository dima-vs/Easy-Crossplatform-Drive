#include <QHash>
#include <QPair>
#include <list>
#include "file/service.h"

namespace Service::File
{

FileService::FileService(
    FileStorage &fileStorage,
    FileRepository& fileRep
    ) :
    m_fileStorage(fileStorage),
    m_fileRep(fileRep)
{}

TreeResult FileService::getFileTree(
    int userId,
    QVariant parentId,
    QVariant maxDepth
    )
{
    QPair<QList<::File>, QList<::File>> filesAndDirs;
    bool success = m_fileRep.getAllNestedObjects(
        userId,
        parentId,
        filesAndDirs,
        maxDepth
        );

    if (!success)
    {
        return TreeResult::fail(ServiceError::CannotGetFileTree);
    }

    bool isRequestedRootFixed = !parentId.isNull();
    int requestedRootId = isRequestedRootFixed ? parentId.toInt() : 0;

    std::list<Model::FileNode> roots;
    QHash<int, Model::FileNode*> lookupTable;

    QList<::File>& files = filesAndDirs.first;
    QList<::File>& dirs = filesAndDirs.second;

    for (int i = 0; i < dirs.size(); ++i)
    {
        ::File& fileObj = dirs[i];
        Model::FileNode fileNode;

        fileNode.id = fileObj.id();
        fileNode.logicalName = fileObj.logicalName();
        fileNode.size = fileObj.size();
        fileNode.type = fileObj.type();
        fileNode.uploadTime = fileObj.uploadTime();

        bool isRoot = (!isRequestedRootFixed && fileObj.parentId().isNull()) ||
                      (isRequestedRootFixed && (fileObj.id() == requestedRootId) );

        if (isRoot)
        {
            roots.push_back(fileNode);
            lookupTable[fileNode.id] = &roots.back();
        }
        else
        {
            int pId = fileObj.parentId().toInt();
            Model::FileNode* parentPtr = lookupTable.value(pId, nullptr);
            if (parentPtr)
            {
                parentPtr->children.push_back(fileNode);
                lookupTable[fileNode.id] = &parentPtr->children.back();
            }
        }
    }

    for (int i = 0; i < files.size(); ++i)
    {
        ::File& fileObj = files[i];
        Model::FileNode fileNode;

        fileNode.id = fileObj.id();
        fileNode.logicalName = fileObj.logicalName();
        fileNode.size = fileObj.size();
        fileNode.type = fileObj.type();
        fileNode.uploadTime = fileObj.uploadTime();

        bool isRoot = (!isRequestedRootFixed && fileObj.parentId().isNull()) ||
                      (isRequestedRootFixed && (fileObj.id() == requestedRootId) );

        if (isRoot)
        {
            roots.push_back(fileNode);
        }
        else
        {
            int pId = fileObj.parentId().toInt();
            Model::FileNode* parentPtr = lookupTable.value(pId, nullptr);
            if (parentPtr) {
                parentPtr->children.push_back(fileNode);
            }
        }
    }

    QList<Model::FileNode> result;
    for (const Model::FileNode& rootObj : roots)
    {
        result.append(rootObj);
    }

    return TreeResult::ok(result);
}


}
