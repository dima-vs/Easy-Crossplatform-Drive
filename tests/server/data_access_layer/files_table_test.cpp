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
#include "user_record.h"
#include "file_record.h"
#include "domain/file_type.h"
#include "converter/file_type_converter.h"

using FileType = Common::Domain::FileType;
using FileTypeConverter = Common::Converter::FileTypeConverter;

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
        UserRecord user("testuser", "test@gmail.com", "12345");
        m_userRep.add(user);
        m_testUserId = user.id();
    }

    int createDir(
        const QString& name,
        QVariant parentId = QVariant(QMetaType::fromType<int>())
        )
    {
        FileRecord dir(m_testUserId, FileType::Directory, name, QVariant(), 0, parentId);
        if (m_fileRep.add(dir))
        {
            return dir.id();
        }
        return -1;
    }

    QString generateServerName(int dirDepth, int fileIdx=-1)
    {
        UserRecord user = m_userRep.findById(m_testUserId);
        if (!user.isValid())
            return QString();

        QString username = user.username();
        QStringList path = getFolderPath(dirDepth);
        QString result = username + "_" + path.join(".");

        if (fileIdx >= 0)
            result += QString(".SomeFile%1.png").arg(fileIdx);

        return result;
    }

    QString generateServerName(const QString& logicalName, QVariant parentId)
    {
        UserRecord user = m_userRep.findById(m_testUserId);
        if (!user.isValid())
            return QString();

        QString username = user.username();
        QString result = username + "_";

        if (parentId.isNull())
            return result + "NULL_" + logicalName;

        QStringList fullPath;
        int currentParentId = parentId.toInt();

        while (currentParentId != 0)
        {
            FileRecord dir = m_fileRep.findById(currentParentId);

            if (!dir.isValid())
                return "";

            fullPath.append(dir.logicalName());
            if (dir.parentId().isNull()) {
                break;
            }

            currentParentId = dir.parentId().toInt();
        }

        std::reverse(fullPath.begin(), fullPath.end());
        return result + fullPath.join(".") + "." + logicalName;
    }

    int createFile(const QString& name, int size,
                   QVariant parentId = QVariant(QMetaType::fromType<int>())
                   )
    {
        QString serverName = generateServerName(name, parentId);
        FileRecord file(m_testUserId, FileType::File, name, serverName, size, parentId);
        if (m_fileRep.add(file))
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
    EXPECT_EQ(m_fileRep.findByOwner(m_testUserId).size(), 0);

    // /Images
    int imgDirId = createDir("Images");
    ASSERT_GT(imgDirId, 0);

    // /Images/icon.png
    int iconId = createFile("icon.png", 2048, imgDirId);
    ASSERT_GT(iconId, 0);

    // /Images/Animals
    int animalsDirId = createDir("Animals", imgDirId);
    ASSERT_GT(animalsDirId, 0);
    // /Images/Animals/Cats
    int catsDirId = createDir("Cats", animalsDirId);
    ASSERT_GT(catsDirId, 0);

    // /Images/Animals/Cats/black_cat.png
    int catFileId = createFile("black_cat.png", 5120, catsDirId);
    ASSERT_GT(catFileId, 0);

    EXPECT_EQ(m_fileRep.findByOwner(m_testUserId).size(), 5);

    FileRecord fetchedCat = m_fileRep.findByPath(m_testUserId, { "Images", "Animals", "Cats", "black_cat.png" });
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
    FileRecord badFile(badUserId, FileType::File, "note.txt", QUuid::createUuid().toString(), 100);

    EXPECT_FALSE(m_fileRep.add(badFile));
}

