#include <QApplication>

#include "model.h"
#include "viewmodel.h"
#include "mainwindow.h"
#include <QObject>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QString styleSheet = R"(
    QMainWindow, QDialog
    {
        background: qlineargradient(x1:0, y1:0, x2:0, y2:1,
                                    stop:0 #00b3b3, stop:1 #ffffff);
    }

    QMenu
    {
        font-family: "Sans Serif";
        font-size: 12px;
        color: #003333;
        background: qlineargradient(x1:0, y1:0, x2:0, y2:1,
                                    stop:0 #00b3b3, stop:1 #ffffff);
    }

    QMenu::item::selected
    {
        background-color: #00cccc;
        color: #66ffff;
    }

    QMenu::item:disabled
    {
        color: #00b3b3;
    }

    QMenu::separator
    {
        height: 1px;
        background: qlineargradient(x1:0, y1:0, x2:1, y2:0
                                    stop:0      #00b3b3,
                                    stop:0.33   #80FFFF,
                                    stop:0.5    #FFFFFF,
                                    stop:0.66   #80FFFF,
                                    stop:1      #00b3b3);
        margin: 4px 10px;
    }

    QLabel
    {
        font-family: "Sans Serif";
        font-weight: bold;
        font-size: 12px;
        color: #003333
    }

    QPushButton
    {
        background-color: qlineargradient(x1:0, y1:0, x2:0, y2:1,
                                          stop:0    #78BCC2,
                                          stop:0.1  #9DEBEB,
                                          stop:0.3  #E6FFFF,
                                          stop:0.5  #FFFFFF,
                                          stop:0.7  #E6FFFF,
                                          stop:0.90 #9DEBEB,
                                          stop:1    #FFFFFF);

        border: 2px solid qlineargradient(x1:0, y1:0, x2:0, y2:1,
                                          stop:0    #80FFFF,
                                          stop:1    #78BCC2);
        border-radius: 4px;
        border-bottom: 2px solid #00b3b3;
        padding: 5px 15px;
        color: #004d4d;
        font-family: "Sans Serif";
        font-weight: bold;
    }

    QPushButton:hover
    {
        background-color: qlineargradient(x1:0, y1:0, x2:0, y2:1,
                                          stop:0    #5FA9AF,
                                          stop:0.1  #89D9D9,
                                          stop:0.3  #D1F5F5,
                                          stop:0.5  #E6FFFF,
                                          stop:0.7  #D1F5F5,
                                          stop:0.90 #89D9D9,
                                          stop:1    #E6FFFF);
    }

    QPushButton:pressed
    {
        background-color: qlineargradient(x1:0, y1:0, x2:0, y2:1,
                                          stop:0    #66FFFF,
                                          stop:0.4  #E6FFFF,
                                          stop:0.6  #E6FFFF,
                                          stop:1  #66FFFF,);
        border: 2px solid qlineargradient(x1:0, y1:0, x2:0, y2:1,
                                          stop:0    #B3FFFF,
                                          stop:1    #78BCC2);
        padding-bottom: 1px;
    }

    QLineEdit, QTextEdit
    {
        border: 2px solid qlineargradient(x1:0, y1:0, x2:0, y2:1,
                                          stop:0    #B3FFFF,
                                          stop:1    #78BCC2);
        border-radius: 4px;
        background: qlineargradient(x1:0, y1:0, x2:1, y2:0
                                    stop:0      #78BCC2,
                                    stop:0.01   #9DEBEB,
                                    stop:0.5    #E6FFFF,
                                    stop:0.99   #FFFFFF,
                                    stop:1      #78BCC2);
        selection-background-color: #00cccc;
        selection-color: #66ffff;

        font-family: "Sans Serif";
    }

    QFrame#ShadowFrame
    {
        border: 1px solid #004d4d;
        background-color: white;
    }

    QTreeView
    {
        background-color: transparent;
        border: 1px solid qlineargradient(x1:0, y1:0, x2:0, y2:1,
                                          stop:0    #80FFFF,
                                          stop:1    #78BCC2);
        border-radius: 4px;
        alternate-background-color: rgba(102, 255, 255, 0.1);
        show-decoration-selected: 1;
        color: #004d4d;

        font-family: "Sans Serif";
        font-size: 14;
        font-weight: bold;
    }

    QHeaderView::section
    {
        background-color: #66ffff;
        color: #004d4d;
        padding: 4px;
        border: 1px solid #e6ffff;
        font-weight: bold;
    }

    QTreeView::item:selected
    {
        background-color: #66ffff;
        color: #000000;
        border: none;
    }

    QTreeView::item:hover
    {
        background-color: rgba(102, 255, 255, 0.3);
    }

    QScrollBar:vertical
    {
        border: 1px solid qlineargradient(x1:0, y1:0, x2:0, y2:1,
                                          stop:0    #ffffff,
                                          stop:1    #78BCC2);
        background: #e6ffff;
        width: 10px;
        margin: 2px;
    }

    QScrollBar::handle:vertical
    {
        background: #66FFFF;
        min-height: 20px;
        border-radius: 2px;
    }

    QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical
    {
        height: 0px;
    }

    QFrame[frameShape="4"],
    QFrame[frameShape="5"]
    {
        background-color: qlineargradient(x1:0, y1:0, x2:0, y2:1,
                                          stop:0    #66ffff,
                                          stop:1    #78BCC2);
        border: 1px solid qlineargradient(x1:0, y1:0, x2:0, y2:1,
                                          stop:0    #78BCC2,
                                          stop:1    #e6ffff);
    }

    QFrame[frameShape="4"]
    {
        max-height: 2px;
        min-height: 2px;
    }

    QFrame[frameShape="5"]
    {
        max-width: 2px;
        min-width: 2px;
    }
)";

    a.setStyleSheet(styleSheet);
    a.setWindowIcon(QIcon(":/icon/view/icon.png"));

    Model m;
    viewmodel vm;
    MainWindow w;
    // model -> viewmodel
    QObject::connect(&m, &Model::saveToken, &vm, &viewmodel::on_saveToken);
    QObject::connect(&m, &Model::treeRequestFinished, &vm, &viewmodel::on_treeRequestFinished);
    QObject::connect(&m, &Model::loginFinished, &vm, &viewmodel::on_loginFinished);
    QObject::connect(&m, &Model::signupFinished, &vm, &viewmodel::on_signupFinished);
    QObject::connect(&m, &Model::sendEmailFinished, &vm, &viewmodel::on_sendEmailFinished);
    QObject::connect(&m, &Model::uploadInitFinished, &vm, &viewmodel::on_uploadInitFinished);
    QObject::connect(&m, &Model::uploadFinished, &vm, &viewmodel::on_uploadFinished);
    QObject::connect(&m, &Model::uploadCompleteFinished, &vm, &viewmodel::on_uploadCompleteFinished);
    QObject::connect(&m, &Model::downloadChunkFinished, &vm, &viewmodel::on_downloadChunkFinished);
    QObject::connect(&m, &Model::renameFinished, &vm, &viewmodel::on_renameFinished);
    QObject::connect(&m, &Model::deletionFinished, &vm, &viewmodel::on_deletionFinished);
    QObject::connect(&m, &Model::createFolderFinished, &vm, &viewmodel::on_createFolderFinished);

    //viewmodel -> model
    QObject::connect(&vm, &viewmodel::initUpload, &m, &Model::uploadInit);
    QObject::connect(&vm, &viewmodel::uploadData, &m, &Model::uploadData);
    QObject::connect(&vm, &viewmodel::completeUpload, &m, &Model::completeUpload);
    QObject::connect(&vm, &viewmodel::userSendEmail, &m, &Model::sendEmail);
    QObject::connect(&vm, &viewmodel::userSignUp, &m, &Model::signup);
    QObject::connect(&vm, &viewmodel::userLogin, &m, &Model::login);
    QObject::connect(&vm, &viewmodel::requestTree, &m, &Model::requestFileTree);
    QObject::connect(&vm, &viewmodel::createFolder, &m, &Model::createFolder);
    QObject::connect(&vm, &viewmodel::deleteFile, &m, &Model::requestDeletion);
    QObject::connect(&vm, &viewmodel::sendToken, &m, &Model::getSavedToken);
    QObject::connect(&vm, &viewmodel::downloadFile, &m, &Model::downloadData);
    QObject::connect(&vm, &viewmodel::renameFile, &m, &Model::renameFile);

    // view -> viewmodel
    QObject::connect(&w, &MainWindow::userUpload, &vm, &viewmodel::on_userUpload);
    QObject::connect(&w, &MainWindow::userSignUp, &vm, &viewmodel::on_userSignup);
    QObject::connect(&w, &MainWindow::userEmail, &vm, &viewmodel::on_userEmail);
    QObject::connect(&w, &MainWindow::userLogin, &vm, &viewmodel::on_userLogin);
    QObject::connect(&w, &MainWindow::requestTree, &vm, &viewmodel::on_requestTree);
    QObject::connect(&w, &MainWindow::userCreateFolder, &vm, &viewmodel::on_userCreateFolder);
    QObject::connect(&w, &MainWindow::userRequestDeletion, &vm, &viewmodel::on_userRequestDeletion);
    QObject::connect(&w, &MainWindow::loadSettings, &vm, &viewmodel::on_loadSettings);
    QObject::connect(&w, &MainWindow::userUpdateConfig, &vm, &viewmodel::on_userUpdateConfig);
    QObject::connect(&w, &MainWindow::userDownload, &vm, &viewmodel::on_userDownload);
    QObject::connect(&w, &MainWindow::userRequestRename, &vm, &viewmodel::on_userRequestRename);

    // viewmodel -> view
    QObject::connect(&vm, &viewmodel::showMessageBox, &w, &MainWindow::on_showMessageBox);
    QObject::connect(&vm, &viewmodel::showVerificationDialog, &w, &MainWindow::on_showVerificationDialog);
    QObject::connect(&vm, &viewmodel::updateTree, &w, &MainWindow::treeLoaded);
    QObject::connect(&vm, &viewmodel::applySettings, &w, &MainWindow::on_applySettings);
    w.show();
    w.loadApp();
    return a.exec();
}
