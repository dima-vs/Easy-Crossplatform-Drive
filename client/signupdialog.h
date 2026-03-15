#ifndef SIGNUPDIALOG_H
#define SIGNUPDIALOG_H

#include <QDialog>

namespace Ui {
class signupdialog;
}

class signupdialog : public QDialog
{
    Q_OBJECT

public:
    explicit signupdialog(QWidget *parent = nullptr);
    ~signupdialog();

private:
    Ui::signupdialog *ui;
};

#endif // SIGNUPDIALOG_H
