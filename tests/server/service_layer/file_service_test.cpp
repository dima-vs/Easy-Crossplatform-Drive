#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <QString>
#include <QByteArray>
#include <QUuid>
#include <QDir>
#include <QVariant>
#include <QDateTime>
#include <QList>
#include <QtTypes>
#include <cmath>
#include <utility>
#include <optional>

#include "database_manager.h"
#include "user_repository.h"
#include "file_repository.h"
#include "file_storage.h"
#include "file/service.h"
#include "file/file_config.h"
#include "mock_time_provider.h"
#include "domain/file_type.h"

namespace Model = ::ServiceModel::File;
using Common::Domain::FileType;


class FileServiceTest : public testing::Test
{
protected:
    using ServiceError = ::ErrorCode::File::ServiceError;

    QString toString(ServiceError error)
    {
        switch (error)
        {
        case ServiceError::CannotGetFileTree:
            return QStringLiteral("CannotGetFileTree");
        case ServiceError::FileAlreadyExist:
            return QStringLiteral("FileAlreadyExist");
        case ServiceError::FileNotFound:
            return QStringLiteral("FileNotFound");
        case ServiceError::FileTooLarge:
            return QStringLiteral("FileTooLarge");
        case ServiceError::InvalidFileSize:
            return QStringLiteral("InvalidFileSize");
        case ServiceError::UuidAlreadyExists:
            return QStringLiteral("UuidAlreadyExists");
        case ServiceError::ChunkTooLarge:
            return QStringLiteral("ChunkTooLarge");
        case ServiceError::InvalidChunkSize:
            return QStringLiteral("InvalidChunkSize");
        case ServiceError::SessionDoesNotExists:
            return QStringLiteral("SessionDoesNotExists");
        case ServiceError::InvalidContentRange:
            return QStringLiteral("InvalidContentRange");
        case ServiceError::FailedToPerformDBOperation:
            return QStringLiteral("FailedToPerformDBOperation");
        case ServiceError::FailedToPerformStorageOperation:
            return QStringLiteral("FailedToPerformStorageOperation");
        case ServiceError::SessionIsNotCompleted:
            return QStringLiteral("SessionIsNotCompleted");
        case ServiceError::FileNotCreated:
            return QStringLiteral("FileNotCreated");
        case ServiceError::UploadSessionAlreadyCompleted:
            return QStringLiteral("UploadSessionAlreadyCompleted");
        case ServiceError::PermissionDenied:
            return QStringLiteral("PermissionDenied");
        case ServiceError::InvalidFileObjType:
            return QStringLiteral("InvalidFileObjType");
        case ServiceError::CannotOverwriteDirectory:
            return QStringLiteral("CannotOverwriteDirectory");
        }
        return QStringLiteral("UnknownError");
    }

    struct HierarchyNode
    {
        int id;
        FileType type;
        QString logicalName;
        QString physicalName = "";
        QByteArray storedContent = QByteArray();
        QList<HierarchyNode> children;

        HierarchyNode& operator<<(const HierarchyNode& child)
        {
            children.append(child);
            return *this;
        }
    };

    DatabaseManager m_DBManager;
    UserRepository m_userRep;
    FileStorage m_fileStorage;
    FileRepository m_fileRep;

    int m_currentUserId;
    QString m_currentUsername;

    QList<HierarchyNode> m_currentHierarchy;
    Model::CompleteUploadResult m_completedUpload;
    // 16 bytes by default
    QByteArray m_currentFileBytes = QByteArray("0123456789ABCDEF");

    testing::NiceMock<Tests::Mocks::MockTimeProvider> m_timeProvider;
    Service::File::FileService m_fileService;

    static Config::File::FileConfig getTestConfig()
    {
        Config::File::FileConfig config;
        config.upload.maxChunkSize = 5;
        config.download.maxChunkSize = 5;
        config.upload.maxFileSize = 1024 * 1024;
        return config;
    }

