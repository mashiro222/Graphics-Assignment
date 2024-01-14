/********************************************************************************
** Form generated from reading UI file 'minidraw.ui'
**
** Created by: Qt User Interface Compiler version 5.9.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MINIDRAW_H
#define UI_MINIDRAW_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QSlider>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MiniDrawClass
{
public:
    /*QWidget *centralWidget;
    QSlider *horizontalSlider;
    QMenuBar *menuBar;
    QToolBar *mainToolBar;
    QStatusBar *statusBar;
	*/
    void setupUi(QMainWindow *MiniDrawClass)
    {
        if (MiniDrawClass->objectName().isEmpty())
            MiniDrawClass->setObjectName(QStringLiteral("MiniDrawClass"));
        MiniDrawClass->resize(1200, 800);
        /*centralWidget = new QWidget(MiniDrawClass);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
		
		horizontalSlider = new QSlider(Minidraw);
        horizontalSlider->setObjectName(QStringLiteral("horizontalSlider"));
        horizontalSlider->setGeometry(QRect(30, 20, 160, 16));
        horizontalSlider->setOrientation(Qt::Horizontal);
		
        
        MiniDrawClass->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(MiniDrawClass);
        menuBar->setObjectName(QStringLiteral("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 600, 18));
        MiniDrawClass->setMenuBar(menuBar);
        mainToolBar = new QToolBar(MiniDrawClass);
        mainToolBar->setObjectName(QStringLiteral("mainToolBar"));
        MiniDrawClass->addToolBar(Qt::TopToolBarArea, mainToolBar);
        statusBar = new QStatusBar(MiniDrawClass);
        statusBar->setObjectName(QStringLiteral("statusBar"));
        MiniDrawClass->setStatusBar(statusBar);

        retranslateUi(MiniDrawClass);

        QMetaObject::connectSlotsByName(MiniDrawClass);*/
    } // setupUi

    void retranslateUi(QMainWindow *MiniDrawClass)
    {
        MiniDrawClass->setWindowTitle(QApplication::translate("MiniDrawClass", "MiniDraw", Q_NULLPTR));
    } // retranslateUi
	
};

namespace Ui {
    class MiniDrawClass: public Ui_MiniDrawClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MINIDRAW_H
