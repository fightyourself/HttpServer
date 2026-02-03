#include "ThreadPool.h"
#include <syscall.h>
#include <unistd.h>

ThreadPool::ThreadPool(int num_threads,const std::string &poolType):threads_(),tasks_(),mtx_(),cv_(),stop_(false),poolType_(poolType){
    for(int i=0;i<num_threads;i++){
        threads_.emplace_back([this]{
            printf("%s thread created:%d\n",poolType_.c_str(),syscall(SYS_gettid));
            while(true){
                std::function<void()> task;
                {
                    std::unique_lock<std::mutex> mtx(mtx_);
                    cv_.wait(mtx,[this]{
                        return (this->stop_ == true) || (this->stop_== false && this->tasks_.size()!=0);
                    });
                    if(this->stop_==true && this->tasks_.size()==0) return;

                    task = std::move(this->tasks_.front());
                    this->tasks_.pop();
                }
                task();
            }
        });
    }
}

ThreadPool::~ThreadPool(){
    stop_ = true;
    cv_.notify_all();
    for(auto & thread:threads_){
        thread.join();
    }
}


void ThreadPool::add_task(std::function<void()>task){
    {
        std::unique_lock<std::mutex> mtx(mtx_);
        tasks_.push(task);
    }
    cv_.notify_one();
}

size_t ThreadPool::size() const{
    return threads_.size();
}