    FileServiceTest() :
        m_DBManager(":memory:", QUuid::createUuid().toString()),
        m_userRep(m_DBManager),
        m_fileStorage("test_storage_data"),
        m_fileRep(m_DBManager),
        m_fileService(
            m_fileStorage,
            m_fileRep,
            m_timeProvider,
            FileServiceTest::getTestConfig()
        )
    {
        m_timeProvider.m_manualTime = QDateTime::currentDateTimeUtc();
    }

    void SetUp() override
    {
        createAndSwitchUser("default_user");
    }

    void TearDown() override
    {
        QDir dir("test_storage_data");
        if (dir.exists())
        {
            dir.removeRecursively();
        }
    }

    int createUser(const QString& username)
    {
        UserRecord user(username, username + "@gmail.com", "some_hash");
        m_userRep.add(user);
        return user.id();
    }

    void switchUser(int userId, const QString& username)
    {
        m_currentUserId = userId;
        m_currentUsername = username;
    }

    int createAndSwitchUser(const QString& username)
    {
        int newId = createUser(username);
        switchUser(newId, username);
        return newId;
    }

    HierarchyNode makeFile(const QString& name, const QByteArray& data)
    {
        return HierarchyNode {
            0,
            FileType::File,
            name,
            "",
            data,
            QList<HierarchyNode>()
        };
    }

    HierarchyNode makeDir(const QString& name)
    {
        return HierarchyNode {
            0,
            FileType::Directory,
            name,
            "",
            QByteArray(),
            QList<HierarchyNode>()
        };
    }

    void initDeepHierarchyList()
    {
        /* Lvl
         *  1          Images                 Documents    root_file.bin
         *             /   \                  /   |   \
         *  2    icon.png  Animals    report.pdf Work Empty
         *                  /   \                 |
         *  3           Cats    Dogs          Statistics
         *              /        \               /  \
         *  4 black_cat.png    WhiteDogs    Tables  Graphs
         *                                    /      /   \
         *  5                           sales.xlsx Empty empty_file
        */

        QByteArray rootFileBytes = QByteArray("Root file binary data...");
        QByteArray iconBytes     = QByteArray("PNG_MAGIC_BYTES_ICON_DATA");
        QByteArray reportBytes   = QByteArray("PDF_MAGIC_BYTES_ANNUAL_REPORT");
        QByteArray blackCatBytes = QByteArray("black cat image.");
        QByteArray salesBytes    = QByteArray("EXCEL_DATA_SALES_2024");
        QByteArray emptyBytes    = QByteArray("");

        auto black_cat_png = makeFile("black_cat.png", blackCatBytes);
        auto sales_xlsx = makeFile("sales.xlsx", salesBytes);
        auto empty_file = makeFile("empty_file", emptyBytes);

        auto GraphsEmpty = makeDir("Empty");

        auto Cats = makeDir("Cats") << black_cat_png;
        auto WhiteDogs = makeDir("WhiteDogs");
        auto Tables = makeDir("Tables") << sales_xlsx;
        auto Graphs = makeDir("Graphs") << GraphsEmpty
                                        << empty_file;

        auto Dogs = makeDir("Dogs") << WhiteDogs;
        auto Statistics = makeDir("Statistics") << Tables
                                                << Graphs;

        auto Animals = makeDir("Animals") << Cats << Dogs;

        auto icon_png = makeFile("icon.png", iconBytes);
        auto report_pdf = makeFile("report.pdf", reportBytes);
        auto Work = makeDir("Work") << Statistics;
        auto DocumentsEmpty = makeDir("Empty");

        auto Images = makeDir("Images") << icon_png << Animals;
        auto Documents = makeDir("Documents") << report_pdf
                                              << Work
                                              << DocumentsEmpty;
        auto root_file_bin = makeFile("root_file.bin", rootFileBytes);

        m_currentHierarchy = {
            Images,
            Documents,
            root_file_bin
        };
    }

    void collectPhysicalNames(const QList<HierarchyNode>& nodes, QList<QString>& outNames)
    {
        for (const auto& node : nodes)
        {
            if (node.type == FileType::File)
            {
                FileRecord dbFile = m_fileRep.findById(node.id);
                if (dbFile.isValid())
                {
                    outNames.append(dbFile.serverName().toString());
                }
            }
            collectPhysicalNames(node.children, outNames);
        }
    }

