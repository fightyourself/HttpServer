#pragma once
#include <thread>
#include <vector>
#include <functional>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <queue>
class ThreadPool{
private:
    std::vector<std::thread> threads_;
    std::queue<std::function<void()>> tasks_;
    std::mutex mtx_;
    std::condition_variable cv_;
    std::atomic_bool stop_;
public:
    ThreadPool(int num_threads);
    ~ThreadPool();
    void add_task(std::function<void()> task);
};