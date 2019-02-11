#include "TaskManager.h"
#include <mutex>
#include <boost/asio/io_context.hpp>
#include "runnable.h"
#include "threadpool.h"
#include "session.h"

class TaskRunnable : public Runnable
{
public:
	TaskRunnable(const SessionPtr &sessionPtr, const PackagePtr &pacPtr, const Task &task)
		: sessionPtr_(sessionPtr), pacPtr_(pacPtr), task_(task)
	{}

	void run()
	{
		if (task_) {
			task_(sessionPtr_, pacPtr_);
		}
	}

private:
	SessionPtr sessionPtr_;
	PackagePtr pacPtr_;
	Task task_;
};

typedef std::pair<boost::asio::io_context*, boost::asio::io_context::work*> WorkContext;

class Worker {
public:
	Worker() : work(io) {
		thread = std::move(std::thread([this]() {
			io.run();
		}));
	}
	~Worker() {
		io.stop();
	}

	std::thread thread;
	boost::asio::io_context io;
	boost::asio::io_context::work work;
};
typedef std::shared_ptr<Worker> WorkerPtr;

class TaskManagerPrivate {
public:
    TaskManagerPrivate() : 
        workThreadCount_(std::max(std::thread::hardware_concurrency() * 2, 1u)),
        pool_(new ThreadPool(workThreadCount_))
    {
        for (int i = 0; i < workThreadCount_; i++) {
			workers_.push_back(std::make_shared<Worker>());
        }
    }

    int workThreadCount_;
	std::unique_ptr<ThreadPool> pool_;
	std::map<std::string, Task> handler_;
	std::vector<WorkerPtr> workers_;
};

TaskManager* TaskManager::s_inst = nullptr;
TaskManager* TaskManager::instance()
{
	static std::once_flag instanceFlag;
	std::call_once(instanceFlag, []() {
		s_inst = new TaskManager();
	});
	return s_inst;
}

TaskManager::TaskManager()
    : d_ptr(new TaskManagerPrivate())
{
}

void TaskManager::addHandleTask(const std::string &protoName, const Task &task)
{
	d_func()->handler_[protoName] = task;
}

void TaskManager::handleMessage(const PackagePtr &pacPtr, const SessionPtr &sessionPtr)
{
	std::string name = std::move(pacPtr->typeName);
    auto iter = d_func()->handler_.find(name);
    if (iter != d_func()->handler_.end()) {
        const Task &task = iter->second;
        if (pacPtr->header.isorder) {
			int workId = sessionPtr->sessionId() % d_func()->workThreadCount_;
            d_func()->workers_[workId]->io.post([=]() {
                task(sessionPtr, pacPtr);
			});
        } else {
            d_func()->pool_->run([=]() {
                task(sessionPtr, pacPtr);
            });
        }
	} else {
		std::cout << "message discard:" << name;
	}
}

void TaskManager::destory()
{
    d_func()->pool_->stop();
}
