#ifndef PROPDIALOG_H
#define PROPDIALOG_H

#include <QDialog>
#include <optional>

namespace Ui {
class propdialog;
}

class propdialog : public QDialog
{
    Q_OBJECT

public:
    explicit propdialog(QWidget *parent = nullptr);
    ~propdialog();

    std::optional<QString> parentName;
    std::optional<QString> name;

private:
    Ui::propdialog *ui;
private slots:
};

#endif // PROPDIALOG_H
