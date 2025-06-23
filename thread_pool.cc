#include <iostream>
#include <vector>
#include <queue>
#include <memory>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <future>
#include <functional>
#include <stdexcept>
#include <unistd.h>

class ThreadPool {
public:
    ThreadPool(size_t threads);
    ~ThreadPool();
    template<class T>
    void enqueue(T&& f);

private:
    bool stop;
    std::vector<std::thread> workers;
    std::queue<std::function<void()>> tasks;
    std::mutex queue_mutex;
    std::condition_variable condition;
};

ThreadPool::ThreadPool(size_t threads):stop(false) {
    for (int i = 0; i < threads; i++) {
        workers.emplace_back([this]{
            while (true) {
                std::function<void()> task;
                {
                    std::unique_lock<std::mutex> lock(queue_mutex);
                    if (this->tasks.empty()) {
                        this->condition.wait(lock);
                    }
                    if (this->stop && this->tasks.empty()) {
                        return;
                    }
                    task = std::move(this->tasks.front());
                    this->tasks.pop();
                }
                task();
            }
        });
    }
}

template<class T>
void ThreadPool::enqueue(T&& f) {
    {
        std::unique_lock<std::mutex> lock(queue_mutex);
        tasks.emplace(std::function<void()>(std::forward<T>(f)));
    }
    condition.notify_one();
}

ThreadPool::~ThreadPool() {
    std::unique_lock<std::mutex> lock(queue_mutex);
    stop = true;
    condition.notify_all();
    for (auto& work : workers) {
        work.join();
    }
}

void task() {
    std::cout << "process_task start" << std::endl;
}

int main() {
    ThreadPool pools(4);
    for (int i = 0; i < 10; i++) {
        pools.enqueue(task);
    }
    sleep(10);

    return 0;
}