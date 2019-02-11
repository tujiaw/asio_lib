#include "ProtoHelp.h"
#include <boost/asio.hpp>
#include <zlib.h>
#include "Buffer.h"
#include "util.h"

using namespace boost::asio::detail;

static const Package emptyPackage;
static const int kFlagLen = sizeof(emptyPackage.header.flag);
static const int kMaxPackageLen = 1024 * 1024 * 100;

int ProtoHelp::net2int(const char *buf)
{
    int result = 0;
    memcpy(&result, buf, sizeof result);
    return socket_ops::network_to_host_long(result);
}

int ProtoHelp::net2int(int i)
{
    return socket_ops::network_to_host_long(i);
}

int ProtoHelp::int2net(int i)
{
    return socket_ops::host_to_network_long(i);
}

bool ProtoHelp::encode(const PackagePtr &package, BufferPtr &buffer)
{
    std::string content;
    content.resize(package->msgPtr->ByteSize());
    if (package->msgPtr->SerializePartialToArray(&content[0], content.size())) {
        if (util::enableCompressSize() >= 0 && content.size() > util::enableCompressSize()) {
            unsigned long bufferSize = compressBound(content.size());
            std::string buffer(bufferSize, 0);
            int errcode = compress((unsigned char*)&buffer[0], &bufferSize, (unsigned char*)&content[0], content.size());
            if (errcode == Z_OK) {
                package->header.iszip = 1;
                content.assign(&buffer[0], bufferSize);
            } else {
                LOG(ERROR) << "compress error:" << errcode;
            }
        }

        package->typeName = package->msgPtr->GetTypeName();
        package->header.typeNameLen = package->typeName.size();
        package->header.msgSize = package->msgPtr->ByteSize();
        package->header.pacSize = kPackageHeaderSize + package->typeName.size() + content.size();
        buffer->append(&package->header, kPackageHeaderSize);
        buffer->append(&package->typeName[0], package->header.typeNameLen);
        buffer->append(content);
        return true;
    }
    return false;
}

PackagePtr ProtoHelp::decode(Buffer &buf)
{
    auto gotoFlag = [](Buffer &buf) -> bool {
        while (buf.readableBytes() > kPackageHeaderSize) {
            const char *p = buf.peek();
            if (p[0] == 'P' && p[1] == 'P') {
                return true;
            } else {
                LOG(INFO) << "flag error";
                buf.retrieve(1);
            }
        }
        return false;
    };

    if (!gotoFlag(buf)) {
        LOG(ERROR) << "go to flag error";
        return nullptr;
    }

    PacHeader header;
    memcpy(&header, buf.peek(), kPackageHeaderSize);

    // 数据不够等待下次读取更多的数据
    if (buf.readableBytes() < (std::size_t)header.pacSize) {
        return nullptr;
    }

    buf.retrieve(kPackageHeaderSize);
    if (header.pacSize < 0 || header.pacSize > kMaxPackageLen) {
        LOG(INFO) << "pac size error:" << header.pacSize;
        return decode(buf);
    }

    if (header.typeNameLen <= 0 || header.typeNameLen > 1024) {
        LOG(INFO) << "type name len error:" << header.typeNameLen;
        return decode(buf);
    }

    std::string typeName = buf.retrieveAsString(header.typeNameLen);
    google::protobuf::Message *msg = createMessage(typeName);;
    if (!msg) {
        LOG(ERROR) << "create message failed:" << typeName;
        return decode(buf);
    }

    bool parseResult = false;
    int remainSize = header.pacSize - kPackageHeaderSize - header.typeNameLen;
    if (header.iszip) {
        unsigned long bufferSize = header.msgSize;
        std::string buffer(bufferSize, 0);
        int errcode = uncompress((unsigned char*)&buffer[0], &bufferSize, (unsigned char*)buf.peek(), remainSize);
        if (errcode == Z_OK) {
            parseResult = msg->ParseFromArray(&buffer[0], bufferSize);
        } else {
            LOG(ERROR) << "uncompress error:" << errcode;
        }
    } else {
        parseResult = msg->ParseFromArray(buf.peek(), header.msgSize);
    }

    if (parseResult) {
        buf.retrieve(remainSize);
        PackagePtr result(new Package());
        result->header = header;
        result->typeName = typeName;
        result->msgPtr = MessagePtr(msg);
        return result;
    }

    LOG(ERROR) << "decode failed:" << typeName;
    return nullptr;
}

google::protobuf::Message* ProtoHelp::createMessage(const std::string & typeName)
{
    google::protobuf::Message *message = nullptr;
    const google::protobuf::Descriptor* desc = google::protobuf::DescriptorPool::generated_pool()->FindMessageTypeByName(typeName);
    if (desc) {
        const google::protobuf::Message *prototype = google::protobuf::MessageFactory::generated_factory()->GetPrototype(desc);
        if (prototype) {
            message = prototype->New();
        }
    }
    return message;
}