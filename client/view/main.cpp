#include "mainwindow.h"

#include <QApplication>

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
        width: 10px;
        margin: 0px;
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

    MainWindow w;
    w.show();
    return a.exec();
}
