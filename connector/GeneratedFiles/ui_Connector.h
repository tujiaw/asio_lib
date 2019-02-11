/********************************************************************************
** Form generated from reading UI file 'Connector.ui'
**
** Created by: Qt User Interface Compiler version 5.3.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_CONNECTOR_H
#define UI_CONNECTOR_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QDialog>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_ConnectorClass
{
public:
    QVBoxLayout *verticalLayout;
    QHBoxLayout *horizontalLayout;
    QLabel *label;
    QLineEdit *leAddress;
    QPushButton *pbConnect;
    QPushButton *pbDisconnect;
    QHBoxLayout *horizontalLayout_2;
    QLabel *label_2;
    QLineEdit *leIntervalMillisecond;
    QPushButton *pbStart;
    QPushButton *pbStop;
    QPushButton *pbHello;
    QSpacerItem *horizontalSpacer;
    QTextEdit *teSendData;
    QHBoxLayout *horizontalLayout_3;
    QLabel *label_3;
    QPushButton *pbClear;
    QSpacerItem *horizontalSpacer_2;
    QHBoxLayout *horizontalLayout_4;
    QListWidget *lwSendData;
    QListWidget *lwRecvData;
    QLabel *labelStatus;

    void setupUi(QDialog *ConnectorClass)
    {
        if (ConnectorClass->objectName().isEmpty())
            ConnectorClass->setObjectName(QStringLiteral("ConnectorClass"));
        ConnectorClass->resize(595, 407);
        verticalLayout = new QVBoxLayout(ConnectorClass);
        verticalLayout->setSpacing(6);
        verticalLayout->setContentsMargins(11, 11, 11, 11);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setSpacing(6);
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        label = new QLabel(ConnectorClass);
        label->setObjectName(QStringLiteral("label"));

        horizontalLayout->addWidget(label);

        leAddress = new QLineEdit(ConnectorClass);
        leAddress->setObjectName(QStringLiteral("leAddress"));

        horizontalLayout->addWidget(leAddress);

        pbConnect = new QPushButton(ConnectorClass);
        pbConnect->setObjectName(QStringLiteral("pbConnect"));

        horizontalLayout->addWidget(pbConnect);

        pbDisconnect = new QPushButton(ConnectorClass);
        pbDisconnect->setObjectName(QStringLiteral("pbDisconnect"));

        horizontalLayout->addWidget(pbDisconnect);


        verticalLayout->addLayout(horizontalLayout);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setSpacing(6);
        horizontalLayout_2->setObjectName(QStringLiteral("horizontalLayout_2"));
        label_2 = new QLabel(ConnectorClass);
        label_2->setObjectName(QStringLiteral("label_2"));

        horizontalLayout_2->addWidget(label_2);

        leIntervalMillisecond = new QLineEdit(ConnectorClass);
        leIntervalMillisecond->setObjectName(QStringLiteral("leIntervalMillisecond"));
        leIntervalMillisecond->setMaximumSize(QSize(50, 16777215));

        horizontalLayout_2->addWidget(leIntervalMillisecond);

        pbStart = new QPushButton(ConnectorClass);
        pbStart->setObjectName(QStringLiteral("pbStart"));

        horizontalLayout_2->addWidget(pbStart);

        pbStop = new QPushButton(ConnectorClass);
        pbStop->setObjectName(QStringLiteral("pbStop"));

        horizontalLayout_2->addWidget(pbStop);

        pbHello = new QPushButton(ConnectorClass);
        pbHello->setObjectName(QStringLiteral("pbHello"));

        horizontalLayout_2->addWidget(pbHello);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_2->addItem(horizontalSpacer);


        verticalLayout->addLayout(horizontalLayout_2);

        teSendData = new QTextEdit(ConnectorClass);
        teSendData->setObjectName(QStringLiteral("teSendData"));

        verticalLayout->addWidget(teSendData);

        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setSpacing(6);
        horizontalLayout_3->setObjectName(QStringLiteral("horizontalLayout_3"));
        label_3 = new QLabel(ConnectorClass);
        label_3->setObjectName(QStringLiteral("label_3"));

        horizontalLayout_3->addWidget(label_3);

        pbClear = new QPushButton(ConnectorClass);
        pbClear->setObjectName(QStringLiteral("pbClear"));

        horizontalLayout_3->addWidget(pbClear);

        horizontalSpacer_2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_3->addItem(horizontalSpacer_2);


        verticalLayout->addLayout(horizontalLayout_3);

        horizontalLayout_4 = new QHBoxLayout();
        horizontalLayout_4->setSpacing(6);
        horizontalLayout_4->setObjectName(QStringLiteral("horizontalLayout_4"));
        lwSendData = new QListWidget(ConnectorClass);
        lwSendData->setObjectName(QStringLiteral("lwSendData"));

        horizontalLayout_4->addWidget(lwSendData);

        lwRecvData = new QListWidget(ConnectorClass);
        lwRecvData->setObjectName(QStringLiteral("lwRecvData"));

        horizontalLayout_4->addWidget(lwRecvData);


        verticalLayout->addLayout(horizontalLayout_4);

        labelStatus = new QLabel(ConnectorClass);
        labelStatus->setObjectName(QStringLiteral("labelStatus"));

        verticalLayout->addWidget(labelStatus);


        retranslateUi(ConnectorClass);

        QMetaObject::connectSlotsByName(ConnectorClass);
    } // setupUi

    void retranslateUi(QDialog *ConnectorClass)
    {
        ConnectorClass->setWindowTitle(QApplication::translate("ConnectorClass", "Connector", 0));
        label->setText(QApplication::translate("ConnectorClass", "Address:", 0));
        pbConnect->setText(QApplication::translate("ConnectorClass", "Connect", 0));
        pbDisconnect->setText(QApplication::translate("ConnectorClass", "Disconnect", 0));
        label_2->setText(QApplication::translate("ConnectorClass", "Send data:", 0));
        leIntervalMillisecond->setText(QApplication::translate("ConnectorClass", "1000", 0));
        pbStart->setText(QApplication::translate("ConnectorClass", "Start", 0));
        pbStop->setText(QApplication::translate("ConnectorClass", "Stop", 0));
        pbHello->setText(QApplication::translate("ConnectorClass", "Hello", 0));
        label_3->setText(QApplication::translate("ConnectorClass", "Recieve data", 0));
        pbClear->setText(QApplication::translate("ConnectorClass", "Clear", 0));
        labelStatus->setText(QString());
    } // retranslateUi

};

namespace Ui {
    class ConnectorClass: public Ui_ConnectorClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CONNECTOR_H
