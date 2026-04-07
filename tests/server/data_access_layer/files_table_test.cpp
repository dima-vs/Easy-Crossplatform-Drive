#include <gtest/gtest.h>
#include <QUuid>
#include <QString>
#include <QList>
#include <QVariant>
#include <QMetaType>
#include <QStringList>

#include "database_manager.h"
#include "user_repository.h"
#include "file_repository.h"
#include "user.h"
#include "file.h"

class FileRepositoryTest : public testing::Test
{
protected:
    DatabaseManager m_DBManager;
    UserRepository m_userRep;
    FileRepository m_fileRep;

    int m_testUserId;

    FileRepositoryTest() :
        m_DBManager(":memory:", QUuid::createUuid().toString()),
        m_userRep(m_DBManager),
        m_fileRep(m_DBManager)
    {
        User user("testuser", "test@gmail.com", "12345");
        m_userRep.addNewUser(user);
        m_testUserId = user.id();
    }

    int createDir(
        const QString& name,
        QVariant parentId = QVariant(QMetaType::fromType<int>())
        )
    {
        File dir(m_testUserId, "directory", name, QVariant(), 0, parentId);
        if (m_fileRep.addNewFile(dir))
        {
            return dir.id();
        }
        return -1;
    }

    int createFile(const QString& name, int size,
                   QVariant parentId = QVariant(QMetaType::fromType<int>())
                   )
    {
        QString serverName = QUuid::createUuid().toString() + ".bin";
        File file(m_testUserId, "file", name, serverName, size, parentId);
        if (m_fileRep.addNewFile(file))
        {
            return file.id();
        }
        return -1;
    }

    void fillRecursive(int depth, int filesPerFolder)
    {
        int previousId = -1;
        // [depth] recursive folders
        // /SomeFolder1/SomeFolder2/.../SomeFolder[depth]/
        for (int i = 1; i <= depth; ++i)
        {
            QVariant parentId = (previousId == -1) ?
                                    QVariant(QMetaType::fromType<int>()) :
                                    QVariant(previousId);
            previousId = createDir(QString("SomeFolder%1").arg(i), parentId);

            /*
             * for each folder also insert [filesPerFolder] files
             * thus every folder (except the deepest one)
             * will have following children:
             *
             * -- SomeFolder[i]/
             * -- SomeFile%1.png/
             * -- SomeFile%2.png
             *    ...
             * -- SomeFile%[filesPerFolder].png
             *
            */
            for (int j = 1; j <= filesPerFolder; ++j)
            {
                createFile(QString("SomeFile%1.png").arg(j), j * 100, previousId);
            }
        }
    }

    QList<QString> getFolderPath(int depthIndex) const
    {
        QList<QString> path;
        for (int i = 1; i <= depthIndex; ++i)
        {
            path.append(QString("SomeFolder%1").arg(i));
        }
        return path;
    }
};


TEST_F(FileRepositoryTest, HierarchyStorageWorks)
{
    EXPECT_EQ(m_fileRep.getFilesByOwner(m_testUserId).size(), 0);

    // /Images
    int imgDirId = createDir("Images");
    ASSERT_GT(imgDirId, 0);

    // /Images/icon.png
    int iconId = createFile("icon.png", 2048, imgDirId);
    ASSERT_GT(iconId, 0);

    // /Images/Animals
    int animalsDirId = createDir("Animals", imgDirId);
    // /Images/Animals/Cats
    int catsDirId = createDir("Cats", animalsDirId);

    // /Images/Animals/Cats/black_cat.png
    int catFileId = createFile("black_cat.png", 5120, catsDirId);
    ASSERT_GT(catFileId, 0);

    EXPECT_EQ(m_fileRep.getFilesByOwner(m_testUserId).size(), 5);

    File fetchedCat = m_fileRep.getFile(m_testUserId, { "Images", "Animals", "Cats", "black_cat.png" });
    EXPECT_EQ(fetchedCat.id(), catFileId);
}

TEST_F(FileRepositoryTest, UniqueConstraint)
{
    int rootDocsId = createDir("Documents");
    ASSERT_GT(rootDocsId, 0);

    int subDocsId = createDir("Work", rootDocsId);
    ASSERT_GT(subDocsId, 0);

    // duplicates inside root dir
    EXPECT_EQ(createDir("Work", rootDocsId), -1)
        << "Allowed duplicate inside a folder!";

    // creation of dir with repeated name but different parent is allowed
    // /Documents/Work/Documents
    EXPECT_GT(createDir("Documents", subDocsId), 0)
        << "Should allow 'Documents' inside '/Documents/Work/'";
}

TEST_F(FileRepositoryTest, NullParentIdUniqueConstraint)
{
    int rootDocsId = createDir("Documents");
    ASSERT_GT(rootDocsId, 0);

    // root duplicate test
    EXPECT_EQ(createDir("Documents"), -1)
        << "Allowed duplicate directory in root!";
}

