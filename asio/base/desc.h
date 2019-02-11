#ifndef ASIO_BASE_DESC_H_
#define ASIO_BASE_DESC_H_

#include <functional>
#include <memory>
#include "google/protobuf/message.h"

#ifndef GLOG_NO_ABBREVIATED_SEVERITIES
#define GLOG_NO_ABBREVIATED_SEVERITIES
#endif
#include <glog/logging.h>

class Session;
class Buffer;
struct Package;

typedef std::shared_ptr<Session> SessionPtr;
typedef std::shared_ptr<google::protobuf::Message> MessagePtr;
typedef std::shared_ptr<Package> PackagePtr;
typedef std::shared_ptr<Buffer> BufferPtr;

typedef std::function<void(const SessionPtr&, const PackagePtr&)> Task;
typedef std::function<void(int error, const PackagePtr &reqMsgPtr, const PackagePtr &rspMsgPtr)> Response;
typedef std::function<void(int error, const MessagePtr &pubMsgPtr)> PublishFunc;

#pragma pack(push,1)
struct PacHeader {
    enum MsgType {
        REQREP = 1,
        PUBSUB
    };

    PacHeader() : msgType(0), typeNameLen(0), extInfo(0), msgId(0), msgSize(0), pacSize(0) {
        flag[0] = 'P';
        flag[1] = 'P';
    }

    char flag[2];
    short msgType;
    short typeNameLen;
    union {
        short extInfo;
        struct {
            unsigned char iszip : 1;
            unsigned char isorder : 1;
        };
    };
    int msgId;
    int msgSize;
    int pacSize;
};
#pragma pack(pop)

const int kPackageHeaderSize = sizeof(PacHeader);
const int kMsTimeout = 3000;

struct Package {
    PacHeader header;
    std::string typeName;
    MessagePtr msgPtr;
};

enum ErrorCode {
    eSuccess = 0,
    eUnkown,
    eTimeout,
    eDisconnect,
    eServiceStopped,
};

#define D_PRIVATE(ClassNamePrivate)                                               \
    friend class ClassNamePrivate;                                                \
    std::unique_ptr<ClassNamePrivate> d_ptr;                                      \
    inline ClassNamePrivate* d_func(void) { return this->d_ptr.get(); }           \
    inline const ClassNamePrivate* d_func(void) const { return this->d_ptr.get(); }


#endif // ASIO_BASE_DESC_H_