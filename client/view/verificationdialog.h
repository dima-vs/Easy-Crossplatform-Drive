#ifndef VERIFICATIONDIALOG_H
#define VERIFICATIONDIALOG_H

#include <QDialog>
#include <QLineEdit>

namespace Ui {
class verificationdialog;
}

class verificationdialog : public QDialog
{
    Q_OBJECT

public:
    explicit verificationdialog(QWidget *parent = nullptr);
    ~verificationdialog();
    QList<QLineEdit*> codeEditList;

protected:
    bool eventFilter(QObject *obj, QEvent *event) override;

private:
    Ui::verificationdialog *ui;
    int activeIndex = 0;

private slots:
    void changeField();
};

#endif // VERIFICATIONDIALOG_H