TEST_F(FileRepositoryTest, FileAndDirNamesUniqueConstraint)
{
    int rootDocsId = createDir("Documents");
    ASSERT_GT(rootDocsId, 0);

    // === testing for root ===
    EXPECT_EQ(createFile("Documents", 1024), -1)
        << "Allowed file and dir with same name in root!";

    // === testing for specific directory ===
    int workDir = createDir("Work", rootDocsId);
    ASSERT_GT(workDir, 0);

    EXPECT_EQ(createFile("Work", 256, rootDocsId), -1)
        << "Allowed file and dir with same name in some dir!";
}

TEST_F(FileRepositoryTest, ForeignKeyConstraintFailsOnBadUser)
{
    int badUserId = 9999;
    File badFile(badUserId, "file", "note.txt", QUuid::createUuid().toString(), 100);

    EXPECT_FALSE(m_fileRep.addNewFile(badFile));
}

TEST_F(FileRepositoryTest, DifferentUsersCanHaveSameFileNames)
{
    User user2("second_user", "second@gmail.com", "12345");
    ASSERT_TRUE(m_userRep.addNewUser(user2));
    int secondUserId = user2.id();

    int rootDocsUser1 = createDir("Documents");
    ASSERT_GT(rootDocsUser1, 0);

    File rootDocsUser2Obj(
        secondUserId,"directory", "Documents",
        QVariant(), 0,
        QVariant(QMetaType::fromType<int>())
        );
    EXPECT_TRUE(m_fileRep.addNewFile(rootDocsUser2Obj))
        << "Should allow second user to create a root folder with the same name";

    int rootDocsUser2 = rootDocsUser2Obj.id();
    ASSERT_GT(rootDocsUser2, 0);

    // both users create file "report.pdf" in their Documents dir
    // first user file
    int fileUser1 = createFile("report.pdf", 1024, rootDocsUser1);
    ASSERT_GT(fileUser1, 0);

    // second user file
    QString serverName2 = QUuid::createUuid().toString() + ".bin";
    File fileUser2Obj(
        secondUserId, "file", "report.pdf",
        serverName2, 2048, rootDocsUser2);
    EXPECT_TRUE(m_fileRep.addNewFile(fileUser2Obj))
        << "Should allow second user to create a file with the same name in their own folder";

    EXPECT_EQ(m_fileRep.getFilesByOwner(m_testUserId).size(), 2);
    EXPECT_EQ(m_fileRep.getFilesByOwner(secondUserId).size(), 2);
}

TEST_F(FileRepositoryTest, DeleteFileWorks)
{
    int imgDirId = createDir("Images");
    createFile("photo1.jpg", 1000, imgDirId);
    createFile("photo2.jpg", 2000, imgDirId);

    EXPECT_EQ(m_fileRep.getFilesByOwner(m_testUserId).size(), 3);

    EXPECT_TRUE(m_fileRep.deleteFile(m_testUserId, {"Images", "photo1.jpg"}));
    EXPECT_EQ(m_fileRep.getFilesByOwner(m_testUserId).size(), 2);

    EXPECT_FALSE(m_fileRep.deleteFile(m_testUserId, {"Images", "not_exist.jpg"}));
}

TEST_F(FileRepositoryTest, RecursiveDeleteFileWorks)
{
    fillRecursive(30, 10);
    // 10x30 files and 30 folders
    int currentObjCount = 10 * 30 + 30;
    EXPECT_EQ(m_fileRep.getFilesByOwner(m_testUserId).size(), currentObjCount);

    QList<QString> deepFilePath = getFolderPath(30);
    deepFilePath.append("SomeFile1.png");

    EXPECT_TRUE(m_fileRep.deleteFile(m_testUserId, deepFilePath));
    currentObjCount -= 1; // 1 file deleted
    EXPECT_EQ(m_fileRep.getFilesByOwner(m_testUserId).size(), currentObjCount);

    // delete folder at depth 15
    QList<QString> midFolderPath = getFolderPath(15);
    EXPECT_TRUE(m_fileRep.deleteFile(m_testUserId, midFolderPath));

    // deleted 16 folders, each of which (except 30th one) have 10 files
    // 30th folder have 9, because we deleted 1 file before
    currentObjCount -= 16 + 15 * 10 + 9;
    EXPECT_EQ(m_fileRep.getFilesByOwner(m_testUserId).size(), currentObjCount);

    // remove root folder
    QList<QString> rootFolderPath = getFolderPath(1);
    EXPECT_TRUE(m_fileRep.deleteFile(m_testUserId, rootFolderPath));

    // user must not have any objects anymore
    EXPECT_EQ(m_fileRep.getFilesByOwner(m_testUserId).size(), 0);
}
