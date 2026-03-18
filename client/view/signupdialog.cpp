#include "signupdialog.h"
#include "ui_signupdialog.h"

signupdialog::signupdialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::signupdialog)
{
    ui->setupUi(this);
}

signupdialog::~signupdialog()
{
    delete ui;
}
