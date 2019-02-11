#ifndef ASIO_BASE_BUFFER_H_
#define ASIO_BASE_BUFFER_H_

#include <vector>
#include <string>
#include <memory>
#include <assert.h>

class Buffer
{
public:
	static const size_t kCheapPrepend = 0;
	static const size_t kInitialSize = 2048;

	Buffer()
		: buffer_(kCheapPrepend + kInitialSize)
		, readerIndex_(kCheapPrepend)
		, writerIndex_(kCheapPrepend)
	{
		assert(readableBytes() == 0);
		assert(writableBytes() == kInitialSize);
		assert(prependableBytes() == kCheapPrepend);
	}

	void swap(Buffer &rhs)
	{
		buffer_.swap(rhs.buffer_);
		std::swap(readerIndex_, rhs.readerIndex_);
		std::swap(writerIndex_, rhs.writerIndex_);
	}

	size_t readableBytes() const
	{
		return writerIndex_ - readerIndex_;
	}

	size_t writableBytes() const
	{
		return buffer_.size() - writerIndex_;
	}

	size_t prependableBytes() const
	{
		return readerIndex_;
	}

	const char *peek() const
	{
		return begin() + readerIndex_;
	}

	void retrieve(size_t len)
	{
		//assert(len <= readableBytes());
		if (len > readableBytes()) {
			int i = 0;
		}
		if (len < readableBytes())
		{
			readerIndex_ += len;
		} else
		{
			retrieveAll();
		}
	}

	void retrieveUntil(const char *end)
	{
		assert(peek() <= end);
		assert(end <= beginWrite());
		retrieve(end - peek());
	}

	void retrieveInt32()
	{
		retrieve(sizeof(__int32));
	}

	void retrieveInt16()
	{
		retrieve(sizeof(__int16));
	}

	void retrieveInt8()
	{
		retrieve(sizeof(__int8));
	}

	void retrieveAll()
	{
		readerIndex_ = kCheapPrepend;
		writerIndex_ = kCheapPrepend;
	}

	std::string retrieveAllAsString()
	{
		return retrieveAsString(readableBytes());
	}

	std::string retrieveAsString(size_t len)
	{
		assert(len <= readableBytes());
		std::string result(peek(), len);
		retrieve(len);
		return result;
	}

	void append(const std::string &str)
	{
		append(str.c_str(), str.size());
	}

	void append(const char* data, size_t len)
	{
		ensureWritableBytes(len);
		std::copy(data, data + len, stdext::checked_array_iterator<char*>(beginWrite(), len));
		hasWritten(len);
	}

	void append(const void* data, size_t len)
	{
		append(static_cast<const char*>(data), len);
	}

	void ensureWritableBytes(size_t len)
	{
		if (writableBytes() < len)
		{
			makeSpace(len);
		}
		assert(writableBytes() >= len);
	}

	char* beginWrite()
	{
		return begin() + writerIndex_;
	}

	const char* beginWrite() const
	{
		return begin() + writerIndex_;
	}

	void hasWritten(size_t len)
	{
		writerIndex_ += len;
	}

	void appendInt32(__int32 x);
	void appendInt16(__int16 x);
	void appendInt8(__int8 x);
	__int32 readInt32();
	__int16 readInt16();
	__int8 readInt8();
	__int32 peekInt32() const;
	__int16 peekInt16() const;
	__int8 peekInt8() const;
	void prependInt32(__int32 x);
	void prependInt16(__int16 x);
	void prependInt8(__int8 x);

	bool Buffer::read(void* data, size_t len)
	{
		if (data && readableBytes() >= len)
		{
			memcpy(data, peek(), len);
			retrieve(len);
			return true;
		}
		return false;
	}

	void prepend(const void* data, size_t len)
	{
		assert(len <= prependableBytes());
		readerIndex_ -= len;
		const char *d = static_cast<const char*>(data);
		std::copy(d, d + len, stdext::checked_array_iterator<char*>(begin() + readerIndex_, len));
	}

private:
	char *begin()
	{
		return &*buffer_.begin();
	}

	const char* begin() const
	{
		return &*buffer_.begin();
	}

	void makeSpace(size_t len)
	{
		if (writableBytes() + prependableBytes() < len + kCheapPrepend)
		{
			buffer_.resize(writerIndex_ + len);
		} else
		{
			assert(kCheapPrepend < readerIndex_);
			size_t readable = readableBytes();
			std::copy(begin() + readerIndex_, begin() + writerIndex_,
				stdext::checked_array_iterator<char*>(begin() + kCheapPrepend, writerIndex_ - readerIndex_));
			readerIndex_ = kCheapPrepend;
			writerIndex_ = readerIndex_ + readable;
			assert(readable == readableBytes());
		}
	}

	Buffer(const Buffer&);
	const Buffer& operator=(const Buffer&);

private:
	std::vector<char> buffer_;
	size_t readerIndex_;
	size_t writerIndex_;
};

#endif // ASIO_BASE_BUFFER_H_
