/********************************************************************************
** Form generated from reading UI file 'widget.ui'
**
** Created by: Qt User Interface Compiler version 5.9.9
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_WIDGET_H
#define UI_WIDGET_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_Widget
{
public:
    QLabel *label_5;
    QTextEdit *EditRecv;
    QLabel *label_4;
    QPushButton *Send;
    QLabel *label;
    QLineEdit *EditPort;
    QLabel *label_2;
    QLineEdit *EditIP;
    QLabel *label_3;
    QLineEdit *EditSend;
    QPushButton *Listen;

    void setupUi(QWidget *Widget)
    {
        if (Widget->objectName().isEmpty())
            Widget->setObjectName(QStringLiteral("Widget"));
        Widget->resize(450, 402);
        label_5 = new QLabel(Widget);
        label_5->setObjectName(QStringLiteral("label_5"));
        label_5->setGeometry(QRect(230, 30, 41, 16));
        EditRecv = new QTextEdit(Widget);
        EditRecv->setObjectName(QStringLiteral("EditRecv"));
        EditRecv->setGeometry(QRect(40, 110, 371, 131));
        label_4 = new QLabel(Widget);
        label_4->setObjectName(QStringLiteral("label_4"));
        label_4->setGeometry(QRect(230, 20, 41, 16));
        Send = new QPushButton(Widget);
        Send->setObjectName(QStringLiteral("Send"));
        Send->setGeometry(QRect(280, 310, 131, 51));
        label = new QLabel(Widget);
        label->setObjectName(QStringLiteral("label"));
        label->setGeometry(QRect(40, 80, 111, 16));
        EditPort = new QLineEdit(Widget);
        EditPort->setObjectName(QStringLiteral("EditPort"));
        EditPort->setGeometry(QRect(280, 30, 131, 20));
        label_2 = new QLabel(Widget);
        label_2->setObjectName(QStringLiteral("label_2"));
        label_2->setGeometry(QRect(40, 30, 71, 16));
        EditIP = new QLineEdit(Widget);
        EditIP->setObjectName(QStringLiteral("EditIP"));
        EditIP->setGeometry(QRect(110, 30, 113, 20));
        label_3 = new QLabel(Widget);
        label_3->setObjectName(QStringLiteral("label_3"));
        label_3->setGeometry(QRect(40, 250, 111, 16));
        EditSend = new QLineEdit(Widget);
        EditSend->setObjectName(QStringLiteral("EditSend"));
        EditSend->setGeometry(QRect(40, 280, 231, 81));
        Listen = new QPushButton(Widget);
        Listen->setObjectName(QStringLiteral("Listen"));
        Listen->setGeometry(QRect(280, 60, 131, 41));

        retranslateUi(Widget);

        QMetaObject::connectSlotsByName(Widget);
    } // setupUi

    void retranslateUi(QWidget *Widget)
    {
        Widget->setWindowTitle(QApplication::translate("Widget", "Widget", Q_NULLPTR));
        label_5->setText(QApplication::translate("Widget", "\347\253\257\345\217\243\345\217\267\357\274\232", Q_NULLPTR));
        label_4->setText(QString());
        Send->setText(QApplication::translate("Widget", "\345\217\221\351\200\201", Q_NULLPTR));
        label->setText(QApplication::translate("Widget", "\346\230\276\347\244\272\344\277\241\346\201\257:", Q_NULLPTR));
        label_2->setText(QApplication::translate("Widget", "\346\234\215\345\212\241\347\253\257\345\234\260\345\235\200\357\274\232", Q_NULLPTR));
        label_3->setText(QApplication::translate("Widget", "\345\217\221\351\200\201\344\277\241\346\201\257:", Q_NULLPTR));
        Listen->setText(QApplication::translate("Widget", "\345\274\200\345\247\213\350\201\212\345\244\251", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class Widget: public Ui_Widget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_WIDGET_H
