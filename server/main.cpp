#include <QCoreApplication>
#include <QTextStream>
#include <QDataStream>
#include <QString>
#include <QFile>
#include <QIODevice>
#include <QTcpServer>
#include <QTcpSocket>
#include <QHostAddress>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QDebug>

enum class GlobalCommand {
    Register = 1,
    Login = 2,
    GetMetadata = 3, // отримати інформацію про всі папки і файли в поточній директорії (або в корні)
    UploadFile = 4,
    DownloadFile = 5,
    CreateFolder = 6,
    DeleteFile = 7,
    RenameFile = 8,
    MoveFile = 9
};

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    QTextStream out(stdout);
    // out << "Hello world" << Qt::endl;

    // QTcpServer server;

    // QObject::connect(&server,
    //         &QTcpServer::newConnection, [&]() {
    //             QTcpSocket *socket = server.nextPendingConnection();

    //             if (!socket->waitForReadyRead()) {
    //                 socket->disconnectFromHost();
    //                 return;
    //             }

    //             QDataStream in(socket);

    //             QString filename;
    //             qint64 filesize;

    //             in >> filename;
    //             in >> filesize;

    //             QFile file(filename);
    //             //file.open(QIODevice::WriteOnly);

    //             if (file.open(QIODevice::WriteOnly)) {
    //                 while (socket->bytesAvailable() < filesize) {
    //                     if (!socket->waitForReadyRead()) break;
    //                 }

    //                 file.write(socket->read(filesize));
    //                 file.close();
    //                 out << "Received: " << filename << " (" << filesize << " bytes)" << Qt::endl;
    //             }

    //             // while (socket->bytesAvailable() < filesize)
    //             //     socket->waitForReadyRead();
    //             // file.write(socket->read(filesize));
    // });

    // bool isOk = server.listen(QHostAddress::LocalHost, 1222);
    // if (isOk)
    //     out << "Server started" << Qt::endl;

    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");

    db.setDatabaseName("main_database.db");

    if (!db.open()) {
        qDebug() << "Error: connection with database fail" << db.lastError().text();
    } else {
        qDebug() << "Database: connection ok";

        QSqlQuery query;
        query.exec("CREATE TABLE IF NOT EXISTS users("
                   "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                   "username TEXT UNIQUE NOT NULL,"
                   "email TEXT UNIQUE,"
                   "password TEXT UNIQUE NOT NULL)");

        query.exec("CREATE TABLE IF NOT EXISTS files("
                   "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                   "owner_id INTEGER NOT NULL,"
                   "type TEXT NOT NULL,"
                   "name TEXT NOT NULL,"
                   "server_name TEXT NOT NULL,"
                   "size INTEGER,"
                   "upload_time DATETIME DEFAULT CURRENT_TIMESTAMP,"
                   "parent_id INTEGER DEFAULT 0,"
                   "FOREIGN KEY(owner_id) REFERENCES users(id))");
    }
    return a.exec();
}