TEST_F(FileRepositoryTest, DifferentUsersCanHaveSameFileNames)
{
    UserRecord user2("second_user", "second@gmail.com", "12345");
    ASSERT_TRUE(m_userRep.add(user2));
    int secondUserId = user2.id();

    int rootDocsUser1 = createDir("Documents");
    ASSERT_GT(rootDocsUser1, 0);

    FileRecord rootDocsUser2Obj(
        secondUserId, FileType::Directory, "Documents",
        QVariant(), 0,
        QVariant(QMetaType::fromType<int>())
        );
    EXPECT_TRUE(m_fileRep.add(rootDocsUser2Obj))
        << "Should allow second user to create a root folder with the same name";

    int rootDocsUser2 = rootDocsUser2Obj.id();
    ASSERT_GT(rootDocsUser2, 0);

    // both users create file "report.pdf" in their Documents dir
    // first user file
    int fileUser1 = createFile("report.pdf", 1024, rootDocsUser1);
    ASSERT_GT(fileUser1, 0);

    // second user file
    QString serverName2 = QUuid::createUuid().toString() + ".bin";
    FileRecord fileUser2Obj(
        secondUserId, FileType::File, "report.pdf",
        serverName2, 2048, rootDocsUser2);
    EXPECT_TRUE(m_fileRep.add(fileUser2Obj))
        << "Should allow second user to create a file with the same name in their own folder";

    EXPECT_EQ(m_fileRep.findByOwner(m_testUserId).size(), 2);
    EXPECT_EQ(m_fileRep.findByOwner(secondUserId).size(), 2);
}

TEST_F(FileRepositoryTest, RecursiveSelectionWorks)
{
    /* Lvl
     *  1          Images             Documents
     *             /   \                 /
     *  2    icon.png  Animals    report.pdf
     *                  /   \
     *  3           Cats    Dogs
     *              /        \
     *  4 black_cat.png    WhiteDogs
    */

    // /Images
    int imgDirId = createDir("Images");
    // /Documents
    int docDirId = createDir("Documents");
    // /Documents/report.pdf
    int reportFileId = createFile("report.pdf", 1246, docDirId);
    // /Images/icon.png
    int iconId = createFile("icon.png", 2048, imgDirId);
    // /Images/Animals
    int animalsDirId = createDir("Animals", imgDirId);
    // /Images/Animals/Cats
    int catsDirId = createDir("Cats", animalsDirId);
    // /Images/Animals/Dogs
    int dogsDirId = createDir("Dogs", animalsDirId);
    // /Images/Animals/Cats/black_cat.png
    int catFileId = createFile("black_cat.png", 5120, catsDirId);
    // /Images/Animals/Dogs/WhiteDogs
    int whiteDogsDirId = createDir("WhiteDogs", dogsDirId);

    QPair<QList<FileRecord>, QList<FileRecord>> result;
    bool success = false;

    // === TEST 1: null parent, maxDepth = inf ===
    result.first.clear();
    result.second.clear();
    success = m_fileRep.getAllNested(
        m_testUserId, QVariant(), result);
    EXPECT_TRUE(success);
    // icon.png, report.pdf, black_cat.png
    EXPECT_EQ(result.first.size(), 3);
    // Images, Documents, Animals, Cats, Dogs, WhiteDogs
    EXPECT_EQ(result.second.size(), 6);

    // check that directories are ordered from root to deepest
    QString dir0 = result.second[0].logicalName();
    QString dir1 = result.second[1].logicalName();
    EXPECT_TRUE((dir0 == "Images" && dir1 == "Documents") ||
                (dir0 == "Documents" && dir1 == "Images"))
        << "Level 1 directories are out of order!";

    EXPECT_EQ(result.second[2].logicalName(), "Animals")
        << "Level 2 directory is out of order!";

    QString dir3 = result.second[3].logicalName();
    QString dir4 = result.second[4].logicalName();
    EXPECT_TRUE((dir3 == "Cats" && dir4 == "Dogs") ||
                (dir3 == "Dogs" && dir4 == "Cats"))
        << "Level 3 directories are out of order!";

    EXPECT_EQ(result.second[5].logicalName(), "WhiteDogs")
        << "Level 4 directory is out of order!";

    // === TEST 2: null parent, maxDepth = 1 ===
    result.first.clear();
    result.second.clear();
    success = m_fileRep.getAllNested(
        m_testUserId, QVariant(), result, 1);
    EXPECT_TRUE(success);
    // Files: [NULL]
    EXPECT_EQ(result.first.size(), 0);
    // Directories: Images, Documents
    EXPECT_EQ(result.second.size(), 2);

    // === TEST 3: null parent, maxDepth = 2 ===
    result.first.clear();
    result.second.clear();
    success = m_fileRep.getAllNested(
        m_testUserId, QVariant(), result, 2);
    EXPECT_TRUE(success);
    // Files: icon.png, report.pdf
    EXPECT_EQ(result.first.size(), 2);
    // Directories: Images, Documents, Animals
    EXPECT_EQ(result.second.size(), 3);

    // === TEST 4: Images dir, maxDepth = 1 ===
    result.first.clear();
    result.second.clear();
    success = m_fileRep.getAllNested(
        m_testUserId, QVariant(imgDirId), result, 1);
    EXPECT_TRUE(success);
    // Files: icon.png, report.pdf
    EXPECT_EQ(result.first.size(), 0);
    // Directories: Images
    EXPECT_EQ(result.second.size(), 1);

    // === TEST 5: Images dir, maxDepth = 3 ===
    // Lvl 1: Images
    // Lvl 2: icon.png, Animals
    // Lvl 3: Cats, Dogs
    result.first.clear();
    result.second.clear();
    success = m_fileRep.getAllNested(
        m_testUserId, QVariant(imgDirId), result, 3);
    EXPECT_TRUE(success);
    EXPECT_EQ(result.first.size(), 1);
    EXPECT_EQ(result.second.size(), 4);

    // === TEST 6: Empty directory WhiteDogs, maxDepth = inf ===
    result.first.clear(); result.second.clear();
    success = m_fileRep.getAllNested(
        m_testUserId, QVariant(whiteDogsDirId), result);
    EXPECT_TRUE(success);
    EXPECT_EQ(result.first.size(), 0);
    EXPECT_EQ(result.second.size(), 1);
}

