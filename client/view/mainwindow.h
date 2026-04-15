#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    void requestFileTree();
    void downloadFile(int fileId);
    void uploadFile();
    void updateProperties(QString name, QStringList access);
    void deleteFile(int fileId);

    void login(QString email, QString pword);
    void signup(QString email, QString pword);


private slots:
    void on_actionLogin_triggered();
    void on_actionSign_Up_triggered();
    void on_actionQuit_triggered();
    void on_actionTest1_triggered();

    void openPropertiesDialog();
};
#endif // MAINWINDOW_H
