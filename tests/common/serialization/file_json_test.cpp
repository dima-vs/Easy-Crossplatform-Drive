#include <gtest/gtest.h>
#include <QJsonObject>
#include <QJsonArray>
#include <QVariant>
#include <optional>

#include "serialization/file/json.h"

TEST(FileJsonTest, CreateEmptyRequest_RoundTrip)
{
    // 1. Init
    DTO::File::CreateEmptyRequest original;
    original.fileName = "new_folder";
    original.parentId = 15;
    original.type = Common::Domain::FileType::Directory;
    original.overwrite = false;

    // 2. Serialize
    QJsonObject json = Serialization::File::toJson(original);

    // 3. Deserialize
    auto restoredOpt = Serialization::File
        ::fromJsonCreateEmptyRequest(json);

    // 4. Check
    ASSERT_TRUE(restoredOpt.has_value());
    EXPECT_EQ(restoredOpt->fileName, original.fileName);
    EXPECT_EQ(restoredOpt->parentId, original.parentId);
    EXPECT_EQ(restoredOpt->type, original.type);
    EXPECT_EQ(restoredOpt->overwrite, original.overwrite);
}

TEST(FileJsonTest, CreateEmptyResponse_RoundTrip)
{
    // 1. Init
    DTO::File::CreateEmptyResponse original;
    original.createdAt = QDateTime::currentDateTime().toUTC();
    original.fileId = 99;
    original.fileName = "data.csv";
    original.parentId = std::nullopt;

    // 2. Serialize
    QJsonObject json = Serialization::File::toJson(original);

    // 3. Deserialize
    auto restoredOpt = Serialization::File
        ::fromJsonCreateEmptyResponse(json);

    // 4. Check
    ASSERT_TRUE(restoredOpt.has_value());
    EXPECT_EQ(restoredOpt->fileId, original.fileId);
    EXPECT_EQ(restoredOpt->fileName, original.fileName);
    EXPECT_FALSE(restoredOpt->parentId.has_value());

    qint64 restoredSec = restoredOpt->createdAt.toSecsSinceEpoch();
    qint64 originalSec = original.createdAt.toSecsSinceEpoch();
    EXPECT_EQ(restoredSec, originalSec);
}

TEST(FileJsonTest, MetadataResponse_RoundTrip)
{
    // 1. Init
    DTO::File::MetadataResponse original;
    original.createdAt = QDateTime::currentDateTime().toUTC();
    original.type = Common::Domain::FileType::File;
    original.fileId = 404;
    original.fileName = "config.json";
    original.parentId = 12;
    original.size = 2048;

    // 2. Serialize
    QJsonObject json = Serialization::File::toJson(original);

    // 3. Deserialize
    auto restoredOpt = Serialization::File
        ::fromJsonMetadataResponse(json);

    // 4. Check
    ASSERT_TRUE(restoredOpt.has_value());
    EXPECT_EQ(restoredOpt->fileId, original.fileId);
    EXPECT_EQ(restoredOpt->fileName, original.fileName);
    EXPECT_EQ(restoredOpt->parentId, original.parentId);
    EXPECT_EQ(restoredOpt->size, original.size);
    EXPECT_EQ(restoredOpt->type, original.type);

    qint64 restoredSec = restoredOpt->createdAt.toSecsSinceEpoch();
    qint64 originalSec = original.createdAt.toSecsSinceEpoch();
    EXPECT_EQ(restoredSec, originalSec);
}

TEST(FileJsonTest, RenameRequest_MoveToRoot_RoundTrip)
{
    // 1. Init
    // move to root (newParentId = null in JSON)
    DTO::File::RenameRequest original;
    original.newFileName = "new_name.txt";
    original.newParentId = QVariant();

    // 2. Serialize
    QJsonObject json = Serialization::File::toJson(original);

    EXPECT_TRUE(json["newParentId"].isNull());

    // 3. Deserialize
    auto restoredOpt = Serialization::File::fromJsonRenameRequest(json);

    // 4. Check
    ASSERT_TRUE(restoredOpt.has_value());
    EXPECT_EQ(restoredOpt->newFileName, original.newFileName);
    ASSERT_TRUE(restoredOpt->newParentId.has_value());
    EXPECT_TRUE(restoredOpt->newParentId->isNull());
}

TEST(FileJsonTest, RenameRequest_OnlyRename_RoundTrip)
{
    // 1. Init
    // rename only (no parentId in JSON)
    DTO::File::RenameRequest original;
    original.newFileName = "only_rename.txt";
    original.newParentId = std::nullopt;

    // 2. Serialize
    QJsonObject json = Serialization::File::toJson(original);
    EXPECT_FALSE(json.contains("newParentId"));

    // 3. Deserialize
    auto restoredOpt = Serialization::File::fromJsonRenameRequest(json);

    // 4. Check
    ASSERT_TRUE(restoredOpt.has_value());
    EXPECT_FALSE(restoredOpt->newParentId.has_value());
    EXPECT_EQ(restoredOpt->newFileName, "only_rename.txt");
}

