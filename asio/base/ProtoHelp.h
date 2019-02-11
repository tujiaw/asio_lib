#ifndef ASIO_BASE_PROTOHELP_H_
#define ASIO_BASE_PROTOHELP_H_

#include "desc.h"
#include <string>

class Buffer;
class ProtoHelp
{
public:
	static int crc32(const char *start, int len);
	static int net2int(const char *buf);
	static int net2int(int i);
	static int int2net(int i);
    static bool encode(const PackagePtr &package, BufferPtr &buffer);
	static PackagePtr decode(Buffer &buf);
	static google::protobuf::Message* createMessage(const std::string & typeName);
};

#endif // ASIO_BASE_PROTOHELP_H_
