#include "signupdialog.h"
#include "ui_signupdialog.h"
#include <qregularexpression.h>
#include <qvalidator.h>

signupdialog::signupdialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::signupdialog)
{
    ui->setupUi(this);

    QString emailPattern = R"(^[a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\.[a-zA-Z]{2,}$)";
    QRegularExpression rx(emailPattern);
    QRegularExpressionValidator *validator = new QRegularExpressionValidator(rx, this);

    ui->loginEdit->setValidator(validator);
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
