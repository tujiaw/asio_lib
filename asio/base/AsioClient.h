#ifndef ASIO_BASE_ASIOCLIENT_H_
#define ASIO_BASE_ASIOCLIENT_H_

#include <boost/asio.hpp>
#include <thread>
#include <deque>
#include <atomic>
#include <mutex>
#include <condition_variable>
#include "desc.h"
#include "Buffer.h"

using boost::asio::ip::tcp;
class AsioClient {
public:
    class MsgCache {
    public:
        MsgCache(AsioClient *client, const PackagePtr &p, const Response &r, int msTimeout)
            : pac(p), res(r), timer(client->io_, boost::posix_time::milliseconds(msTimeout)) { 
            timer.async_wait(std::bind(&AsioClient::onTimeout, client, std::placeholders::_1, p->header.msgId));
        }
        ~MsgCache() {
            timer.cancel_one();
        }
        PackagePtr pac;
        Response res;
        boost::asio::deadline_timer timer;
    };
    typedef std::shared_ptr<MsgCache> MsgCachePtr;

	explicit AsioClient(const std::string &address, int heartbeatSeconds = 0);
	~AsioClient();
	AsioClient(const AsioClient&) = delete;
	AsioClient& operator=(const AsioClient&) = delete;

	void addHandlePublish(const std::string &typeName, const PublishFunc &func);
	void start();
	void stop();
	bool stopped() const;

    int sendMessage(const MessagePtr &msgPtr, MessagePtr &rspPtr, int msTimeout = kMsTimeout);
    int postMessage(const MessagePtr &msgPtr, const Response &res, int msTimeout = kMsTimeout);
	int postOrderMessage(const MessagePtr &msgPtr, const Response &res, int msTimeout = kMsTimeout);

private:
	int postMessage(bool isOrder, const MessagePtr &msgPtr, const Response &res, int msTimeout = kMsTimeout);
	void close();
	void onRead();
	void onWrite();
    void onTimeout(boost::system::error_code err, int msgId);
	void doClose();
	void doSubscribe();
	void onHeartbeat();
	void doHeartbeat(const boost::system::error_code &e);
    void doResponse(const PackagePtr &pack);

private:
	std::string address_;
	boost::asio::io_context io_;
    boost::asio::io_context ioDoResponse_;
    boost::asio::io_context::work workDoResponse_;
	tcp::socket socket_;
	std::deque<PackagePtr> pendingList_;
	std::atomic<int> id_;
	std::thread runthread_;
    std::thread threadDoResponse_;
	std::unique_ptr<boost::asio::deadline_timer> heartbeatTimer_;
	int heartbeatSeconds_;
	std::atomic<bool> isOnline_;

    static const int kTempBufSize = boost::asio::detail::default_max_transfer_size;
    char tempBuf_[kTempBufSize];
	Buffer readBuffer_;

	std::mutex mutex_;
	std::condition_variable cond_;

	std::mutex responseMutex_;
    std::map<int, MsgCachePtr> responseMap_;
	std::map<std::string, PublishFunc> publishMap_;
};

#endif // ASIO_BASE_ASIOCLIENT_H_