#ifndef ASIO_BASE_RUNNABLE_H_
#define ASIO_BASE_RUNNABLE_H_

// 可运行的任务，默认支持自动销毁，通过重载run来实现要执行的任务
class Runnable {
public:
	Runnable(void) : m_ref(0) {}
	virtual ~Runnable(void) {}
	virtual void run(void) = 0;

	bool autoDelete(void) const { return this->m_ref != -1; }
	void setAutoDelete(bool v) { this->m_ref = v ? 0 : -1; }

private:
	int m_ref;

	friend class ThreadPool;
	friend class ThreadPoolPrivate;
	friend class ThreadPoolThread;
};

#endif // ASIO_BASE_RUNNABLE_H_

