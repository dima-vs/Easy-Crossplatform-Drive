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

    QPushButton
    {
        background-color: #ffffff;
        border: 2px solid #66ffff;
        border-radius: 4px;
        padding: 5px 15px;
        color: #004d4d;
        font-weight: bold;
    }

    QPushButton:hover
    {
        background-color: #e6ffff;
    }

    QPushButton:pressed
    {
        background-color: #66ffff;
        color: white;
    }

    QLineEdit, QTextEdit
    {
        border: 1px solid #66ffff;
        border-radius: 4px;
        background: white;
        selection-background-color: #66ffff;
    }

    QFrame#ShadowFrame
    {
        border: 1px solid #004d4d;
        background-color: white;
    }

    QTreeView
    {
        background-color: transparent;
        border: 1px solid #66ffff;
        border-radius: 4px;
        alternate-background-color: rgba(102, 255, 255, 0.1);
        show-decoration-selected: 1;
        color: #004d4d;
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
        border: none;
        background: #e6ffff;
        width: 8px;
        margin: 1px;
    }

    QScrollBar::handle:vertical
    {
        background: #66ffff;
        min-height: 20px;
        border-radius: 5px;
    }

    QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical
    {
        height: 0px;
    }

    QFrame[frameShape="4"],
    QFrame[frameShape="5"]
    {
        background-color: #66ffff;
        border: none;
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
