#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "logindialog.h"
#include "signupdialog.h"
#include "propdialog.h"
#include "verificationdialog.h"
#include <cmath>
#include <QInputDialog>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->centralwidget->setLayout(ui->mainLayout);
    setCentralWidget(ui->centralwidget);
    size = ui->treePlaceholder->minimumSize();
    sizePolicy = ui->treePlaceholder->sizePolicy();
    connect(ui->propertiesButton, &QPushButton::clicked, this, &MainWindow::on_actionProperties_triggered);
    connect(ui->uploadButton, &QPushButton::clicked, this, &MainWindow::uploadFile);
    connect(ui->refreshButton, &QPushButton::clicked, this, &MainWindow::requestFileTree);
    connect(ui->addfolderButton, &QPushButton::clicked, this, &MainWindow::on_actionNewFolder_triggered);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::requestFileTree()
{
    emit requestTree();
}

void MainWindow::uploadFile()
{
    emit userUpload();
}

void MainWindow::on_actionLogin_triggered()
{
    logindialog* logindlg = new logindialog(this);
    logindlg->setWindowFlags(Qt::Dialog | Qt::Drawer);
    if (logindlg->exec() == QDialog::Accepted)
    {
        emit userLogin(logindlg->login, logindlg->password);
    }
    else
    {
        return;
    }
}

void MainWindow::on_actionSign_Up_triggered()
{
    signupdialog* signupdlg = new signupdialog(this);
    signupdlg->setWindowFlags(Qt::Dialog | Qt::Drawer);
    if (signupdlg->exec() == QDialog::Accepted)
    {
        emit userEmail(signupdlg->email);
        username = signupdlg->username;
        password = signupdlg->password;
    }
    else
    {
        return;
    }
}

void MainWindow::on_actionQuit_triggered()
{
    QCoreApplication::quit();
}

void MainWindow::on_actionProperties_triggered()
{
    propdialog* propdlg = new propdialog(this);
    propdlg->setWindowFlags(Qt::Dialog | Qt::Drawer);
    if (propdlg->exec() != QDialog::Accepted)
        return;
}

void MainWindow::on_actionNewFolder_triggered()
{
    qDebug() << "on_actionNewFolder";
    bool ok;
    QString result = QInputDialog::getText(this, "Title",
                                         "Folder name:", QLineEdit::Normal,
                                         "", &ok);
    if (ok && !result.isEmpty())
    {
        emit userCreateFolder(result, currentIndex);
    }
}

void MainWindow::treeLoaded(QTreeWidget *treeWidget)
{
    treeWidget->setMinimumSize(size);
    treeWidget->setSizePolicy(sizePolicy);

    if (QLayoutItem *item = ui->bottomLayout->takeAt(0))
    {
        if (QWidget *widget = item->widget())
        {
            widget->hide();
            widget->deleteLater();
        }
        delete item;
    }
    ui->bottomLayout->insertWidget(0, treeWidget);
    //ui->bottomLayout->removeWidget(ui->treePlaceholder);

    treeWidget->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    treeWidget->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    QObject::connect(treeWidget, &QTreeWidget::itemClicked, [this](QTreeWidgetItem *item)
    {
        currentIndex = item->data(0, Qt::ItemDataRole::UserRole).toInt();
    });
}

void MainWindow::on_showMessageBox(QString message)
{
    QMessageBox msgBox;
    msgBox.setIcon(QMessageBox::Information);
    msgBox.setWindowTitle("Info");
    msgBox.setText(message);
    msgBox.setStandardButtons(QMessageBox::Ok);
    msgBox.setWindowFlags(Qt::FramelessWindowHint);
    msgBox.exec();
}

void MainWindow::on_showVerificationDialog()
{
    verificationdialog* verdlg = new verificationdialog(this);
    verdlg->setWindowFlags(Qt::Dialog | Qt::Drawer);
    int code = 0;
    if (verdlg->exec() == QDialog::Accepted)
    {
        for (int i = 0; i < verdlg->codeEditList.size(); ++i)
        {
            code += verdlg->codeEditList.at(i)->text().toInt()*std::pow(10, verdlg->codeEditList.size()-1-i);
        }
    }
    else
    {
        return;
    }
    qDebug() << code;
    emit userSignUp(username, code, password);
}

void MainWindow::on_actionTest1_triggered()
{
    // verificationdialog* verdlg = new verificationdialog(this);
    // verdlg->setWindowFlags(Qt::Dialog | Qt::Drawer);
    // if (verdlg->exec() != QDialog::Accepted)
    //     return;


}

void MainWindow::on_actionUpload_triggered()
{
    uploadFile();
}
