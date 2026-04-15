#include "verificationdialog.h"
#include "ui_verificationdialog.h"
#include <QIntValidator>
#include <QEvent>
#include <QTimer>

verificationdialog::verificationdialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::verificationdialog)
{
    ui->setupUi(this);

    QIntValidator *validator = new QIntValidator(0, 9, this);
    codeEditList.append(ui->lineEdit_1);
    codeEditList.append(ui->lineEdit_2);
    codeEditList.append(ui->lineEdit_3);
    codeEditList.append(ui->lineEdit_4);
    codeEditList.append(ui->lineEdit_5);
    codeEditList.append(ui->lineEdit_6);
    for (int i = 0; i < codeEditList.size(); ++i)
    {
        codeEditList[i]->setValidator(validator);

        connect(codeEditList[i], &QLineEdit::textEdited,
                this, &verificationdialog::changeField);

        codeEditList[i]->installEventFilter(this);
    }
    codeEditList[activeIndex]->setFocus();
}

verificationdialog::~verificationdialog()
{
    delete ui;
}

bool verificationdialog::eventFilter(QObject *obj, QEvent *event)
{
    if (event->type() == QEvent::FocusIn)
    {
        for (int i = 0; i < codeEditList.size(); ++i)
        {
            if (obj == codeEditList[i])
            {
                activeIndex = i;
                QLineEdit *edit = codeEditList[i];
                QTimer::singleShot(0, edit, [edit]() {
                    edit->selectAll();
                });
                break;
            }
        }
    }
    return QDialog::eventFilter(obj, event);
}

void verificationdialog::changeField()
{
    if (activeIndex < codeEditList.size()-1)
    {
        activeIndex++;
    }
    codeEditList[activeIndex]->setFocus();
}