TEST_F(FileRepositoryTest, DeleteFileWorks)
{
    int imgDirId = createDir("Images");
    createFile("photo1.jpg", 1000, imgDirId);
    createFile("photo2.jpg", 2000, imgDirId);

    EXPECT_EQ(m_fileRep.findByOwner(m_testUserId).size(), 3);

    EXPECT_TRUE(m_fileRep.remove(m_testUserId, {"Images", "photo1.jpg"}));
    EXPECT_EQ(m_fileRep.findByOwner(m_testUserId).size(), 2);

    EXPECT_FALSE(m_fileRep.remove(m_testUserId, {"Images", "not_exist.jpg"}));
}

TEST_F(FileRepositoryTest, RecursiveDeleteFileWorks)
{
    fillRecursive(5, 3);
    // === 5x3 files and 5 folders ===
    int currentObjCount = 5 * 3 + 5;
    EXPECT_EQ(m_fileRep.findByOwner(m_testUserId).size(), currentObjCount);

    // === delete file inside a folder at depth 5 ===
    QList<QString> deepFilePath = getFolderPath(5);
    deepFilePath.append("SomeFile1.png");

    QList<QString> serverNamesToDeleteOut;
    int totalObjectsDeleted = -1;
    bool lastOpSuccess = m_fileRep.remove(
        m_testUserId,
        deepFilePath,
        serverNamesToDeleteOut,
        &totalObjectsDeleted
        );

    ASSERT_TRUE(lastOpSuccess);
    int objectsMustBeDeleted = 1;
    ASSERT_EQ(serverNamesToDeleteOut.size(), 1);
    EXPECT_EQ(totalObjectsDeleted, objectsMustBeDeleted);

    QString serverNameFromRep = serverNamesToDeleteOut[0];
    QString generatedServerName = generateServerName(5, 1);
    EXPECT_EQ(serverNameFromRep, generatedServerName)
        << "Reference: " << serverNameFromRep.toStdString()
        << "\n:Generated" << generatedServerName.toStdString();

    currentObjCount -= 1; // 1 file deleted
    EXPECT_EQ(m_fileRep.findByOwner(m_testUserId).size(), currentObjCount);

    // === delete folder at depth 3 ===
    QList<QString> midFolderPath = getFolderPath(3);

    serverNamesToDeleteOut.clear();
    lastOpSuccess = m_fileRep.remove(
        m_testUserId,
        midFolderPath,
        serverNamesToDeleteOut,
        &totalObjectsDeleted
        );
    ASSERT_TRUE(lastOpSuccess);

    // deleted 3 folders, each of which (except 5th one) have 3 files
    // 5th folder have 2, because we deleted 1 file before
    objectsMustBeDeleted = 3 + 2 * 3 + 2;
    currentObjCount -= objectsMustBeDeleted;
    // count only files deleted
    ASSERT_EQ(serverNamesToDeleteOut.size(), 2 * 3 + 2);
    EXPECT_EQ(m_fileRep.findByOwner(m_testUserId).size(), currentObjCount);
    EXPECT_EQ(totalObjectsDeleted, objectsMustBeDeleted);

    for (int i = 3; i <= 5; ++i)
    {
        for (int j = 1; j <= 3; ++j)
        {
            if ((i == 5) && (j == 1))
                continue;
            generatedServerName = generateServerName(i, j);
            EXPECT_TRUE(serverNamesToDeleteOut.contains(generatedServerName));
        }
    }

    // === remove root folder ===
    QList<QString> rootFolderPath = getFolderPath(1);
    serverNamesToDeleteOut.clear();
    lastOpSuccess = m_fileRep.remove(
        m_testUserId,
        rootFolderPath,
        serverNamesToDeleteOut,
        &totalObjectsDeleted
        );
    ASSERT_TRUE(lastOpSuccess);
    // deleted 2 folders, each of which have 3 files
    objectsMustBeDeleted = 2 + 2 * 3;
    EXPECT_EQ(totalObjectsDeleted, objectsMustBeDeleted);

    // user must not have any objects anymore
    EXPECT_EQ(m_fileRep.findByOwner(m_testUserId).size(), 0);
}


