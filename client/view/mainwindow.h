#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QWidget>
#include <QTreeWidget>
#include <QMessageBox>
#include <QPainter>
#include "ViewConfig.h"
#include "TreeNodeData.h"

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
    void loadApp();

private:
    Ui::MainWindow *ui;
    QPixmap bg;
    QPixmap scaledBg;
    void paintEvent(QPaintEvent *event);
    QString username; // configurable
    QString password;
    void requestFileTree();
    void downloadFile(int fileId, QString name, qint64 size);
    void uploadFile();
    void updateProperties(QString name, QStringList access);
    void deleteFile(int fileId);
    void login(QString email, QString pword);
    void signup(QString email, QString pword);
    QSize treeSize;
    QSizePolicy sizePolicy;
    std::optional<int> currentIndex;
    QString currentName;
    qint64 currentSize;
    void showContextMenu(const QPoint &pos);
    void autoSaveConfig();

    // configurable
    bool expandTreeOnLoad;
    bool rememberLastLogin;

public slots:
    //from viewmodel
    void treeLoaded(QTreeWidget *treeWidget);
    void on_showMessageBox(QString message);
    void on_showVerificationDialog();
    void on_applySettings(CommonTypes::ViewConfig cfg);

private slots:
    //buttons and menu
    void on_actionLogin_triggered();
    void on_actionSign_Up_triggered();
    void on_actionQuit_triggered();
    void on_actionUpload_triggered();
    void on_actionDownload_triggered();
    void on_actionProperties_triggered();
    void on_actionNewFolder_triggered();
    void on_actionDelete_triggered();
    void on_actionDeselect_triggered();
    void on_actionAuto_expand_tree_triggered(bool checked);
    void on_actionRememberLastLogin_triggered(bool checked);

signals:
    //to viewmodel
    void loadSettings();
    void userLogin(QString email, QString password);
    void userSignUp(QString username, int code, QString password);
    void userEmail(QString email);

    void userRenameFile(int fileId, int parentId, QString name);
    void userDownload(int fileId, QString name, qint64 size);
    void userUpload(std::optional<int> id);
    void userCreateFolder(QString name, std::optional<int> id);
    void userRequestDeletion(std::optional<int> id);
    void userRequestRename(int fileId, std::optional<int> parentId, std::optional<QString> name);

    void requestTree();

    void userUpdateConfig(CommonTypes::ViewConfig cfg);
};
#endif // MAINWINDOW_H
