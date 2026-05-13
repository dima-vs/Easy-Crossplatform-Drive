#include "signupdialog.h"
#include "ui_signupdialog.h"
#include <qregularexpression.h>
#include <qvalidator.h>

signupdialog::signupdialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::signupdialog)
{
    ui->setupUi(this);

    QString usernamePattern = R"(^[a-zA-Z0-9._-]+$)";
    QString emailPattern = R"(^[a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\.[a-zA-Z]{2,}$)";

    QRegularExpression emailRegExp(emailPattern);
    QRegularExpressionValidator *emailValidator =
        new QRegularExpressionValidator(emailRegExp, this);
    ui->loginEdit->setValidator(emailValidator);

    QRegularExpression usernameRegExp(usernamePattern);
    QRegularExpressionValidator *usernameValidator =
        new QRegularExpressionValidator(usernameRegExp, this);
    ui->usernameEdit->setValidator(usernameValidator);

    QObject::connect(ui->loginEdit, &QLineEdit::textChanged, [this]()
                     {email = ui->loginEdit->text();});
    QObject::connect(ui->passwordEdit, &QLineEdit::textChanged, [this]()
                     {password = ui->passwordEdit->text();});
    QObject::connect(ui->usernameEdit, &QLineEdit::textChanged, [this]()
                     {username = ui->usernameEdit->text();});
}

signupdialog::~signupdialog()
{
    delete ui;
}