TEST(FileJsonTest, RenameResponse_RoundTrip)
{
    // 1. Init
    DTO::File::RenameResponse original;
    original.fileId = 55;
    original.fileName = "renamed_file.md";
    original.parentId = 7;

    // 2. Serialize
    QJsonObject json = Serialization::File::toJson(original);

    // 3. Deserialize
    auto restoredOpt = Serialization::File
        ::fromJsonRenameResponse(json);

    // 4. Check
    ASSERT_TRUE(restoredOpt.has_value());
    EXPECT_EQ(restoredOpt->fileId, original.fileId);
    EXPECT_EQ(restoredOpt->fileName, original.fileName);
    EXPECT_EQ(restoredOpt->parentId, original.parentId);
}

TEST(FileJsonTest, TreeNodeResponse_DeepTree_RoundTrip)
{
    // 1. Init: MyFolder -> document.pdf
    DTO::File::TreeNodeResponse file;
    file.fileId = 101;
    file.name = "document.pdf";
    file.isDirectory = false;
    file.size = 5000;

    DTO::File::TreeNodeResponse folder;
    folder.fileId = 100;
    folder.name = "MyFolder";
    folder.isDirectory = true;
    folder.children = QList<DTO::File::TreeNodeResponse>{file};

    // 2. Serialize
    QJsonObject json = Serialization::File::toJson(folder);

    // 3. Deserialize
    auto restoredOpt = Serialization::File
        ::fromJsonTreeNodeResponse(json);

    // 4. Check
    ASSERT_TRUE(restoredOpt.has_value());
    const auto& restored = restoredOpt.value();

    EXPECT_EQ(restored.fileId, folder.fileId);
    EXPECT_EQ(restored.name, folder.name);
    EXPECT_FALSE(restored.size.has_value());
    ASSERT_TRUE(restored.children.has_value());
    ASSERT_EQ(restored.children->size(), 1);

    const auto& restoredChild = restored.children->at(0);
    EXPECT_EQ(restoredChild.fileId, file.fileId);
    EXPECT_EQ(restoredChild.size, file.size);
    EXPECT_FALSE(restoredChild.children.has_value());
}

TEST(FileJsonTest, TreeNodeArray_DeepTree_RoundTrip)
{
    // 1. Init:
    //   RootFolder (ID: 1, Dir)
    //    ├── doc.pdf (ID: 2, File)
    //    └── SubFolder (ID: 3, Dir)
    //           └── deep_secret.txt (ID: 4, File)
    //   root_file.txt (ID: 5, File)

    // node 4
    DTO::File::TreeNodeResponse grandchild;
    grandchild.fileId = 4;
    grandchild.name = "deep_secret.txt";
    grandchild.isDirectory = false;
    grandchild.size = 200;

    // node 3
    DTO::File::TreeNodeResponse childFolder;
    childFolder.fileId = 3;
    childFolder.name = "SubFolder";
    childFolder.isDirectory = true;
    childFolder.children = QList<DTO::File::TreeNodeResponse> {
        grandchild
    };

    // node 2
    DTO::File::TreeNodeResponse childFile;
    childFile.fileId = 2;
    childFile.name = "doc.pdf";
    childFile.isDirectory = false;
    childFile.size = 100;

    // node 1
    DTO::File::TreeNodeResponse rootFolder;
    rootFolder.fileId = 1;
    rootFolder.name = "RootFolder";
    rootFolder.isDirectory = true;
    rootFolder.children = QList<DTO::File::TreeNodeResponse> {
        childFile, childFolder
    };

    // node 5
    DTO::File::TreeNodeResponse rootFile;
    rootFile.fileId = 5;
    rootFile.name = "root_file.txt";
    rootFile.isDirectory = false;
    rootFile.size = 300;

    QList<DTO::File::TreeNodeResponse> originalTree = {
        rootFolder, rootFile
    };

    // 2. Serialize
    QJsonArray jsonArray = Serialization::File::toJson(originalTree);

    // 3. Deserialize
    auto restoredOpt = Serialization::File
        ::fromJsonTreeNodeArray(jsonArray);

    // 4. Check
    ASSERT_TRUE(restoredOpt.has_value());
    const QList<DTO::File::TreeNodeResponse>& restoredTree =
        restoredOpt.value();

    // check root objects (depth=0)
    ASSERT_EQ(restoredTree.size(), 2);
    EXPECT_EQ(restoredTree[0].name, "RootFolder");
    EXPECT_EQ(restoredTree[1].name, "root_file.txt");

    // check depth=1 objects
    ASSERT_TRUE(restoredTree[0].children.has_value());
    ASSERT_EQ(restoredTree[0].children->size(), 2);
    EXPECT_EQ(restoredTree[0].children->at(0).name, "doc.pdf");
    EXPECT_EQ(restoredTree[0].children->at(1).name, "SubFolder");

    // check depth=2 objects
    ASSERT_TRUE(restoredTree[0].children->at(1).children.has_value());
    ASSERT_EQ(restoredTree[0].children->at(1).children->size(), 1);
    DTO::File::TreeNodeResponse deepSecretFile =
            restoredTree[0].children->at(1)
            .children->at(0);
    EXPECT_EQ(deepSecretFile.name, "deep_secret.txt");
    EXPECT_EQ(deepSecretFile.size, 200);
}

