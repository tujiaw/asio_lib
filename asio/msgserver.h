#ifndef ASIO_MSGSERVER_H_
#define ASIO_MSGSERVER_H_

#include "base/desc.h"
#include "base/session.h"

class MsgServer {
 public:
    explicit MsgServer(unsigned short port);
    ~MsgServer();
    MsgServer(const MsgServer&) = delete;
    MsgServer& operator=(const MsgServer&) = delete;

    void run();
    void stop();
    void addHandleMessage(const std::string &protoName, const Task &task);
    void publishMessage(const MessagePtr &msg);

 private:
    D_PRIVATE(AsioServer)
};

#endif  // ASIO_MSGSERVER_H_
