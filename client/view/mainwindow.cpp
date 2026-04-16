#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "logindialog.h"
#include "signupdialog.h"
#include "propdialog.h"
#include "verificationdialog.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->centralwidget->setLayout(ui->mainLayout);
    setCentralWidget(ui->centralwidget);

    connect(ui->propertiesButton, &QPushButton::clicked, this, &MainWindow::openPropertiesDialog);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_actionLogin_triggered()
{
    logindialog* logindlg = new logindialog(this);
    logindlg->setWindowFlags(Qt::Dialog | Qt::Drawer);
    if (logindlg->exec() != QDialog::Accepted)
        return;
}

void MainWindow::on_actionSign_Up_triggered()
{
    signupdialog* signupdlg = new signupdialog(this);
    signupdlg->setWindowFlags(Qt::Dialog | Qt::Drawer);
    if (signupdlg->exec() != QDialog::Accepted)
        return;
}

void MainWindow::on_actionQuit_triggered()
{
    QCoreApplication::quit();
}

void MainWindow::openPropertiesDialog()
{
    propdialog* propdlg = new propdialog(this);
    propdlg->setWindowFlags(Qt::Dialog | Qt::Drawer);
    if (propdlg->exec() != QDialog::Accepted)
        return;
}

void MainWindow::treeLoaded(QTreeWidget *treeWidget)
{
    treeWidget->setParent(ui->treeView);
}

void MainWindow::on_actionTest1_triggered()
{
    verificationdialog* verdlg = new verificationdialog(this);
    verdlg->setWindowFlags(Qt::Dialog | Qt::Drawer);
    if (verdlg->exec() != QDialog::Accepted)
        return;
}