    void createHierarchy(
        QList<HierarchyNode>& nodes,
        QVariant parentId = QVariant()
        )
    {
        for (HierarchyNode& node : nodes)
        {
            if (node.type == FileType::Directory)
            {
                auto res = m_fileService.createEmpty(
                    m_currentUserId,
                    m_currentUsername,
                    node.logicalName,
                    parentId,
                    FileType::Directory,
                    false
                    );
                ASSERT_TRUE(res.isOk()) <<
                    "failed to create directory: " <<
                    node.logicalName.toStdString();

                node.id = res.data().fileId;
                createHierarchy(node.children, node.id);
            }
            else if (node.type == FileType::File)
            {
                uploadFile(node.logicalName, parentId,
                           node.storedContent);
                node.id = m_completedUpload.fileId;
            }
        }
    }

    void assertTreesAreEqual(
        const QList<HierarchyNode>& expectedList,
        const QList<Model::FileNode>& actualList
        )
    {
        ASSERT_EQ(expectedList.size(), actualList.size()) <<
            "lists sizes mismatch";

        for (const auto& expected : expectedList)
        {
            auto it = std::find_if(actualList.begin(), actualList.end(),
                                   [&expected](const Model::FileNode& node) {
                                       return node.id == expected.id;
                                });

            ASSERT_NE(it, actualList.end()) <<
                "cannot find node with ID " << expected.id <<
                " (" << expected.logicalName.toStdString() << ")";

            const Model::FileNode& actual = *it;

            EXPECT_EQ(expected.type, actual.type) <<
                "type mismatch for " <<
                expected.logicalName.toStdString();
            EXPECT_EQ(expected.logicalName, actual.logicalName) <<
                "name mismatch for ID " << expected.id;

            if (expected.type == FileType::File)
            {
                EXPECT_EQ(expected.storedContent.size(), actual.size.toInt()) <<
                    "size mismatch for " << expected.logicalName.toStdString();
            }

            // convert std::list to QList
            QList<Model::FileNode> actualChildren;
            for(const auto& child : actual.children) {
                actualChildren.append(child);
            }

            assertTreesAreEqual(expected.children, actualChildren);
        }
    }

    void uploadFile(
        QString fileName,
        QVariant parentId,
        QByteArray fileBytes,
        bool overwrite = false
        )
    {
        qint64 fileSize = fileBytes.size();
        auto initRes = m_fileService.initUpload(
            m_currentUserId,
            m_currentUsername,
            fileName,
            parentId,
            fileSize,
            overwrite
            );

        if (fileSize == 0)
        {
            auto res = m_fileService.createEmpty(
                m_currentUserId,
                m_currentUsername,
                fileName,
                parentId,
                FileType::File,
                false
                );
            ASSERT_TRUE(res.isOk()) <<
                toString(res.error()).toStdString();

            m_completedUpload.fileId = res.data().fileId;
            m_completedUpload.fileName = res.data().fileName;
            m_completedUpload.parentId = res.data().parentId;
            m_completedUpload.size = 0;
            return;
        }

        ASSERT_TRUE(initRes.isOk()) <<
            toString(initRes.error()).toStdString();

        QString uploadId = initRes.data().uploadId;
        int chunkSize = initRes.data().chunkSize;

        int chunksCountNeeded = std::ceil(
            static_cast<double>(fileSize) / chunkSize
            );
        int lastChunkSize = fileSize % chunkSize;
        if (lastChunkSize == 0 && fileSize > 0)
        {
            lastChunkSize = chunkSize;
        }

        for (int i = 0; i < chunksCountNeeded; ++i)
        {
            bool isLastChunk = i == chunksCountNeeded-1;
            int startByte = i * chunkSize;
            int endByte = isLastChunk?
                              startByte + lastChunkSize - 1:
                              startByte + chunkSize - 1;

            Model::ContentRange range{startByte, endByte, fileSize};

            auto uploadRes = m_fileService.upload(
                uploadId, range, fileBytes.mid(startByte, chunkSize));
            ASSERT_TRUE(uploadRes.isOk()) <<
                toString(uploadRes.error()).toStdString() <<
                "\nstart byte: " << range.startByte <<
                "\nend byte: " << range.endByte <<
                "\ntotal bytes: " << range.totalBytes <<
                "\nis last chunk: " << isLastChunk;
        }

        auto completeRes = m_fileService.completeUpload(uploadId);
        ASSERT_TRUE(completeRes.isOk()) <<
            toString(completeRes.error()).toStdString();

        m_completedUpload = completeRes.data();
    }
};


