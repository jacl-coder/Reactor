#pragma once
#include <sys/syscall.h>
#include <unistd.h>
#include <queue>
#include <mutex>
#include <unistd.h>
#include <thread>
#include <condition_variable>
#include <functional>
#include <future>
#include <atomic>

class ThreadPool
{
private:
    std::vector<std::thread> threads_;
    std::queue<std::function<void()>> taskqueue_;
    std::mutex mutex_;
    std::condition_variable condition_;
    std::atomic_bool stop_;

public:
    ThreadPool(size_t threadnum);

    void addtask(std::function<void()> task);

    ~ThreadPool();
};
