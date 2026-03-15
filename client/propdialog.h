#ifndef PROPDIALOG_H
#define PROPDIALOG_H

#include <QDialog>

namespace Ui {
class propdialog;
}

class propdialog : public QDialog
{
    Q_OBJECT

public:
    explicit propdialog(QWidget *parent = nullptr);
    ~propdialog();

private:
    Ui::propdialog *ui;
};

#endif // PROPDIALOG_H