TEST_F(FileServiceTest, SuccessfulMultiChunkUploadFlow)
{
    QString fileName = "multi_chunk.txt";
    // 12 bytes
    QByteArray fileData = "123456789012";
    qint64 fileSize = fileData.size();
    QVariant rootParentId;

    auto initRes = m_fileService.initUpload(
        m_currentUserId,
        m_currentUsername,
        fileName,
        rootParentId,
        fileSize,
        false
        );

    ASSERT_TRUE(initRes.isOk());
    QString uploadId = initRes.data().uploadId;
    EXPECT_EQ(initRes.data().chunkSize, 5) <<
        "chunk size should be from test config";

    // Chunk 1 (bytes 0-4) -> "12345"
    Model::ContentRange range1;
    range1.startByte = 0;
    range1.endByte = 4;
    range1.totalBytes = fileSize;
    auto uploadRes1 = m_fileService.upload(
        uploadId, range1, fileData.mid(0, 5));
    ASSERT_TRUE(uploadRes1.isOk()) << "chunk 1 failed";

    // Chunk 2 (bytes 5-9) -> "67890"
    Model::ContentRange range2;
    range2.startByte = 5;
    range2.endByte = 9;
    range2.totalBytes = fileSize;
    auto uploadRes2 = m_fileService.upload(
        uploadId, range2, fileData.mid(5, 5));
    ASSERT_TRUE(uploadRes2.isOk()) << "chunk 2 failed";

    // try to complete ahead of time
    auto earlyCompleteRes = m_fileService.completeUpload(uploadId);
    EXPECT_FALSE(earlyCompleteRes.isOk());
    EXPECT_EQ(earlyCompleteRes.error(), ServiceError::SessionIsNotCompleted);

    // Chunk 3 (bytes 10-11) -> "12"
    Model::ContentRange range3;
    range3.startByte = 10;
    range3.endByte = 11;
    range3.totalBytes = fileSize;
    auto uploadRes3 = m_fileService.upload(
        uploadId, range3, fileData.mid(10, 2));
    ASSERT_TRUE(uploadRes3.isOk()) << "chunk 3 failed";

    // complete upload
    auto completeRes = m_fileService.completeUpload(uploadId);
    ASSERT_TRUE(completeRes.isOk()) <<
        "failed to complete multi-chunk upload";

    FileRecord dbFile = m_fileRep.findById(completeRes.data().fileId);
    ASSERT_TRUE(dbFile.isValid());
    EXPECT_EQ(dbFile.size(), 12);

    QString serverFileName = dbFile.serverName().toString();
    EXPECT_TRUE(m_fileStorage.exists(serverFileName));
    EXPECT_EQ(m_fileStorage.getFileSize(serverFileName), fileSize);
}

