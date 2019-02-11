#include "asio/msgserver.h"
#include "base/AsioServer.h"

MsgServer::MsgServer(unsigned short port)
    : d_ptr(new AsioServer(port)) { }

MsgServer::~MsgServer() { }
void MsgServer::run() { d_func()->run(); }
void MsgServer::stop() { d_func()->stop(); }
void MsgServer::addHandleMessage(const std::string &protoName, const Task &task) {
    d_func()->addHandleMessage(protoName, task);
}
void MsgServer::publishMessage(const MessagePtr &msg) {
    d_func()->publishMessage(msg);
}
