#include <QCoreApplication>
#include <QDebug>
#include <QString>
#include <QVariant>

#include "database_manager.h"
#include "user_repository.h"
#include "file_repository.h"
#include "user.h"
#include "file.h"

void printFile(const File& f)
{
    if (f.isValid()) {
        qInfo() << "[FILE FOUND] ID:" << f.id()
        << "Name:" << f.logicalName()
        << "Type:" << f.type()
        << "ParentID:" << f.parentId().toInt();
    } else {
        qInfo() << "[FILE NOT FOUND]";
    }
}

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    qDebug() << "DataAccessLayer";

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
    //fileRep.deleteFile(uid, { "Images", "Animals", "Cats", "black_cat.png" });


    return 0;

    //return a.exec();
}
