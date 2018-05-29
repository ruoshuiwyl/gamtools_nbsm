//
// Created by ruoshui on 5/29/18.
//

#include <atomic>
#include <deque>
#include <thread>
#include <vector>
#include <assert.h>
#include "thread_pool.h"


namespace gamtools {
    struct ThreadPool::Impl {
        using Task = std::function<void()>;
        Impl();
        ~Impl();
        void submit(std::function<void()> && task);
        void wait();
        void JoinThreads();
        void SetBackgroundThreads(int thread_num);
    private:
        void BGThread();
        std::atomic<bool> exit_all_threads_;
        std::atomic<bool> wait_for_jobs_to_complete_;
        int total_threads_limit_;
        std::atomic<int> queue_len_;
        std::atomic<int> finish_len_;
        std::deque<Task> queue_;
        std::mutex               mtx_;
        std::condition_variable  bgsignal_;
        std::condition_variable  finish_cv_;
        std::vector<std::thread> bgthreads_;
    };

    ThreadPool::Impl::Impl() :exit_all_threads_(false),
                              wait_for_jobs_to_complete_(false),
                              queue_len_(0),
                              queue_(),
                              mtx_(),
                              bgsignal_(),
                              bgthreads_(){}
    ThreadPool::Impl::~Impl() {
        assert(bgthreads_.size() == 0U);
    }
    void ThreadPool::Impl::JoinThreads() {
        std::unique_lock<std::mutex> lock(mtx_);
        assert(!exit_all_threads_);
        wait_for_jobs_to_complete_ = true;
        exit_all_threads_ = true;
        total_threads_limit_ = 0;
        lock.unlock();
        bgsignal_.notify_all();
        for (auto& th : bgthreads_) {
            th.join();
        }
        bgthreads_.clear();
        exit_all_threads_ = false;
        wait_for_jobs_to_complete_ = false;
    }

    void ThreadPool::Impl::submit(std::function<void()> &&task) {
        std::lock_guard<std::mutex> lock(mtx_);
        if (exit_all_threads_){
            return ;
        }
        queue_.push_back(task);
        ++finish_len_;
        queue_len_.store(static_cast<int>(queue_.size()));
        bgsignal_.notify_one();
    }

    void ThreadPool::Impl::SetBackgroundThreads(int thread_num) {
    total_threads_limit_ = thread_num;
     while((int)bgthreads_.size() < total_threads_limit_) {
             bgthreads_.push_back(std::thread(&Impl::BGThread, std::move(this)));
     }
    }

    void ThreadPool::Impl::BGThread() {
        while(true) {
            std::unique_lock<std::mutex> lock(mtx_);
            while(!exit_all_threads_ && queue_.empty()) {
                bgsignal_.wait(lock);
            }
            if (exit_all_threads_) {
                if (!wait_for_jobs_to_complete_ || queue_.empty()) {
                    break;
                }
            }
            auto task = std::move(queue_.front());
            queue_.pop_front();
            queue_len_.store(static_cast<int>(queue_.size()));
            lock.unlock();
            task();
            lock.lock();
            --finish_len_;
            if (finish_len_.load() == 0) {
                finish_cv_.notify_one();
            }
            lock.unlock();
        }
    }
    void ThreadPool::Impl::wait() {
        std::unique_lock<std::mutex> lock(mtx_);
        while(!finish_len_.load()) {
            finish_cv_.wait(lock);
        }
    }

    ThreadPool::ThreadPool(int thread_num) {
        impl_ = std::unique_ptr<ThreadPool::Impl>(new Impl);
        impl_->SetBackgroundThreads(thread_num);
    }
    void ThreadPool::wait() {
        impl_->wait();
    }
    void ThreadPool::close() {
        impl_->JoinThreads();
    }
    void ThreadPool::submit(std::function<void()> &&f) {
        impl_->submit(std::move(f));
    }

}