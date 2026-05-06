#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QWidget>
#include <QTreeWidget>
#include <QMessageBox>

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
    QString username;
    QString password;
    void requestFileTree();
    void downloadFile(int fileId);
    void uploadFile();
    void updateProperties(QString name, QStringList access);
    void deleteFile(int fileId);
    void login(QString email, QString pword);
    void signup(QString email, QString pword);
    QSize size;
    QSizePolicy sizePolicy;
    std::optional<int> currentIndex;

public slots:
    //from viewmodel
    void treeLoaded(QTreeWidget *treeWidget);
    void on_showMessageBox(QString message);
    void on_showVerificationDialog();

private slots:
    //buttons and menu
    void on_actionLogin_triggered();
    void on_actionSign_Up_triggered();
    void on_actionQuit_triggered();
    void on_actionTest1_triggered();
    void on_actionUpload_triggered();
    void on_actionProperties_triggered();
    void on_actionNewFolder_triggered();

signals:
    //to viewmodel
    void userLogin(QString email, QString password);
    void userSignUp(QString username, int code, QString password);
    void userEmail(QString email);

    void userRenameFile(int fileId, int parentId, QString name);
    void userDownload(int fileId);
    void userUpload(std::optional<int> id);
    void userCreateFolder(QString name, std::optional<int> id);

    void requestTree();
};
#endif // MAINWINDOW_H
