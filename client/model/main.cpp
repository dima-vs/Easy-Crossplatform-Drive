#include <QCoreApplication>
#include <QDebug>
#include <QString>
#include "model.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    qDebug() << "Model";

    Model *m = new Model;

    return a.exec();
}
