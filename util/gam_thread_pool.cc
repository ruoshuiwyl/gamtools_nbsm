//
// Created by ruoshui on 4/26/17.
//

#include <future>
#include <iostream>
#include "gam_thread_pool.h"

namespace gamtools {
    GAMThreadPool::GAMThreadPool(size_t size) {
        active_.store(true);
        size = size < 1 ? 1 : size;
        for (int i = 0; i < size; ++i) {
            workers_.emplace_back(&GAMThreadPool::Schedual, this);
        }
    }

    GAMThreadPool::~GAMThreadPool() {
        std::cerr << "~" << std::endl;
        std::unique_lock<std::mutex> lock(queue_mutex_);
        active_.store(false);
        condition_.notify_all();
        lock.unlock();
//    condition_.notify_all();
//    stop_.store(true);
        for (auto &worker : workers_) {
            worker.join();
//        worker.detach();
        }
        std::cout << tasks_.empty() << std::endl;
    }

    void GAMThreadPool::Schedual() {
        auto id = std::this_thread::get_id();
//    std::cerr << id << "\t" << __LINE__ << "\t" << active_.load() << std::endl;
        while (true) {
            std::unique_lock<std::mutex> lock(queue_mutex_);
//        std::cerr << id << "\t" << __LINE__ << std::endl;
            condition_.wait(lock, [this]() { return (!active_ || !tasks_.empty()); });
            if (!active_.load() && tasks_.empty()) {
                break;
            }
//        std::cerr << "tasks size " << tasks_.size() << std::endl;
            Task task{std::move(tasks_.front())};
            tasks_.pop();
            lock.unlock();
            task();
        }
    }
}
