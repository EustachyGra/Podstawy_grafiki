/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 6.5.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QWidget>
#include <ekran.h>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralwidget;
    Ekran *ekran;
    QPushButton *pushButton;
    QPushButton *pusty_reset;
    QPushButton *tekstura_reset;
    QPushButton *rob;
    Ekran *tekstura;
    QMenuBar *menubar;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName("MainWindow");
        MainWindow->resize(1313, 688);
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName("centralwidget");
        ekran = new Ekran(centralwidget);
        ekran->setObjectName("ekran");
        ekran->setGeometry(QRect(30, 70, 581, 521));
        pushButton = new QPushButton(centralwidget);
        pushButton->setObjectName("pushButton");
        pushButton->setGeometry(QRect(1190, 20, 83, 29));
        pusty_reset = new QPushButton(centralwidget);
        pusty_reset->setObjectName("pusty_reset");
        pusty_reset->setGeometry(QRect(260, 20, 83, 29));
        tekstura_reset = new QPushButton(centralwidget);
        tekstura_reset->setObjectName("tekstura_reset");
        tekstura_reset->setGeometry(QRect(920, 20, 83, 29));
        rob = new QPushButton(centralwidget);
        rob->setObjectName("rob");
        rob->setGeometry(QRect(610, 290, 80, 69));
        tekstura = new Ekran(centralwidget);
        tekstura->setObjectName("tekstura");
        tekstura->setGeometry(QRect(700, 70, 581, 521));
        MainWindow->setCentralWidget(centralwidget);
        menubar = new QMenuBar(MainWindow);
        menubar->setObjectName("menubar");
        menubar->setGeometry(QRect(0, 0, 1313, 25));
        MainWindow->setMenuBar(menubar);
        statusbar = new QStatusBar(MainWindow);
        statusbar->setObjectName("statusbar");
        MainWindow->setStatusBar(statusbar);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "MainWindow", nullptr));
        pushButton->setText(QCoreApplication::translate("MainWindow", "Dodaj", nullptr));
        pusty_reset->setText(QCoreApplication::translate("MainWindow", "Reset pkty", nullptr));
        tekstura_reset->setText(QCoreApplication::translate("MainWindow", "Reset pkty", nullptr));
        rob->setText(QCoreApplication::translate("MainWindow", "Texture\n"
"<---\n"
"", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
