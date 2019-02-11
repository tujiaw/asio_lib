#include "AsioServer.h"
#include <iostream>
#include <boost/lexical_cast.hpp>
#include "ProtoHelp.h"
#include "session.h"
#include "util.h"
#include "asio/proto/pb_base.pb.h"

AsioServer::AsioServer(unsigned short port)
	: acceptor_(io_, tcp::endpoint(tcp::v4(), port))
	, socket_(io_)
{
	init();
	onAccept();
}


AsioServer::~AsioServer()
{
}

void AsioServer::run()
{
    io_.run();
	//runthread_ = std::move(std::thread([this]() {
	//	io_.run();
	//}));
}

void AsioServer::stop()
{
	if (!io_.stopped()) {
		io_.stop();
        if (runthread_.joinable()) {
            runthread_.join();
        }
	}
}

void AsioServer::init()
{
	this->addHandleMessage(PbBase::SubscribeReq::descriptor()->full_name(), [](const SessionPtr &sessionPtr, const PackagePtr &reqPtr) {
		PbBase::SubscribeReq *msg = static_cast<PbBase::SubscribeReq*>(reqPtr->msgPtr.get());
		for (int i = 0; i < msg->typenamelist_size(); i++) {
			if (msg->type() == 1) {
				sessionPtr->addSubscribe(msg->typenamelist(i));
			} else {
				sessionPtr->removeSubscribe(msg->typenamelist(i));
			}
		}
		std::shared_ptr<PbBase::SubscribeRsp> rspPtr(new PbBase::SubscribeRsp());
		rspPtr->set_errorcode(0);
		sessionPtr->replyMessage(reqPtr, rspPtr);
	});
	this->addHandleMessage(PbBase::HeartbeatReq::descriptor()->full_name(), [](const SessionPtr &sessionPtr, const PackagePtr &reqPtr) {
		PbBase::HeartbeatReq *msg = static_cast<PbBase::HeartbeatReq*>(reqPtr->msgPtr.get());
		// LOG(INFO) << "host:" << sessionPtr->remoteEndpoint() << ",cpu:" << msg->cpu() << ",memory:" << msg->memory();
		PbBase::HeartbeatRsp *rsp = new PbBase::HeartbeatRsp();
		rsp->set_servertime(util::currentMillisecond());
		sessionPtr->replyMessage(reqPtr, MessagePtr(rsp));
	});
}

void AsioServer::onAccept()
{
	acceptor_.async_accept(socket_,
		[this](boost::system::error_code ec)
	{
		if (!ec) {
			std::make_shared<Session>(&socket_)->start();
		} else {
			LOG(ERROR) << "async accept error:" << ec.message();
		}
		onAccept();
	});
}

void AsioServer::addHandleMessage(const std::string &protoName, const Task &task)
{
	TaskManager::instance()->addHandleTask(protoName, task);
}

void AsioServer::publishMessage(const MessagePtr &msg)
{
	SessionManager::instance()->publishMessage(msg);
}
