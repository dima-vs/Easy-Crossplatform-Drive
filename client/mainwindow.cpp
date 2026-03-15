#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "logindialog.h"
#include "signupdialog.h"
#include "propdialog.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->centralwidget->setLayout(ui->mainLayout);
    setCentralWidget(ui->centralwidget);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_actionLogin_triggered()
{
    logindialog* logindlg = new logindialog(this);
    logindlg->setWindowFlags(Qt::Dialog | Qt::Drawer);
    logindlg->exec();
}

void MainWindow::on_actionSign_Up_triggered()
{
    signupdialog* signupdlg = new signupdialog(this);
    signupdlg->setWindowFlags(Qt::Dialog | Qt::Drawer);
    signupdlg->exec();
}
