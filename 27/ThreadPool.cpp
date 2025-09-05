#include "ThreadPool.h"

ThreadPool::ThreadPool(size_t threadnum, const std::string &threadtype) : stop_(false), threadtype_(threadtype)
{
    for (int i = 0; i < threadnum; i++)
    {
        threads_.emplace_back([this]
                              {
            printf("create thread %d, type is %s.\n",syscall(SYS_gettid),threadtype_.c_str());
            while(stop_ == false)
            {
                std::function<void()> task;

                {
                    std::unique_lock<std::mutex> lock(this->mutex_);

                    this->condition_.wait(lock,[this]{
                        return ((this->stop_==true)||(this->taskqueue_.empty()==false));
                    });

                    if((this->stop_==true)&&(this->taskqueue_.empty()==true))
                    {
                        return;
                    }

                    task = std::move(this->taskqueue_.front());
                    this->taskqueue_.pop();
                }
                task();
            } });
    }
}

void ThreadPool::addtask(std::function<void()> task)
{
    {
        std::lock_guard<std::mutex> lock(mutex_);
        taskqueue_.push(task);
    }
    condition_.notify_one();
}

ThreadPool::~ThreadPool()
{
    stop_ = true;
    condition_.notify_all();
    for (std::thread &th : threads_)
    {
        th.join();
    }
}

/*
void show(int no, const std::string &name)
{
    printf("thread %d is %d, name is %s.\n", no, syscall(SYS_gettid), name.c_str());
}

void test()
{
    printf("who am i %d\n", syscall(SYS_gettid));
}

int main()
{
    ThreadPool threadpool(4);
    std::string name = "hello";
    threadpool.addtask(std::bind(show, 8, name));
    sleep(1);

    threadpool.addtask(test);
    sleep(1);

    threadpool.addtask(std::bind([]{
        printf("lambda who am i %d\n",syscall(SYS_gettid));
    }));
    sleep(1);
}
*/