TEST_F(FileServiceTest, UploadChunkFailsOnInvalidRanges)
{
    QString fileName = "bad_ranges.txt";
    QByteArray fileData = "1234567890"; // 10 bytes
    int fileSize = fileData.size();

    auto initRes = m_fileService.initUpload(
        m_currentUserId, m_currentUsername, fileName,
        QVariant(), fileSize, false
        );
    ASSERT_TRUE(initRes.isOk());
    QString uploadId = initRes.data().uploadId;
    int chunkSize = initRes.data().chunkSize;

    // 1. Try to send more bytes then chunkSize.
    // Has to be chunkSize - 1 to be correct.
    Model::ContentRange hugeRange{0, chunkSize, fileSize};
    auto uploadRes1 = m_fileService.upload(uploadId, hugeRange, fileData);
    EXPECT_FALSE(uploadRes1.isOk());
    EXPECT_EQ(uploadRes1.error(), ServiceError::ChunkTooLarge);

    // 2. Real size and specified size don't match.
    // Specify 5 bytes chunk [0-4]
    Model::ContentRange mismatchRange{0, chunkSize-1, fileSize};
    QByteArray shortData = "123"; // send 3 bytes
    auto uploadRes2 = m_fileService.upload(
        uploadId,
        mismatchRange,
        shortData);
    EXPECT_FALSE(uploadRes2.isOk());
    EXPECT_EQ(uploadRes2.error(), ServiceError::InvalidContentRange);

    // 3. Last chunk has an invalid size.
    // Total: 10 bytes -> expected chunks: [0-4] (5), [5-9] (5)
    // Try to send the second chunk as non-final (3 bytes instead of 5)
    Model::ContentRange badLastRange{5, 7, fileSize};
    auto uploadRes3 = m_fileService.upload(
        uploadId, badLastRange, fileData.mid(5, 3));
    EXPECT_FALSE(uploadRes3.isOk());
    EXPECT_EQ(uploadRes3.error(), ServiceError::InvalidChunkSize);
}

TEST_F(FileServiceTest, UploadChunkSkipsAlreadyUploadedChunk)
{
    QString fileName = "duplicate_chunk.txt";
    QByteArray fileData = "12345678";
    int fileSize = fileData.size();

    auto initRes = m_fileService.initUpload(
        m_currentUserId, m_currentUsername, fileName,
        QVariant(), fileData.size(), false
        );
    ASSERT_TRUE(initRes.isOk());

    QString uploadId = initRes.data().uploadId;
    int chunkSize = initRes.data().chunkSize;
    Model::ContentRange range{0, chunkSize-1, fileSize};

    auto res1 = m_fileService.upload(
        uploadId, range, fileData.mid(0, chunkSize));
    ASSERT_TRUE(res1.isOk()) <<
        toString(res1.error()).toStdString();

    // try to upload already uploaded chunk
    auto res2 = m_fileService.upload(
        uploadId, range, fileData.mid(0, chunkSize));
    // service should just skip upload and return OK
    EXPECT_TRUE(res2.isOk()) <<
        toString(res2.error()).toStdString();
}


TEST_F(FileServiceTest, DownloadChunksWorks)
{
    QString fileName = "data.bin";
    uploadFile(fileName, QVariant(), m_currentFileBytes);

    int downloadChunkSize = getTestConfig().download.maxChunkSize;
    QByteArray downloadedChunk;

    auto downloadRes = m_fileService.download(
        m_currentUserId,
        m_completedUpload.fileId,
        Model::RequestedRange { 0, downloadChunkSize-1 },
        downloadedChunk
        );

    ASSERT_TRUE(downloadRes.isOk()) <<
        toString(downloadRes.error()).toStdString();

    EXPECT_EQ(downloadedChunk, m_currentFileBytes.mid(0, downloadChunkSize));
}

TEST_F(FileServiceTest, DownloadChunkFailsOnInvalidRanges)
{
    QString fileName = "bad_download_ranges.bin";
    uploadFile(fileName, QVariant(), m_currentFileBytes);
    int fileId = m_completedUpload.fileId;
    int fileSize = m_currentFileBytes.size();

    QByteArray outBytes;

    // 1. negative startByte
    auto res1 = m_fileService.download(
        m_currentUserId, fileId, {-1, 4}, outBytes);
    EXPECT_FALSE(res1.isOk());
    EXPECT_EQ(res1.error(), ServiceError::InvalidContentRange);

    // 2. startByte > endByte
    auto res2 = m_fileService.download(
        m_currentUserId, fileId, {5, 4}, outBytes);
    EXPECT_FALSE(res2.isOk());
    EXPECT_EQ(res2.error(), ServiceError::InvalidContentRange);

    // 3. startByte >= fileSize
    auto res3 = m_fileService.download(
        m_currentUserId, fileId, {fileSize, fileSize + 4}, outBytes);
    EXPECT_FALSE(res3.isOk());
    EXPECT_EQ(res3.error(), ServiceError::InvalidContentRange);
}

