#include "logindialog.h"
#include "ui_logindialog.h"
#include <qregularexpression.h>
#include <qvalidator.h>

logindialog::logindialog(QWidget *parent, std::optional<QString> lastLogin)
    : QDialog(parent)
    , ui(new Ui::logindialog)
{
    ui->setupUi(this);

    QString loginPattern = R"(^[a-zA-Z0-9._-]+$)";
    QRegularExpression rx(loginPattern);
    QRegularExpressionValidator *validator = new QRegularExpressionValidator(rx, this);

    ui->loginEdit->setValidator(validator);
    ui->loginEdit->setText(lastLogin.value());
    login = lastLogin.value();

    QObject::connect(ui->loginEdit, &QLineEdit::textChanged, [this]()
                     {login = ui->loginEdit->text();});
    QObject::connect(ui->passwordEdit, &QLineEdit::textChanged, [this]()
                     {password = ui->passwordEdit->text();});
}

logindialog::~logindialog()
{
    delete ui;
}
