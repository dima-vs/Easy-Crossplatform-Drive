#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "logindialog.h"
#include "signupdialog.h"
#include "propdialog.h"
#include "verificationdialog.h"
#include <cmath>
#include <QInputDialog>
#include <qlabel.h>

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
    connect(ui->deleteButton, &QPushButton::clicked, this, &MainWindow::on_actionDelete_triggered);
    connect(ui->downloadButton, &QPushButton::clicked, this, &MainWindow::on_actionDownload_triggered);
    currentIndex = std::nullopt;
    currentName = "";
    currentSize = 0;
    //expandTreeOnLoad = false; //add initialization from cfg later
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::loadApp()
{
    emit loadSettings();
}

void MainWindow::requestFileTree()
{
    emit requestTree();
}

void MainWindow::downloadFile(int fileId, QString name, qint64 size)
{

    emit userDownload(fileId, name, size);
}

void MainWindow::uploadFile()
{
    emit userUpload(currentIndex);
}

void MainWindow::showContextMenu(const QPoint &pos)
{
    QTreeWidget *tree = qobject_cast<QTreeWidget*>(sender());

    if (!tree) return;

    QModelIndex index = tree->indexAt(pos);

    QMenu menu(this);

    menu.addAction(ui->actionNewFolder);
    menu.addAction(ui->actionDownload);
    menu.addAction(ui->actionUpload);
    menu.addSeparator();
    menu.addAction(ui->actionDelete);
    menu.addAction(ui->actionProperties);
    menu.addSeparator();
    menu.addAction(ui->actionDeselect);

    if (!index.isValid())
    {
        ui->actionDelete->setEnabled(false);
        ui->actionProperties->setEnabled(false);
    }
    else
    {
        ui->actionDelete->setEnabled(true);
        ui->actionProperties->setEnabled(true);
    }

    menu.exec(tree->viewport()->mapToGlobal(pos));
}

void MainWindow::autoSaveConfig()
{
    CommonTypes::ViewConfig cfg;

    cfg.expandTreeOnLoad = expandTreeOnLoad;
    cfg.rememberLastLogin = rememberLastLogin;
    if (rememberLastLogin)
    {
        cfg.lastLogin = username;
    }
    else
    {
        cfg.lastLogin = "";
    }
    //add more variables here

    emit userUpdateConfig(cfg);
}
void MainWindow::on_actionLogin_triggered()
{
    QString s;
    if (rememberLastLogin)
    {
        s = username;
    }
    else
    {
        s = "";
    }
    logindialog* logindlg = new logindialog(this, s);
    logindlg->setWindowFlags(Qt::Dialog | Qt::Drawer);
    if (logindlg->exec() == QDialog::Accepted)
    {
        username = logindlg->login;
        emit userLogin(logindlg->login, logindlg->password);
        autoSaveConfig();
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
        autoSaveConfig();
    }
    else
    {
        return;
    }
}

void MainWindow::on_actionQuit_triggered()
{
    autoSaveConfig();
    QCoreApplication::quit();
}

void MainWindow::on_actionProperties_triggered()
{
    if (currentIndex == std::nullopt)
    {
        on_showMessageBox("No file selected!");
        return;
    }

    QLayoutItem* layoutItem = ui->bottomLayout->itemAt(0);

    if (!layoutItem || !layoutItem->widget())
        return;

    QTreeWidget* treeWidget =
        qobject_cast<QTreeWidget*>(layoutItem->widget());

    if (!treeWidget)
        return;

    propdialog* propdlg = new propdialog(this);
    propdlg->setWindowFlags(Qt::Dialog | Qt::Drawer);

    if (propdlg->exec() != QDialog::Accepted)
    {
        delete propdlg;
        return;
    }

    std::optional<int> finalParentId = std::nullopt;

    if (propdlg->parentName.has_value())
    {
        QString rawPath = propdlg->parentName.value().trimmed();

        // "/" -> root
        if (rawPath == "/")
        {
            finalParentId = -1;
        }
        else
        {
            if (rawPath.contains('(') && rawPath.contains(')'))
            {
                rawPath = rawPath.section('(', 1, 1)
                .section(')', 0, 0)
                    .trimmed();
            }

            QStringList pathParts =
                rawPath.split('/', Qt::SkipEmptyParts);

            QTreeWidgetItem* currentItem = nullptr;

            if (!pathParts.isEmpty())
            {
                QString firstPart = pathParts.takeFirst();

                for (int i = 0; i < treeWidget->topLevelItemCount(); ++i)
                {
                    QTreeWidgetItem* item =
                        treeWidget->topLevelItem(i);

                    if (item->text(0) == firstPart)
                    {
                        currentItem = item;
                        break;
                    }
                }
            }

            for (const QString& part : pathParts)
            {
                if (!currentItem)
                    break;

                QTreeWidgetItem* foundChild = nullptr;

                for (int i = 0; i < currentItem->childCount(); ++i)
                {
                    QTreeWidgetItem* child =
                        currentItem->child(i);

                    if (child->text(0) == part)
                    {
                        foundChild = child;
                        break;
                    }
                }

                currentItem = foundChild;
            }

            if (currentItem)
            {
                auto data = currentItem
                                ->data(0, Qt::UserRole)
                                .value<CommonTypes::TreeNodeData>();

                finalParentId = data.fileId;
            }
        }
    }

    emit userRequestRename(
        currentIndex.value(),
        finalParentId,
        propdlg->name
        );

    propdlg->deleteLater();
}

