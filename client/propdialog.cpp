#include "propdialog.h"
#include "ui_propdialog.h"

propdialog::propdialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::propdialog)
{
    ui->setupUi(this);
}

propdialog::~propdialog()
{
    delete ui;
}
