#include "asio/msgclient.h"
#include "asio/base/AsioClient.h"

MsgClient::MsgClient(const std::string &address, int heartbeatSeconds)
    : d_ptr(new AsioClient(address, heartbeatSeconds))
    , address_(address) {}

MsgClient::~MsgClient() { }

void MsgClient::addHandlePublish(const std::string &typeName, const PublishFunc &func) {
    if (d_func()->stopped()) {
        LOG(WARNING) << "service stopped";
        return;
    }
    d_func()->addHandlePublish(typeName, func);
}

void MsgClient::start() {
    if (d_func()->stopped()) {
        d_ptr.reset(new AsioClient(address_));
    }
    d_func()->start();
}

void MsgClient::stop() {
    d_func()->stop();
}

int MsgClient::sendMessage(const MessagePtr &msgPtr, MessagePtr &rspPtr, int msTimeout) {
    if (d_func()->stopped()) {
        LOG(WARNING) << "service stopped";
        return eDisconnect;
    }
    return d_func()->sendMessage(msgPtr, rspPtr, msTimeout);
}

int MsgClient::postMessage(const MessagePtr &msgPtr, const Response &res, int msTimeout) {
    if (d_func()->stopped()) {
        LOG(WARNING) << "service stopped";
        return eDisconnect;
    }
    return d_func()->postMessage(msgPtr, res, msTimeout);
}

int MsgClient::postOrderMessage(const MessagePtr &msgPtr, const Response &res, int msTimeout)
{
	if (d_func()->stopped()) {
		LOG(WARNING) << "service stopped";
		return eDisconnect;
	}
	return d_func()->postOrderMessage(msgPtr, res, msTimeout);
}
