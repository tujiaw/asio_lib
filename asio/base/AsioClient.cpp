#include "AsioClient.h"
#include "threadpool.h"
#include "ProtoHelp.h"
#include "util.h"
#include "asio/proto/pb_base.pb.h"

AsioClient::AsioClient(const std::string &address, int heartbeatSeconds) 
    : address_(address), 
    socket_(io_), 
    id_(0), 
    heartbeatSeconds_(heartbeatSeconds), 
    isOnline_(false), 
    workDoResponse_(ioDoResponse_)
{
}

AsioClient::~AsioClient()
{
	stop();
}

void AsioClient::start()
{
	LOG(INFO) << "connect to:" << address_;
	tcp::resolver resolver(io_);
	int pos = address_.find(":");
	assert(pos > 0);
	auto endpoints = resolver.resolve({ address_.substr(0, pos), address_.substr(pos + 1) });
    boost::asio::async_connect(socket_, endpoints, [this](boost::system::error_code ec, tcp::endpoint) {
		LOG(INFO) << "connect " << ec.message();
		isOnline_ = !ec;
		if (!ec) {
			onRead();
			doSubscribe();
			onHeartbeat();
		}
	});

	if (!runthread_.joinable()) {
		runthread_ = std::move(std::thread([this](){ io_.run(); }));
	}
    if (!threadDoResponse_.joinable()) {
        threadDoResponse_ = std::move(std::thread([this]() { ioDoResponse_.run(); }));
    }
}

void AsioClient::addHandlePublish(const std::string &typeName, const PublishFunc &func)
{
	publishMap_[typeName] = func;
}

void AsioClient::stop()
{
    io_.stop();
    ioDoResponse_.stop();
    close();
	if (runthread_.joinable()) {
		runthread_.join();
	}
    if (threadDoResponse_.joinable()) {
        threadDoResponse_.join();
    }
}

bool AsioClient::stopped() const
{
	return io_.stopped();
}

void AsioClient::close()
{
	io_.post(std::bind(&AsioClient::doClose, this));
}

int AsioClient::sendMessage(const MessagePtr &msgPtr, MessagePtr &rspPtr, int msTimeout)
{
	bool isWait = true;
	int err = postMessage(msgPtr, [&](int error, const PackagePtr &reqMsgPtr, const PackagePtr &rspMsgPtr) {
        if (!error) {
            rspPtr = rspMsgPtr->msgPtr;
        }
		err = error;
		std::unique_lock<std::mutex> lock(mutex_);
		isWait = false;
		this->cond_.notify_all();
	}, msTimeout);

	if (err != eSuccess) {
		return (int)err;
	}

	std::unique_lock<std::mutex> lock(mutex_);
	while (isWait) {
        cond_.wait(lock);
	}
	return err;
}

int AsioClient::postMessage(const MessagePtr &msgPtr, const Response &res, int msTimeout)
{
    return postMessage(false, msgPtr, res, msTimeout);
}

int AsioClient::postOrderMessage(const MessagePtr &msgPtr, const Response &res, int msTimeout)
{
	return postMessage(true, msgPtr, res, msTimeout);
}

int AsioClient::postMessage(bool isOrder, const MessagePtr &msgPtr, const Response &res, int msTimeout)
{
    if (io_.stopped()) {
        return eServiceStopped;
    }
    if (!isOnline_) {
        return eDisconnect;
    }

    PackagePtr pacPtr(new Package);
    pacPtr->header.msgType = PacHeader::REQREP;
    pacPtr->header.msgId = ++id_ % INT_MAX;
    pacPtr->header.typeNameLen = msgPtr->GetTypeName().length();
    pacPtr->header.isorder = isOrder ? 1 : 0;
    pacPtr->typeName = msgPtr->GetTypeName();
    pacPtr->msgPtr = msgPtr;

    {
        std::unique_lock<std::mutex> lock(responseMutex_);
        responseMap_[pacPtr->header.msgId] = MsgCachePtr(new MsgCache(this, pacPtr, res, msTimeout));
    }

    io_.post([this, pacPtr] {
        bool isProgress = !pendingList_.empty();
        pendingList_.push_back(pacPtr);
        if (!isProgress) {
            onWrite();
        }
    });
    return eSuccess;
}

void AsioClient::onRead()
{
	socket_.async_read_some(boost::asio::buffer(tempBuf_, kTempBufSize),
		[this](boost::system::error_code ec, std::size_t length) {
		if (ec) {
			LOG(ERROR) << "onRead error:" << ec.message();
			close();
			return;
		}
        
		readBuffer_.append(tempBuf_, length);
		do {
			PackagePtr pack = ProtoHelp::decode(readBuffer_);
			if (pack) {
                ioDoResponse_.post([this, pack]() { this->doResponse(pack); });
			} else {
				break;
			}
		} while (readBuffer_.readableBytes() > 0);

		onRead();
	});
}