TEST_F(FileServiceTest, DownloadChunkFailsOnExceedingLimits)
{
    QString fileName = "large_download_chunk.bin";
    uploadFile(fileName, QVariant(), m_currentFileBytes);
    int fileId = m_completedUpload.fileId;

    int downloadChunkSize = getTestConfig().download.maxChunkSize;
    QByteArray outBytes;

    // has to be downloadChunkSize-1 in order to be correct
    auto res = m_fileService.download(
        m_currentUserId, fileId, {0, downloadChunkSize}, outBytes);
    EXPECT_FALSE(res.isOk());
    EXPECT_EQ(res.error(), ServiceError::ChunkTooLarge);
}


TEST_F(FileServiceTest, GetTreeReturnsCorrectFullHierarchy)
{
    initDeepHierarchyList();
    createHierarchy(m_currentHierarchy);
    auto treeRes = m_fileService.getTree(
        m_currentUserId, QVariant(), QVariant());
    ASSERT_TRUE(treeRes.isOk()) <<
        toString(treeRes.error()).toStdString();

    assertTreesAreEqual(m_currentHierarchy, treeRes.data());
}

TEST_F(FileServiceTest, DownloadChunkFailsOnPermissionDeniedAndNotFound)
{
    // upload for "default_user"
    QString fileName = "secret_data.bin";
    uploadFile(fileName, QVariant(), m_currentFileBytes);
    int fileId = m_completedUpload.fileId;

    QByteArray outBytes;
    createAndSwitchUser("hacker_user");

    // 1. hacker tries to download someone else's file
    auto res1 = m_fileService.download(
        m_currentUserId, fileId, {0, 4}, outBytes);
    EXPECT_FALSE(res1.isOk());
    EXPECT_EQ(res1.error(), ServiceError::PermissionDenied);

    // 2. hacker tries to download non-existent file
    auto res2 = m_fileService.download(
        m_currentUserId, 999999, {0, 4}, outBytes);
    EXPECT_FALSE(res2.isOk());
    EXPECT_EQ(res2.error(), ServiceError::FileNotFound);
}

TEST_F(FileServiceTest, RemoveSingleFileWorks)
{
    QString fileName = "to_delete.bin";
    uploadFile(fileName, QVariant(), m_currentFileBytes);

    int fileId = m_completedUpload.fileId;
    FileRecord dbFile = m_fileRep.findById(fileId);
    QString serverFileName = dbFile.serverName().toString();

    EXPECT_TRUE(m_fileStorage.exists(serverFileName));

    auto res = m_fileService.removeEntry(m_currentUserId, fileId);
    ASSERT_TRUE(res.isOk()) << toString(res.error()).toStdString();

    EXPECT_FALSE(m_fileRep.findById(fileId).isValid());
    EXPECT_FALSE(m_fileStorage.exists(serverFileName));
}

TEST_F(FileServiceTest, RemoveDirectoryCascadesToPhysicalFilesInDeepHierarchy)
{
    initDeepHierarchyList();
    createHierarchy(m_currentHierarchy);

    HierarchyNode docsNode = m_currentHierarchy[1];
    EXPECT_EQ(docsNode.logicalName, "Documents");

    QList<QString> physicalNamesToDelete;
    collectPhysicalNames({docsNode}, physicalNamesToDelete);

    // must be 3 files (report, sales, empty_file)
    EXPECT_EQ(physicalNamesToDelete.size(), 3);

    for (const QString& serverName : physicalNamesToDelete)
    {
        EXPECT_TRUE(m_fileStorage.exists(serverName)) <<
            "file should exist before deletion: " << serverName.toStdString();
    }

    // remove Documents dir
    auto rmRes = m_fileService.removeEntry(m_currentUserId, docsNode.id);
    ASSERT_TRUE(rmRes.isOk()) << toString(rmRes.error()).toStdString();

    EXPECT_FALSE(m_fileRep.findById(docsNode.id).isValid());

    for (const QString& serverName : physicalNamesToDelete)
    {
        EXPECT_FALSE(m_fileStorage.exists(serverName));
    }

    // Images dir still has to exist
    HierarchyNode imagesNode = m_currentHierarchy[0];
    EXPECT_TRUE(m_fileRep.findById(imagesNode.id).isValid());
}

