#include "Session.h"
#include "ProtoHelp.h"
#include <boost/asio.hpp>
#include <boost/lexical_cast.hpp>
#include <thread>
#include "Buffer.h"
#include "TaskManager.h"

using boost::asio::ip::tcp;

static const int kTempBufSize = boost::asio::detail::default_max_transfer_size;
static std::atomic<int> s_sessionId = 0;

std::string endpoint2str(const tcp::endpoint &endpoint)
{
	std::string host = endpoint.address().to_string();
	host += (":" + boost::lexical_cast<std::string>(endpoint.port()));
	return host;
}

struct SessionData {
	SessionData(void* socket) 
		: socket_(std::move(*(tcp::socket*)socket))
		, io_(socket_.get_io_service())
        , id_(0)
		, sessionId_(s_sessionId++)
	{
	}

	tcp::socket socket_;
	boost::asio::io_context &io_;
	char tempBuf_[kTempBufSize];
	Buffer readBuffer_;
	std::mutex subscribeMutex_;
	std::vector<std::string> subscribeList_;
    std::atomic<int> id_;
	int sessionId_;
};

Session::Session(void* socket)
	: d(new SessionData(socket))
{
    std::cout << "Session create:" << remoteEndpoint() << std::endl;
	SessionManager::instance()->addSession(this);
}

Session::~Session()
{
	SessionManager::instance()->removeSession(this);
	std::cout << "Session delete:" << remoteEndpoint() << std::endl;
	delete d;
}

void Session::start()
{
	onRead();
}

void Session::replyMessage(const PackagePtr &req, const MessagePtr &rspMsg)
{ 
	if (!req || !rspMsg) {
		return;
	}
	PackagePtr rspPtr(new Package());
    rspPtr->header = req->header;
	rspPtr->typeName = rspMsg->GetTypeName();
	rspPtr->msgPtr = rspMsg;
	postPackage(rspPtr);
}

void Session::publishMessage(const MessagePtr &msg)
{
	if (!msg) {
		return;
	}

	{
		std::lock_guard<std::mutex> lock(d->subscribeMutex_);
		if (std::find(d->subscribeList_.begin(), d->subscribeList_.end(), msg->GetTypeName()) == d->subscribeList_.end()) {
			return;
		}
	}

	PackagePtr pack(new Package());
    pack->header.msgType = PacHeader::PUBSUB;
    pack->header.msgId = ++d->id_ % INT_MAX;
	pack->header.typeNameLen = msg->GetTypeName().length();
	pack->typeName = msg->GetTypeName();
	pack->msgPtr = msg;
	postPackage(pack);
}

void Session::addSubscribe(const std::string &typeName)
{
	std::lock_guard<std::mutex> lock(d->subscribeMutex_);
	if (std::find(d->subscribeList_.begin(), d->subscribeList_.end(), typeName) == d->subscribeList_.end()) {
		d->subscribeList_.push_back(typeName);
	}
}

void Session::removeSubscribe(const std::string &typeName)
{
	std::lock_guard<std::mutex> lock(d->subscribeMutex_);
	auto it = std::find(d->subscribeList_.begin(), d->subscribeList_.end(), typeName);
	if (it != d->subscribeList_.end()) {
		d->subscribeList_.erase(it);
	}
}

std::string Session::remoteEndpoint() const
{
	return endpoint2str(d->socket_.remote_endpoint());
}

int Session::sessionId() const
{
	return d->sessionId_;
}

void Session::onRead()
{
	auto self(shared_from_this());
	d->socket_.async_read_some(boost::asio::buffer(d->tempBuf_, kTempBufSize),
        [this, self](boost::system::error_code ec, std::size_t length) {
		if (!ec) {
            d->readBuffer_.append(d->tempBuf_, length);
			do {
				PackagePtr pack = ProtoHelp::decode(d->readBuffer_);
				if (pack) {
                    LOG(INFO) << "session onread:" << pack->header.msgId;
					TaskManager::instance()->handleMessage(pack, self);
				} else {
					break;
				}
			} while (d->readBuffer_.readableBytes() > 0);

			onRead();
		} else {
			LOG(INFO) << "Session::onRead error:" << ec.message() << "(" << ec << ")";
		}
	});
}

void Session::onWrite(BufferPtr writeBuffer)
{
	int writeLen = writeBuffer->readableBytes();
	if (writeLen <= 0) {
		return;
	}

	auto self(shared_from_this());
	boost::asio::async_write(d->socket_, boost::asio::buffer(writeBuffer->peek(), writeLen),
		[this, self, writeBuffer](boost::system::error_code ec, std::size_t length)
	{
		if (!ec) {
			writeBuffer->retrieve(length);
			onWrite(writeBuffer);
		}
	});
}

void Session::postPackage(const PackagePtr &pack)
{
    BufferPtr writeBuffer(new Buffer());
    if (ProtoHelp::encode(pack, writeBuffer)) {
        d->io_.post([this, writeBuffer]{ onWrite(writeBuffer); });
    }
}

//////////////////////////////////////////////////////////////////////////
SessionManager* SessionManager::s_inst = nullptr;
SessionManager* SessionManager::instance()
{
	static std::once_flag flag;
	std::call_once(flag, []() {
		s_inst = new SessionManager();
	});
	return s_inst;
}

void SessionManager::addSession(Session *session)
{
	std::lock_guard<std::mutex> lock(mutex_);
	sessionList_.push_back(session);
}

void SessionManager::removeSession(Session *session)
{
	std::lock_guard<std::mutex> lock(mutex_);
	auto it = std::find(sessionList_.begin(), sessionList_.end(), session);
	if (it != sessionList_.end()) {
		sessionList_.erase(it);
	}
}

void SessionManager::publishMessage(const MessagePtr &msg)
{
	std::lock_guard<std::mutex> lock(mutex_);
	for (auto it = sessionList_.begin(); it != sessionList_.end(); ++it) {
		(*it)->publishMessage(msg);
	}
}