TEST(FileJsonTest, UploadCompleteResponse_RoundTrip)
{
    // 1. Init
    DTO::File::UploadCompleteResponse original;
    original.fileId = 777;
    original.fileName = "finished.exe";
    original.parentId = std::nullopt; // root
    original.size = 12345;
    original.createdAt = QDateTime::currentDateTime().toUTC();

    // 2. Serialize
    QJsonObject json = Serialization::File::toJson(original);

    // 3. Deserialize
    auto restoredOpt = Serialization::File
        ::fromJsonUploadCompleteResponse(json);

    // 4. Check
    ASSERT_TRUE(restoredOpt.has_value());
    EXPECT_EQ(restoredOpt->fileId, original.fileId);

    qint64 restoredSec = restoredOpt->createdAt.toSecsSinceEpoch();
    qint64 originalSec = original.createdAt.toSecsSinceEpoch();
    EXPECT_EQ(restoredSec, originalSec);

    EXPECT_FALSE(restoredOpt->parentId.has_value());
}

TEST(FileJsonTest, UploadInitRequest_RoundTrip)
{
    // 1. Init
    DTO::File::UploadInitRequest original;
    original.fileName = "photo.jpg";
    original.fileSize = 1024 * 1024 * 5;
    original.parentId = 42;
    original.overwrite = true;

    // 2. Serialize
    QJsonObject json = Serialization::File::toJson(original);

    // 3. Deserialize
    auto restoredOpt = Serialization::File
        ::fromJsonUploadInitRequest(json);

    // 4. Check
    ASSERT_TRUE(restoredOpt.has_value());
    EXPECT_EQ(restoredOpt->fileName, original.fileName);
    EXPECT_EQ(restoredOpt->fileSize, original.fileSize);
    EXPECT_EQ(restoredOpt->parentId, original.parentId);
    EXPECT_EQ(restoredOpt->overwrite, original.overwrite);
}

TEST(FileJsonTest, UploadInitResponse_RoundTrip)
{
    // 1. Init
    DTO::File::UploadInitResponse original;
    original.chunkSize = 1048576;
    original.uploadId = "abc-123-xyz";
    original.expiresAt = QDateTime::currentDateTime().addDays(1).toUTC();

    // 2. Serialize
    QJsonObject json = Serialization::File::toJson(original);

    // 3. Deserialize
    auto restoredOpt = Serialization::File
        ::fromJsonUploadInitResponse(json);

    // 4. Check
    ASSERT_TRUE(restoredOpt.has_value());
    EXPECT_EQ(restoredOpt->chunkSize, original.chunkSize);
    EXPECT_EQ(restoredOpt->uploadId, original.uploadId);

    qint64 restoredSec = restoredOpt->expiresAt.toSecsSinceEpoch();
    qint64 originalSec = original.expiresAt.toSecsSinceEpoch();
    EXPECT_EQ(restoredSec, originalSec);
}

TEST(FileJsonTest, EmptyJson_ReturnsNullopt)
{
    QJsonObject emptyJson;

    EXPECT_FALSE(Serialization::File::fromJsonCreateEmptyRequest(emptyJson).has_value());
    EXPECT_FALSE(Serialization::File::fromJsonCreateEmptyResponse(emptyJson).has_value());
    EXPECT_FALSE(Serialization::File::fromJsonMetadataResponse(emptyJson).has_value());
    EXPECT_FALSE(Serialization::File::fromJsonRenameRequest(emptyJson).has_value());
    EXPECT_FALSE(Serialization::File::fromJsonRenameResponse(emptyJson).has_value());
    EXPECT_FALSE(Serialization::File::fromJsonTreeNodeResponse(emptyJson).has_value());
    EXPECT_FALSE(Serialization::File::fromJsonUploadCompleteResponse(emptyJson).has_value());
    EXPECT_FALSE(Serialization::File::fromJsonUploadInitRequest(emptyJson).has_value());
    EXPECT_FALSE(Serialization::File::fromJsonUploadInitResponse(emptyJson).has_value());
}
