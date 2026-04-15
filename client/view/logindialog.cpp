#include "logindialog.h"
#include "ui_logindialog.h"
#include <qregularexpression.h>
#include <qvalidator.h>

logindialog::logindialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::logindialog)
{
    ui->setupUi(this);

    QString emailPattern = R"(^[a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\.[a-zA-Z]{2,}$)";
    QRegularExpression rx(emailPattern);
    QRegularExpressionValidator *validator = new QRegularExpressionValidator(rx, this);

    ui->loginEdit->setValidator(validator);
}

logindialog::~logindialog()
{
    delete ui;
}
