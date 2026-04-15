#include "propdialog.h"
#include "ui_propdialog.h"

propdialog::propdialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::propdialog)
{
    ui->setupUi(this);

    connect(ui->checkBox, &QCheckBox::checkStateChanged, this, &propdialog::checkBoxChanged);
}

propdialog::~propdialog()
{
    delete ui;
}

void propdialog::checkBoxChanged()
{
    if(ui->checkBox->isChecked() == true)
    {
        parentBackup = ui->parentnameEdit->text();
        ui->parentnameEdit->clear();
        ui->parentnameEdit->setReadOnly(true);
    }
    else
    {
        ui->parentnameEdit->setReadOnly(false);
        ui->parentnameEdit->setText(parentBackup);
        parentBackup.clear();
    }
}
