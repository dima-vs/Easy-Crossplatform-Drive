#include <QCoreApplication>
#include <QDebug>
#include <QString>
#include "database_manager.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    DatabaseManager dbm;

    qDebug() << "DataAccessLayer";

    return a.exec();
}
