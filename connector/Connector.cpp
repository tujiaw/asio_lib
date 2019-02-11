#include "Connector.h"
#include <QHostAddress>
#include <QMessageBox>
#include <QTimer>
#include "asio/proto/pb_base.pb.h"
#include "asio/msgclient.h"

Connector::Connector(QWidget *parent)
	: QDialog(parent), id_(100), timer_(new QTimer(this))
	, conn_("127.0.0.1:5566", 5)
{
	ui.setupUi(this);

	connect(ui.pbConnect, &QPushButton::clicked, this, &Connector::onConnect);
	connect(ui.pbDisconnect, &QPushButton::clicked, this, &Connector::onDisconnect);
	connect(ui.pbStart, &QPushButton::clicked, this, &Connector::onStart);
	connect(ui.pbStop, &QPushButton::clicked, this, &Connector::onStop);
	connect(ui.pbClear, &QPushButton::clicked, this, &Connector::onRecvDataClear);
	connect(ui.pbHello, &QPushButton::clicked, this, &Connector::onHello);
	connect(timer_, &QTimer::timeout, this, &Connector::onTimer);
	connect(this, &Connector::sigResult, this, &Connector::onResult, Qt::QueuedConnection);

	ui.leAddress->setText("127.0.0.1:5566");
	ui.teSendData->setText("hello, world!");
}

void Connector::onConnect()
{
	conn_.start();
}

void Connector::onDisconnect()
{
	conn_.stop();
}

void Connector::onStart()
{
	int ms = ui.leIntervalMillisecond->text().toInt();
	if (ms <= 0) {
		return;
	}

	if (timer_->interval() != ms) {
		timer_->setInterval(ms);
		timer_->stop();
	}
	timer_->start();
}

void Connector::onStop()
{
	timer_->stop();
}

void Connector::onTimer()
{
	sendMsg();
}

void Connector::onResult(const QString &text)
{
	if (ui.lwRecvData->count() > 1000) {
		ui.lwRecvData->clear();
	}
	ui.lwRecvData->addItem(text);
	ui.lwRecvData->scrollToBottom();
}

void Connector::onRecvDataClear()
{
	ui.lwSendData->clear();
	ui.lwRecvData->clear();
}

void Connector::onHello()
{
	sendMsg();
}

void Connector::sendMsg()
{
	++id_;
    std::string content = (QString::number(id_) + ":" + ui.teSendData->toPlainText()).toStdString();
	MessagePtr msgPtr(new PbBase::EchoReq());
    PbBase::EchoReq *req = (PbBase::EchoReq*)msgPtr.get();
    req->set_content(content);

    conn_.postMessage(msgPtr, [this, content](int error, const PackagePtr &reqMsgPtr, const PackagePtr &rspMsgPtr) {
        if (error) {
            LOG(ERROR) << "on post error:" << error;
        } else {
            PbBase::EchoRsp *msg = static_cast<PbBase::EchoRsp*>(rspMsgPtr->msgPtr.get());
            if (msg->content() == content) {
                LOG(INFO) << rspMsgPtr->header.msgId << ",ok";
				int pos = msg->content().find_first_of(':');
				QString rspId = QString::fromStdString(msg->content().substr(0, pos));
				emit sigResult(rspId);
            } else {
                LOG(ERROR) << "failed";
            }
        }
    });

	if (ui.lwSendData->count() > 1000) {
		ui.lwSendData->clear();
	}
	ui.lwSendData->addItem(QString::number(id_));
	ui.lwSendData->scrollToBottom();

    //MessagePtr rsp;
    //if (0 == conn_.sendMessage(msgPtr, rsp)) {
    //    PbBase::EchoRsp *msg = static_cast<PbBase::EchoRsp*>(rsp.get());
    //    std::string resContent = msg->content();
    //    QByteArray xx = QString::fromStdString(resContent).toLocal8Bit();
    //    if (resContent == content) {
    //        std::string id = content.substr(0, content.find(':'));
    //        //ui.lwRecvData->addItem(QString::fromStdString(id));
    //        LOG(INFO) << id << ",ok";
    //    }
    //}
}