TEST_F(FileRepositoryTest, DeleteByIdWorks)
{
    // /Photos
    int photosDirId = createDir("Photos");
    ASSERT_GT(photosDirId, 0);

    // /Photos/pic1.png
    int pic1Id = createFile("pic1.png", 1000, photosDirId);
    ASSERT_GT(pic1Id, 0);

    // /Photos/pic2.png
    int pic2Id = createFile("pic2.png", 2000, photosDirId);
    ASSERT_GT(pic2Id, 0);

    // /Photos/Vacation
    int vacDirId = createDir("Vacation", photosDirId);
    ASSERT_GT(vacDirId, 0);

    // /Photos/Vacation/sea.png
    int seaId = createFile("sea.png", 3000, vacDirId);
    ASSERT_GT(seaId, 0);

    // Photos + pic1 + pic2 + Vacation + sea = 5
    EXPECT_EQ(m_fileRep.findByOwner(m_testUserId).size(), 5);

    EXPECT_TRUE(m_fileRep.remove(m_testUserId, pic1Id));
    EXPECT_EQ(m_fileRep.findByOwner(m_testUserId).size(), 4)
        << "Total count should decrease by 1";

    // security check
    int hackerUserId = 999;
    EXPECT_FALSE(m_fileRep.remove(hackerUserId, pic2Id))
        << "Should fail if the user is not the owner of the file";

    EXPECT_EQ(m_fileRep.findByOwner(m_testUserId).size(), 4);

    QList<QString> deletedPhysicalFiles;
    int deletedCount = 0;

    EXPECT_TRUE(m_fileRep.remove(m_testUserId, vacDirId, deletedPhysicalFiles, &deletedCount));

    EXPECT_EQ(deletedCount, 2);
    EXPECT_EQ(deletedPhysicalFiles.size(), 1);

    EXPECT_EQ(m_fileRep.findByOwner(m_testUserId).size(), 2);

    EXPECT_FALSE(m_fileRep.remove(m_testUserId, 999999))
        << "Should return false for non-existent ID";
}
