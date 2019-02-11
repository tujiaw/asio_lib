#pragma once

#include <QtWidgets/QDialog>
#include "ui_Connector.h"
#include <QTcpSocket>
#include "asio/msgclient.h"

class QTimer;
class Connector : public QDialog
{
	Q_OBJECT

public:
	Connector(QWidget *parent = Q_NULLPTR);

	void onConnect();
	void onDisconnect();
	void onStart();
	void onStop();
	void onRecvDataClear();
	void onHello();

	void sendMsg();

signals:
	void sigResult(const QString &text);

public slots:
	void onTimer();
	void onResult(const QString &text);

private:
	Ui::ConnectorClass ui;
	int id_;
	QTimer *timer_;
	MsgClient conn_;
};
