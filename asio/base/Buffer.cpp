#include "Buffer.h"
#include <boost/asio.hpp>

void Buffer::appendInt32(__int32 x)
{
	append(&x, sizeof(x));
}

void Buffer::appendInt16(__int16 x)
{
	append(&x, sizeof(x));
}

void Buffer::appendInt8(__int8 x)
{
	append(&x, sizeof(x));
}

__int32 Buffer::readInt32()
{
	__int32 result = peekInt32();
	retrieveInt32();
	return result;
}

__int16 Buffer::readInt16()
{
	__int16 result = peekInt16();
	retrieveInt16();
	return result;
}

__int8 Buffer::readInt8()
{
	__int8 result = peekInt8();
	retrieveInt8();
	return result;
}

__int32 Buffer::peekInt32() const
{
	assert(readableBytes() >= sizeof(__int32));
	__int32 be32 = 0;
	::memcpy(&be32, peek(), sizeof(be32));
	return be32;
}

__int16 Buffer::peekInt16() const
{
	assert(readableBytes() >= sizeof(__int16));
	__int16 be16 = 0;
	::memcpy(&be16, peek(), sizeof(be16));
	return be16;
}

__int8 Buffer::peekInt8() const
{
	assert(readableBytes() >= sizeof(__int8));
	__int8 x = *peek();
	return x;
}

void Buffer::prependInt32(__int32 x)
{
	prepend(&x, sizeof(x));
}

void Buffer::prependInt16(__int16 x)
{
	prepend(&x, sizeof(x));
}

void Buffer::prependInt8(__int8 x)
{
	prepend(&x, sizeof(x));
}
