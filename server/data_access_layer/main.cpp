#include <QCoreApplication>
#include <QDebug>
#include <QString>
#include <QVariant>

#include "database_manager.h"
#include "user_repository.h"
#include "file_repository.h"
#include "token_repository.h"
#include "file_storage.h"


void testDatabase();
void testFileStorage();

void printFileInfo(const File &file) {
    qDebug() << "--- File Info ---";
    qDebug() << "ID:         " << (file.isIDSet() ? QString::number(file.id()) : "Not set");
    qDebug() << "Name:       " << file.logicalName();
    qDebug() << "Type:       " << file.type();
    qDebug() << "Size:       " << file.size() << "bytes";
    qDebug() << "Owner ID:   " << file.ownerId();
    qDebug() << "Upload Time:" << file.uploadTime().toString(Qt::ISODate);
    qDebug() << "Server Name:" << file.serverName();
    qDebug() << "Parent ID:  " << file.parentId();
    qDebug() << "Valid:      " << file.isValid();
    qDebug() << "-----------------";
}


int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    qDebug() << "DataAccessLayer";

    testDatabase();
    //testFileStorage();

    return 0;
    //return a.exec();
}

void testFileStorage()
{
    QString storagePath = "./test_storage/files";
    FileStorage storage(storagePath);

    QString sName = "test_chunked_file.dat";

    storage.removeFile(sName);

    qDebug() << "\n1. Writing chunks in random order...";

    QByteArray chunk2 = "WORLD";
    storage.writeChunk(sName, 5, chunk2);

    QByteArray chunk1 = "HELLO";
    storage.writeChunk(sName, 0, chunk1);

    QByteArray chunk3 = "!";
    storage.writeChunk(sName, 10, chunk3);

    qint64 size = storage.getFileSize(sName);
    qDebug() << "Current file size:" << size << "bytes (Expected: 11)";

    qDebug() << "\n2. Reading data back...";

    QByteArray fullData = storage.readChunk(sName, 0, size);
    qDebug() << "Full content:" << fullData << "(Expected: HELLOWORLD!)";

    QByteArray midData = storage.readChunk(sName, 5, 5);
    qDebug() << "Middle content:" << midData << "(Expected: WORLD)";

    qDebug() << "\n3. Testing error cases...";
    QByteArray outOfBounds = storage.readChunk(sName, 100, 10);
    qDebug() << "Read out of bounds returned size:" << outOfBounds.size();

    // qDebug() << "\n4. Removing file...";
    // if (storage.removeFile(sName)) {
    //     qDebug() << "File successfully removed.";
    // }

    qDebug() << "Does file still exist (size check):" << storage.getFileSize(sName);
}

void testDatabase()
{
    DatabaseManager db;
    UserRepository userRep(db);

    userRep.addNewUser(User("user1", "user1@gmail.com", "12345"));
    userRep.addNewUser(User("user2", "user2@gmail.com", "11111111111"));
    userRep.addNewUser(User("user3", "user3@gmail.com", "fffffffff"));
    userRep.addNewUser(User("user4", "user4@gmail.com", "34retww455y5"));

    User user = userRep.getUser("user3");
    qDebug() << user.id() <<
        user.username() <<
        user.email() <<
        user.passwordHash();

    userRep.deleteUser("user2");

    User user3 = userRep.getUser("user3");
    int uid = user3.id();

    FileRepository fileRep(db);
    // File(ownerId, type, logicalName, serverName, size, [parentId])
    // servername is empty (QVariant()) for directories
    fileRep.addNewFile(File(uid, "directory", "Images", QVariant(), 0));
    // /Videos
    fileRep.addNewFile(File(uid, "directory", "Videos", QVariant(), 0));

    int imgDirId = fileRep.getFile(uid, { "Images" }).id();
    // /Images/icon.png
    fileRep.addNewFile(File(uid, "file", "icon.png",
                            "fsdlkgjgerhg45j.bin", 2048, imgDirId));

    // /Images/Animals
    fileRep.addNewFile(File(uid, "directory", "Animals", QVariant(), 0, imgDirId));
    int animalsDirId = fileRep.getFile(uid, { "Images", "Animals" }).id();
    // /Images/Animals/Cats
    fileRep.addNewFile(File(uid, "directory", "Cats", QVariant(), 0, animalsDirId));

    int catsDirId = fileRep.getFile(uid, { "Images", "Animals", "Cats" }).id();
    // /Images/Animals/Cats/black_cat.png
    fileRep.addNewFile(File(uid, "file", "black_cat.png", "someservername1.bin", 5120, catsDirId));
    // /Images/Animals/Cats/white_cat.png
    fileRep.addNewFile(File(uid, "file", "white_cat.jpg", "someservername2.bin", 43544, catsDirId));

    fileRep.addNewFile(File(uid, "directory", "temp", QVariant(), 0, catsDirId));
    fileRep.addNewFile(File(uid, "directory", "cache", QVariant(), 0, catsDirId));

    // delete /Images/Animals/Cats/black_cat.png
    fileRep.deleteFile(uid, { "Images", "Animals", "Cats", "black_cat.png" });


    TokenRepository tokenRep(db);
    tokenRep.addNewToken(Token(
        "id1", "token1", uid, QDateTime::currentDateTime().addDays(1)));
    tokenRep.addNewToken(Token(
        "id2", "token2", uid, QDateTime::currentDateTime().addDays(1)));
    tokenRep.addNewToken(Token(
        "id3", "token3", uid, QDateTime::currentDateTime().addYears(-1)));
    tokenRep.addNewToken(Token(
        "id1", "token1", uid, QDateTime::currentDateTime().addDays(1)));

    tokenRep.cleanExpiredTokens();

    QList<File> fileList = fileRep.getFilesByOwner(uid);
    qDebug() << "File list size: " << fileList.size();
    for (File& fileObj : fileList)
    {
        printFileInfo(fileObj);
    }
}
