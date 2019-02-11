#include <stdlib.h>
#include <functional>
#include <thread>
#include <string>
#include <mutex>
#include <condition_variable>
#include <vector>
#include <memory>
#include <assert.h>
#include <algorithm>
#include <queue>
#include <process.h>

class ThreadPool
{
public:
    typedef std::function<void()> Task;

    ThreadPool(int num)
        : num_(num)
        , maxQueueSize_(0)
        , running_(false)
    {
    }

    ~ThreadPool()
    {
        if (running_) {
            stop();
        }
    }

    ThreadPool(const ThreadPool&) = delete;
    void operator=(const ThreadPool&) = delete;

    void setMaxQueueSize(int maxSize)
    {
        maxQueueSize_ = maxSize;
    }

    void start()
    {
        assert(threads_.empty());
        running_ = true;
        threads_.reserve(num_);
        for (int i = 0; i < num_; i++) {
            threads_.push_back(std::thread(std::bind(&ThreadPool::threadFunc, this)));
        }
    }

    void stop()
    {
        {
            std::unique_lock<std::mutex> ul(mutex_);
            running_ = false;
            notEmpty_.notify_all();
        }

        for (auto &iter : threads_) {
            if (iter.joinable()) {
                iter.join();
            }
        }
    }

    void run(const Task &t)
    {
        if (threads_.empty()) {
            t();
        } else {
            std::unique_lock<std::mutex> ul(mutex_);
            while (isFull()) {
                notFull_.wait(ul);
            }
            assert(!isFull());
            queue_.push_back(t);
            notEmpty_.notify_one();
        }
    }

private:
    bool isFull() const
    {
        return maxQueueSize_ > 0 && queue_.size() >= maxQueueSize_;
    }

    void threadFunc()
    {
        printf("create thread id:0x%x\n", std::this_thread::get_id());
        while (running_) {
            Task task(take());
            if (task) {
                task();
            }
        }
        printf("end thread id:0x%x\n", std::this_thread::get_id());
    }

    Task take()
    {
        std::unique_lock<std::mutex> ul(mutex_);
        while (queue_.empty() && running_) {
            notEmpty_.wait(ul);
        }
        Task task;
        if (!queue_.empty()) {
            task = queue_.front();
            queue_.pop_front();
            if (maxQueueSize_ > 0) {
                notFull_.notify_one();
            }
        }
        return task;
    }

private:
    int num_;
    std::mutex mutex_;
    std::condition_variable notEmpty_;
    std::condition_variable notFull_;
    std::vector<std::thread> threads_;
    std::deque<Task> queue_;
    size_t maxQueueSize_;
    bool running_;
};