void MainWindow::on_actionNewFolder_triggered()
{
    qDebug() << "on_actionNewFolder";
    bool ok;
    QString result = QInputDialog::getText(this, "New Folder",
                                         "Folder name:", QLineEdit::Normal,
                                         "", &ok);
    if (ok && !result.isEmpty())
    {
        emit userCreateFolder(result, currentIndex);
    }
}

void MainWindow::on_actionDelete_triggered()
{
    emit userRequestDeletion(currentIndex);
}

void MainWindow::on_actionDeselect_triggered()
{
    QAbstractItemView *tree = qobject_cast<QAbstractItemView*>(focusWidget());

    if (!tree)
    {
        QLayoutItem *item = ui->bottomLayout->itemAt(0);
        if (item && item->widget())
        {
            tree = qobject_cast<QAbstractItemView*>(item->widget());
        }
    }

    if (tree && tree->selectionModel())
    {
        tree->selectionModel()->clearSelection();
        currentIndex = std::nullopt;
        currentName = "";
        currentSize = 0;
        tree->setCurrentIndex(QModelIndex());
    }
}

void MainWindow::on_actionAuto_expand_tree_triggered(bool checked)
{
    expandTreeOnLoad = checked;
    autoSaveConfig();
}

void MainWindow::on_actionRememberLastLogin_triggered(bool checked)
{
    rememberLastLogin = checked;
    autoSaveConfig();
}

void MainWindow::treeLoaded(QTreeWidget *treeWidget)
{
    currentIndex = std::nullopt;
    currentName = "";
    currentSize = 0;
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
    treeWidget->setContextMenuPolicy(Qt::CustomContextMenu);
    QObject::connect(treeWidget, &QTreeWidget::itemClicked, [this](QTreeWidgetItem *item)
    {
        currentIndex = item->data(0, Qt::ItemDataRole::UserRole).value<CommonTypes::TreeNodeData>().fileId;
        currentName = item->text(0);
        currentSize = item->data(0, Qt::ItemDataRole::UserRole).value<CommonTypes::TreeNodeData>().size;
    });
    connect(treeWidget, &QTreeWidget::customContextMenuRequested,
            this, &MainWindow::showContextMenu);
    if (expandTreeOnLoad)
    {
        treeWidget->expandAll();
    }
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

void MainWindow::on_applySettings(CommonTypes::ViewConfig cfg)
{
    expandTreeOnLoad = cfg.expandTreeOnLoad;
    ui->actionAuto_expand_tree->setChecked(expandTreeOnLoad);
    rememberLastLogin = cfg.rememberLastLogin;
    ui->actionRememberLastLogin->setChecked(cfg.rememberLastLogin);
    username = cfg.lastLogin;
    qDebug() << "Applied settings";
}

void MainWindow::on_actionUpload_triggered()
{
    uploadFile();
}

void MainWindow::on_actionDownload_triggered()
{
    if (currentIndex != std::nullopt)
    {
        downloadFile(currentIndex.value(), currentName, currentSize);
    }
    else
    {
        on_showMessageBox("Choose file to download!");
    }
}