TEST_F(FileServiceTest, RemoveFailsOnPermissionDeniedAndNotFound)
{
    // upload file as a "default_user"
    uploadFile("secret_file.bin", QVariant(), m_currentFileBytes);
    int fileId = m_completedUpload.fileId;
    QString serverFileName = m_fileRep.findById(fileId).serverName().toString();

    createAndSwitchUser("hacker_user");

    // 1. hacker tries to remove someone else's file
    auto res1 = m_fileService.removeEntry(m_currentUserId, fileId);
    EXPECT_FALSE(res1.isOk());
    EXPECT_EQ(res1.error(), ServiceError::PermissionDenied);

    // file still has to exist
    EXPECT_TRUE(m_fileRep.findById(fileId).isValid());
    EXPECT_TRUE(m_fileStorage.exists(serverFileName));

    // 2. hacker tries to remove non-existent file
    auto res2 = m_fileService.removeEntry(m_currentUserId, 999999);
    EXPECT_FALSE(res2.isOk());
    EXPECT_EQ(res2.error(), ServiceError::FileNotFound);
}

TEST_F(FileServiceTest, RenameAndMoveWorks)
{
    auto dir1Res = m_fileService.createEmpty(
        m_currentUserId, m_currentUsername, "FolderA",
        QVariant(), FileType::Directory, false);
    auto dir2Res = m_fileService.createEmpty(
        m_currentUserId, m_currentUsername, "FolderB",
        QVariant(), FileType::Directory, false);

    int dir1Id = dir1Res.data().fileId;
    int dir2Id = dir2Res.data().fileId;

    uploadFile("document.txt", dir1Id, m_currentFileBytes);
    int fileId = m_completedUpload.fileId;

    // 1. rename
    auto renameRes = m_fileService.renameAndMove(
        m_currentUserId, fileId, std::nullopt, "secret_doc.txt"
        );
    ASSERT_TRUE(renameRes.isOk()) <<
        toString(renameRes.error()).toStdString();
    EXPECT_EQ(renameRes.data().logicalName, "secret_doc.txt");
    EXPECT_EQ(renameRes.data().parentId.toInt(), dir1Id);

    // 2. move
    auto moveRes = m_fileService.renameAndMove(
        m_currentUserId, fileId, QVariant(dir2Id), std::nullopt
        );
    ASSERT_TRUE(moveRes.isOk()) <<
        toString(moveRes.error()).toStdString();
    EXPECT_EQ(moveRes.data().parentId.toInt(), dir2Id);
    EXPECT_EQ(moveRes.data().logicalName, "secret_doc.txt");

    // 3. move to root
    auto rootRes = m_fileService.renameAndMove(
        m_currentUserId, fileId, QVariant(), "root_doc.txt"
        );
    ASSERT_TRUE(rootRes.isOk()) <<
        toString(rootRes.error()).toStdString();
    EXPECT_TRUE(rootRes.data().parentId.isNull());
    EXPECT_EQ(rootRes.data().logicalName, "root_doc.txt");
}

TEST_F(FileServiceTest, RenameAndMoveFailsOnCollision)
{
    auto dirRes = m_fileService.createEmpty(
        m_currentUserId, m_currentUsername, "Work",
        QVariant(), FileType::Directory, false);
    int dirId = dirRes.data().fileId;

    uploadFile("file1.txt", dirId, m_currentFileBytes);
    int file1Id = m_completedUpload.fileId;

    uploadFile("file2.txt", dirId, m_currentFileBytes);
    int file2Id = m_completedUpload.fileId;

    // try to rename file2 to file1
    auto collisionRes = m_fileService.renameAndMove(
        m_currentUserId, file2Id, std::nullopt, "file1.txt"
        );

    EXPECT_FALSE(collisionRes.isOk());
    EXPECT_EQ(collisionRes.error(), ServiceError::FileAlreadyExist);
}
