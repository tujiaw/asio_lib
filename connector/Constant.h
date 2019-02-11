#pragma once

#include <functional>
#include "google/protobuf/message.h"

class session;

typedef std::shared_ptr<google::protobuf::Message> MessagePtr;
struct Package {
	Package() 
	: totalSize(0)
	, id(0)
	, typeNameLen(0)
	, msgPtr(nullptr)
	{
		flag[0] = 'P';
		flag[1] = 'P';
	}

	char flag[2];			// 消息标志'P','P'
	int totalSize;			// 整个package大小

	int id;					// 消息ID
	int typeNameLen;		// 类型名长度
	std::string typeName;	// 类型名
	MessagePtr msgPtr;		// protobuf消息
};

typedef std::shared_ptr<session> SessionPtr;
typedef std::shared_ptr<google::protobuf::Message> MessagePtr;
typedef std::shared_ptr<Package> PackagePtr;

typedef std::function<void(const SessionPtr&, const PackagePtr&)> Task;
typedef std::function<void(int error, const PackagePtr &reqMsgPtr, const PackagePtr &rspMsgPtr)> Response;
