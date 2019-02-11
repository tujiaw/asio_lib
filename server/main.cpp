
#include <iostream>
#include "asio/proto/pb_base.pb.h"
#include "asio/msgclient.h"
#include "asio/msgserver.h"
#include "asio/base/util.h"
#include <thread>

int main(int argc, char* argv[])
{
	util::initlog(argv[0], true);

#if 1
	LOG(INFO) << "server start...";
	MsgServer server(5566);
	server.addHandleMessage(PbBase::HelloReq::descriptor()->full_name(), [&](const SessionPtr &sessionPtr, const PackagePtr &reqPtr) {
		PbBase::HelloReq *hello = dynamic_cast<PbBase::HelloReq*>(reqPtr->msgPtr.get());

		std::shared_ptr<PbBase::HelloRsp> rspPtr(new PbBase::HelloRsp());
		rspPtr->set_hello(hello->address());
		sessionPtr->replyMessage(reqPtr, rspPtr);

		PbBase::ServerInfoPub *pub = new PbBase::ServerInfoPub();
		pub->set_hello("11111111");
		server.publishMessage(MessagePtr(pub));
	});
	server.addHandleMessage(PbBase::EchoReq::descriptor()->full_name(), [&](const SessionPtr &sessionPtr, const PackagePtr &reqPtr) {
		PbBase::EchoReq *req = static_cast<PbBase::EchoReq*>(reqPtr->msgPtr.get());
		PbBase::EchoRsp *rsp = new PbBase::EchoRsp();
		rsp->set_errorcode(0);
		rsp->set_content(req->content());
        //std::this_thread::sleep_for(std::chrono::seconds(2));
        LOG(INFO) << "ECHO request pacSize:" << reqPtr->header.pacSize << ", last:" << req->content().substr(req->content().length() - 3) << ", id:" << reqPtr->header.msgId;
		sessionPtr->replyMessage(reqPtr, MessagePtr(rsp));
	});

	server.run();
#else
	LOG(INFO) << "client start...";
	MsgClient client("127.0.0.1:5566");
	client.addHandlePublish(PbBase::ServerInfoPub::default_instance().GetTypeName(), [](int error, const MessagePtr &msg) {
		PbBase::ServerInfoPub *pub = static_cast<PbBase::ServerInfoPub*>(msg.get());
		LOG(INFO) << "publish:" << pub->hello();
	});
	client.start();

	while (1) {
		PbBase::EchoReq *msg = new PbBase::EchoReq();
		msg->set_content("hello");
		client.postMessage(MessagePtr(msg), [](int error, const PackagePtr &reqMsgPtr, const PackagePtr &rspMsgPtr) {
			if (error == 0) {
				PbBase::EchoRsp *rsp = static_cast<PbBase::EchoRsp*>(rspMsgPtr->msgPtr.get());
				LOG(INFO) << "response:" << rsp->content();
			}
		});
		std::this_thread::sleep_for(std::chrono::seconds(1));
	}
#endif

	while (1) { std::this_thread::sleep_for(std::chrono::seconds(1)); }
	return 0;
}

