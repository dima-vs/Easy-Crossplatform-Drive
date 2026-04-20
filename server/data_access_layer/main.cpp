#include <QCoreApplication>
#include <QDebug>
#include <QString>
#include <QDateTime>
#include <QVariant>

#include "database_manager.h"
#include "user_repository.h"
#include "file_repository.h"
#include "token_repository.h"
#include "file_storage.h"
#include "domain/file_type.h"
#include "converter/file_type_converter.h"

using FileType = Common::Domain::FileType;
using FileTypeConverter = Common::Converter::FileTypeConverter;

void testDatabase();
void testFileStorage();

void printFileInfo(const FileRecord &file) {
    qDebug() << "--- File Info ---";
    qDebug() << "ID:         " << (file.isIDSet() ? QString::number(file.id()) : "Not set");
    qDebug() << "Name:       " << file.logicalName();
    qDebug() << "Type:       " << FileTypeConverter::toString(file.type());
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

    UserRecord u1("user1", "user1@gmail.com", "12345");
    userRep.add(u1);

    UserRecord u2("user2", "user2@gmail.com", "11111111111");
    userRep.add(u2);

    UserRecord u3("user3", "user3@gmail.com", "fffffffff");
    userRep.add(u3);

    UserRecord u4("user4", "user4@gmail.com", "34retww455y5");
    userRep.add(u4);

    UserRecord user = userRep.findByUsername("user3");
    qDebug() << user.id() <<
        user.username() <<
        user.email() <<
        user.passwordHash();

    userRep.remove("user2");

    UserRecord user3 = userRep.findByUsername("user3");
    int uid = user3.id();

    FileRepository fileRep(db);

    // File(ownerId, type, logicalName, serverName, size, [parentId])
    // servername is empty (QVariant()) for directories

    // /Images/
    FileRecord fImages(uid, FileType::Directory, "Images", QVariant(), 0);
    fileRep.add(fImages);

    // /Videos
    FileRecord fVideos(uid, FileType::Directory, "Videos", QVariant(), 0);
    fileRep.add(fVideos);

    int imgDirId = fileRep.findByPath(uid, { "Images" }).id();

    // /Images/icon.png
    FileRecord fIcon(uid, FileType::File, "icon.png", "fsdlkgjgerhg45j.bin", 2048, imgDirId);
    fileRep.add(fIcon);

    // /Images/Animals
    FileRecord fAnimals(uid, FileType::Directory, "Animals", QVariant(), 0, imgDirId);
    fileRep.add(fAnimals);

    int animalsDirId = fileRep.findByPath(uid, { "Images", "Animals" }).id();

    // /Images/Animals/Cats
    FileRecord fCats(uid, FileType::Directory, "Cats", QVariant(), 0, animalsDirId);
    fileRep.add(fCats);

    int catsDirId = fileRep.findByPath(uid, { "Images", "Animals", "Cats" }).id();

    // /Images/Animals/Cats/black_cat.png
    FileRecord fBlackCat(uid, FileType::File, "black_cat.png", "someservername1.bin", 5120, catsDirId);
    fileRep.add(fBlackCat);

    // /Images/Animals/Cats/white_cat.png
    FileRecord fWhiteCat(uid, FileType::File, "white_cat.jpg", "someservername2.bin", 43544, catsDirId);
    fileRep.add(fWhiteCat);

    FileRecord fTemp(uid, FileType::Directory, "temp", QVariant(), 0, catsDirId);
    fileRep.add(fTemp);

    FileRecord fCache(uid, FileType::Directory, "cache", QVariant(), 0, catsDirId);
    fileRep.add(fCache);

    // delete /Images/Animals/Cats/black_cat.png
    fileRep.remove(uid, { "Images", "Animals", "Cats", "black_cat.png" });


    TokenRepository tokenRep(db);

    TokenRecord t1("id1", "token1", uid, QDateTime::currentDateTime().addDays(1));
    tokenRep.add(t1);

    TokenRecord t2("id2", "token2", uid, QDateTime::currentDateTime().addDays(1));
    tokenRep.add(t2);

    TokenRecord t3("id3", "token3", uid, QDateTime::currentDateTime().addYears(-1));
    tokenRep.add(t3);

    TokenRecord t1_copy("id1", "token1", uid, QDateTime::currentDateTime().addDays(1));
    tokenRep.add(t1_copy);

    tokenRep.cleanExpired(QDateTime::currentDateTimeUtc());

    QList<FileRecord> fileList = fileRep.findByOwner(uid);
    qDebug() << "File list size: " << fileList.size();
    for (FileRecord& fileObj : fileList)
    {
        printFileInfo(fileObj);
    }
}
