/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.12.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_QCefViewTestClass
{
public:
    QWidget *centralWidget;
    QWidget *cefContainer;
    QVBoxLayout *verticalLayout_cefContainer;
    QLineEdit *lineEdit_url;
    QPushButton *btn_forward;
    QPushButton *btn_back;
    QPushButton *btn_stop;
    QPushButton *btn_reload;

    void setupUi(QMainWindow *QCefViewTestClass)
    {
        if (QCefViewTestClass->objectName().isEmpty())
            QCefViewTestClass->setObjectName(QString::fromUtf8("QCefViewTestClass"));
        QCefViewTestClass->resize(1300, 824);
        centralWidget = new QWidget(QCefViewTestClass);
        centralWidget->setObjectName(QString::fromUtf8("centralWidget"));
        centralWidget->setStyleSheet(QString::fromUtf8("#centralWidget {\n"
"background-color: pink;\n"
"}"));
        cefContainer = new QWidget(centralWidget);
        cefContainer->setObjectName(QString::fromUtf8("cefContainer"));
        cefContainer->setGeometry(QRect(10, 50, 1280, 768));
        cefContainer->setStyleSheet(QString::fromUtf8("#cefContainer {\n"
"background-color: white;\n"
"}"));
        verticalLayout_cefContainer = new QVBoxLayout(cefContainer);
        verticalLayout_cefContainer->setSpacing(0);
        verticalLayout_cefContainer->setContentsMargins(11, 11, 11, 11);
        verticalLayout_cefContainer->setObjectName(QString::fromUtf8("verticalLayout_cefContainer"));
        verticalLayout_cefContainer->setContentsMargins(0, 0, 0, 0);
        lineEdit_url = new QLineEdit(centralWidget);
        lineEdit_url->setObjectName(QString::fromUtf8("lineEdit_url"));
        lineEdit_url->setGeometry(QRect(10, 20, 951, 20));
        btn_forward = new QPushButton(centralWidget);
        btn_forward->setObjectName(QString::fromUtf8("btn_forward"));
        btn_forward->setGeometry(QRect(1050, 20, 75, 23));
        btn_back = new QPushButton(centralWidget);
        btn_back->setObjectName(QString::fromUtf8("btn_back"));
        btn_back->setGeometry(QRect(970, 20, 75, 23));
        btn_stop = new QPushButton(centralWidget);
        btn_stop->setObjectName(QString::fromUtf8("btn_stop"));
        btn_stop->setGeometry(QRect(1210, 20, 75, 23));
        btn_reload = new QPushButton(centralWidget);
        btn_reload->setObjectName(QString::fromUtf8("btn_reload"));
        btn_reload->setGeometry(QRect(1130, 20, 75, 23));
        QCefViewTestClass->setCentralWidget(centralWidget);

        retranslateUi(QCefViewTestClass);

        QMetaObject::connectSlotsByName(QCefViewTestClass);
    } // setupUi

    void retranslateUi(QMainWindow *QCefViewTestClass)
    {
        QCefViewTestClass->setWindowTitle(QApplication::translate("QCefViewTestClass", "QCefViewTest", nullptr));
        btn_forward->setText(QApplication::translate("QCefViewTestClass", "Forward", nullptr));
        btn_back->setText(QApplication::translate("QCefViewTestClass", "Back", nullptr));
        btn_stop->setText(QApplication::translate("QCefViewTestClass", "Stop", nullptr));
        btn_reload->setText(QApplication::translate("QCefViewTestClass", "Reload", nullptr));
    } // retranslateUi

};

namespace Ui {
    class QCefViewTestClass: public Ui_QCefViewTestClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
