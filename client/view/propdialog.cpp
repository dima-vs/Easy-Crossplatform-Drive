#include "propdialog.h"
#include "ui_propdialog.h"

propdialog::propdialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::propdialog)
{
    ui->setupUi(this);

    connect(ui->filenameEdit, &QLineEdit::textChanged, [this]()
            {
                if (ui->filenameEdit->text().isEmpty())
                {
                    name = std::nullopt;
                }
                else
                {
                    name = ui->filenameEdit->text();
                }
            });
    connect(ui->parentnameEdit, &QLineEdit::textChanged, [this]()
            {
                if (ui->parentnameEdit->text().isEmpty())
                {
                    parentName = std::nullopt;
                }
                else
                {
                    parentName = ui->parentnameEdit->text();
                }
            });
}

propdialog::~propdialog()
{
    delete ui;
}