void AsioClient::onWrite()
{
	if (pendingList_.empty()) {
		return;
	}

	PackagePtr pacPtr = pendingList_.front();
	pendingList_.pop_front();
    BufferPtr writeBuffer(new Buffer());
    if (ProtoHelp::encode(pacPtr, writeBuffer)) {
        LOG(INFO) << "write msgid:" << pacPtr->header.msgId;
        boost::asio::async_write(socket_, boost::asio::buffer(writeBuffer->peek(), writeBuffer->readableBytes()),
            [this, writeBuffer](std::error_code ec, std::size_t length) {
            if (!ec) {
                onWrite();
            } else {
                LOG(ERROR) << "async write error: " << ec.message();
                this->close();
            }
        });
    }
}

void AsioClient::onTimeout(boost::system::error_code err, int msgId)
{
    if (err == boost::asio::error::operation_aborted) {
        return;
    }

    MsgCachePtr rsp;
    {
        std::unique_lock<std::mutex> lock(responseMutex_);
        auto it = responseMap_.find(msgId);
        if (it != responseMap_.end()) {
			LOG(INFO) << "on timeout, msgid:" << msgId;
            rsp = it->second;
            responseMap_.erase(it);
        } else {
            LOG(ERROR) << "onTimeout id not find:" << msgId;
        }
    }
    if (rsp) {
        rsp->res(eTimeout, rsp->pac, nullptr);
    }
}

void AsioClient::doClose()
{
	try {
		if (socket_.is_open()) {
			socket_.shutdown(boost::asio::socket_base::shutdown_both);
			socket_.close();
		}
	}
	catch (std::exception &e) {
		LOG(ERROR) << "do close exception:" << e.what();
	}
	isOnline_ = false;
}

void AsioClient::doSubscribe()
{
	PbBase::SubscribeReq *msg(new PbBase::SubscribeReq());
	for (auto it = publishMap_.begin(); it != publishMap_.end(); ++it) {
		msg->add_typenamelist(it->first);
	}
	msg->set_type(1);
	postMessage(MessagePtr(msg), [](int error, const PackagePtr &reqMsgPtr, const PackagePtr &rspMsgPtr) {
		if (error == 0) {
			PbBase::SubscribeRsp *p = static_cast<PbBase::SubscribeRsp*>(rspMsgPtr->msgPtr.get());
			if (p->errorcode() == 0) {
				LOG(INFO) << "subscribe success...";
			} else {
				LOG(ERROR) << "subscribe failed, response error:" << p->errorcode();
			}
		} else {
			LOG(ERROR) << "subscribe failed, error:" << error;
		}
	});
}

void AsioClient::onHeartbeat()
{
	if (heartbeatSeconds_ > 0) {
		heartbeatTimer_.reset(new boost::asio::deadline_timer(io_, boost::posix_time::seconds(heartbeatSeconds_)));
		heartbeatTimer_->async_wait(std::bind(&AsioClient::doHeartbeat, this, std::placeholders::_1));
	}
}

void AsioClient::doHeartbeat(const boost::system::error_code &e)
{
	if (!e) {
		return;
	}

	PbBase::HeartbeatReq *msg(new PbBase::HeartbeatReq());
	msg->set_cpu(0);
	msg->set_memory(0);
	postMessage(MessagePtr(msg), [&](int error, const PackagePtr &reqMsgPtr, const PackagePtr &rspMsgPtr) {
		if (error == 0) {
			PbBase::HeartbeatRsp *rsp = static_cast<PbBase::HeartbeatRsp*>(rspMsgPtr->msgPtr.get());
			LOG(INFO) << "heartbeat " << util::getFormatTime(rsp->servertime());

			heartbeatTimer_->expires_at(heartbeatTimer_->expires_at() + boost::posix_time::seconds(heartbeatSeconds_));
			heartbeatTimer_->async_wait(std::bind(&AsioClient::doHeartbeat, this, std::placeholders::_1));
		}
	});
}

void AsioClient::doResponse(const PackagePtr &pack)
{
    if (pack->header.msgType == PacHeader::PUBSUB) {
        // 推送消息
        auto it = publishMap_.find(pack->typeName);
        if (it != publishMap_.end()) {
            it->second(0, pack->msgPtr);
        } else {
            LOG(WARNING) << "onRead publish not found:" << pack->typeName;
        }
    } else {
        // 应答消息
        MsgCachePtr rsp;
        {
            std::unique_lock<std::mutex> lock(responseMutex_);
            auto it = responseMap_.find(pack->header.msgId);
            if (it != responseMap_.end()) {
                rsp = it->second;
                responseMap_.erase(it);
            } else {
                LOG(ERROR) << "onRead id not find:" << pack->header.msgId;
            }
        }
        if (rsp) {
            rsp->res(eSuccess, rsp->pac, pack);
        }
    }
}
