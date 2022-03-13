/********************************************************************************
** Form generated from reading UI file 'QtGuiZero00.ui'
**
** Created by: Qt User Interface Compiler version 5.9.7
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_QTGUIZERO00_H
#define UI_QTGUIZERO00_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_QtGuiZero00
{
public:
    QWidget *centralWidget;
    QMenuBar *menuBar;
    QToolBar *mainToolBar;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *QtGuiZero00)
    {
        if (QtGuiZero00->objectName().isEmpty())
            QtGuiZero00->setObjectName(QStringLiteral("QtGuiZero00"));
        QtGuiZero00->resize(600, 400);
        centralWidget = new QWidget(QtGuiZero00);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        QtGuiZero00->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(QtGuiZero00);
        menuBar->setObjectName(QStringLiteral("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 600, 23));
        QtGuiZero00->setMenuBar(menuBar);
        mainToolBar = new QToolBar(QtGuiZero00);
        mainToolBar->setObjectName(QStringLiteral("mainToolBar"));
        QtGuiZero00->addToolBar(Qt::TopToolBarArea, mainToolBar);
        statusBar = new QStatusBar(QtGuiZero00);
        statusBar->setObjectName(QStringLiteral("statusBar"));
        QtGuiZero00->setStatusBar(statusBar);

        retranslateUi(QtGuiZero00);

        QMetaObject::connectSlotsByName(QtGuiZero00);
    } // setupUi

    void retranslateUi(QMainWindow *QtGuiZero00)
    {
        QtGuiZero00->setWindowTitle(QApplication::translate("QtGuiZero00", "QtGuiZero00", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class QtGuiZero00: public Ui_QtGuiZero00 {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_QTGUIZERO00_H
