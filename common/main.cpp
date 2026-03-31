#include <QCoreApplication>
#include <QDebug>
#include <QString>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    qDebug() << "Common";

    return 0;
    //return a.exec();
}
