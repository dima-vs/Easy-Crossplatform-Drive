#include <gtest/gtest.h>
#include <QUuid>
#include <QString>
#include <QByteArray>
#include <QDir>

#include "file_storage.h"

class FileStorageTest : public testing::Test
{
protected:
    QString m_testStoragePath;
    FileStorage* m_storage = nullptr;

    void SetUp() override
    {
        m_testStoragePath = QDir::currentPath() + "/test_file_storage_" +
                            QUuid::createUuid().toString(QUuid::WithoutBraces);
        m_storage = new FileStorage(m_testStoragePath);
    }

    void TearDown() override
    {
        delete m_storage;

        QDir dir(m_testStoragePath);
        if (dir.exists())
        {
            dir.removeRecursively();
        }
    }
};

TEST_F(FileStorageTest, ReadAndWriteChunksInRandomOrder)
{
    QString sName = "resume_test.dat";

    QByteArray chunk2 = "WORLD";
    EXPECT_TRUE(m_storage->writeChunk(sName, 5, chunk2));

    QByteArray chunk1 = "HELLO";
    EXPECT_TRUE(m_storage->writeChunk(sName, 0, chunk1));

    QByteArray chunk3 = "!";
    EXPECT_TRUE(m_storage->writeChunk(sName, 10, chunk3));

    // check whole file size (5 + 5 + 1 = 11)
    EXPECT_EQ(m_storage->getFileSize(sName), 11);

    QByteArray fullData = m_storage->readChunk(sName, 0, 11);
    EXPECT_EQ(fullData, QByteArray("HELLOWORLD!"));

    QByteArray midData = m_storage->readChunk(sName, 5, 5);
    EXPECT_EQ(midData, QByteArray("WORLD"));
}

TEST_F(FileStorageTest, WritesChunksSequentially)
{
    QString sName = "sequential.dat";

    EXPECT_TRUE(m_storage->writeChunk(sName, 0, "Part1-"));
    EXPECT_TRUE(m_storage->writeChunk(sName, 6, "Part2"));

    EXPECT_EQ(m_storage->getFileSize(sName), 11);
    EXPECT_EQ(m_storage->readChunk(sName, 0, 11), QByteArray("Part1-Part2"));
}

TEST_F(FileStorageTest, ReadOutOfBoundsHandlesSafely)
{
    QString sName = "bounds.dat";
    m_storage->writeChunk(sName, 0, "12345");

    QByteArray outOfBounds = m_storage->readChunk(sName, 100, 10);
    EXPECT_TRUE(outOfBounds.isEmpty()) << "Should return empty array when reading past EOF";

    QByteArray partialData = m_storage->readChunk(sName, 3, 10);
    EXPECT_EQ(partialData, QByteArray("45")) << "Should return only available bytes up to EOF";

    QByteArray ghostData = m_storage->readChunk("not_exist.bin", 0, 100);
    EXPECT_TRUE(ghostData.isEmpty());
}

TEST_F(FileStorageTest, RemoveFileWorks)
{
    QString sName = "to_delete.dat";

    m_storage->writeChunk(sName, 0, "Data");
    EXPECT_EQ(m_storage->getFileSize(sName), 4);

    EXPECT_TRUE(m_storage->removeFile(sName));
    EXPECT_EQ(m_storage->getFileSize(sName), 0);
    EXPECT_FALSE(m_storage->removeFile(sName));
}

TEST_F(FileStorageTest, HandlesEmptyHolesCorrectly)
{
    QString sName = "with_zero_holes.dat";

    EXPECT_TRUE(m_storage->writeChunk(sName, 10, "DATA"));
    EXPECT_EQ(m_storage->getFileSize(sName), 14);

    QByteArray readBack = m_storage->readChunk(sName, 0, 14);

    ASSERT_EQ(readBack.size(), 14);
    // first 10 bytes have to be zero
    for (int i = 0; i < 10; ++i)
    {
        EXPECT_EQ(readBack.at(i), '\0');
    }

    EXPECT_EQ(readBack.sliced(10, 4), QByteArray("DATA"));
}
