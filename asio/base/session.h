#ifndef ASIO_BASE_SESSION_H_
#define ASIO_BASE_SESSION_H_

#include "desc.h"
#include <mutex>

class Session : public std::enable_shared_from_this<Session>
{
public:
	explicit Session(void* socket);
	~Session();
	Session(const Session&) = delete;
	Session& operator=(const Session&) = delete;
	
	void start();
	void replyMessage(const PackagePtr &req, const MessagePtr &rspMsg);
	void publishMessage(const MessagePtr &msg);
	void addSubscribe(const std::string &typeName);
	void removeSubscribe(const std::string &typeName);
	std::string remoteEndpoint() const;
	int sessionId() const;

private:
	void onRead();
	void onWrite(BufferPtr writeBuffer);
	void postPackage(const PackagePtr &pack);

private:
	friend struct SessionData;
	SessionData *d;
};

class SessionManager
{
public:
	static SessionManager* instance();
	void addSession(Session *session);
	void removeSession(Session *session);
	void publishMessage(const MessagePtr &msg);

private:
	static SessionManager *s_inst;
	std::mutex mutex_;
	std::vector<Session*> sessionList_;
};

#endif // ASIO_BASE_SESSION_H_