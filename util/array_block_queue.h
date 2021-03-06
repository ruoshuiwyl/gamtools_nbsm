//
// Created by ruoshui on 4/23/18.
//

#ifndef GAMTOOLS_SM_CHANNEL_H
#define GAMTOOLS_SM_CHANNEL_H

#include <queue>
#include <mutex>
#include <thread>
#include <condition_variable>
#include <cstddef>
#include "block_queue.h"

//#pragma once
//
namespace gamtools {
    template <typename T>
    class ArrayBlockQueue : public BlockQueue<T> {
    public:
        explicit  ArrayBlockQueue(): eof_(false){}
        ArrayBlockQueue(const ArrayBlockQueue&) = delete;
        void operator=(const ArrayBlockQueue&) = delete;
        void SendEof() {
            std::lock_guard<std::mutex> lk(lock_);
            eof_ = true;
            cv_.notify_all();
        }
        bool eof() {
            std::lock_guard<std::mutex> lk(lock_);
            return buffer_.empty() && eof_;
        }

        size_t size()  {
            std::lock_guard<std::mutex> lk(lock_);
            return buffer_.size();
        }
        bool read(T& elem) {
            std::unique_lock<std::mutex> lk(lock_);
            cv_.wait(lk, [&] { return eof_ || !buffer_.empty(); });
            if (eof_ && buffer_.empty()) {
                return false;
            }
            elem = std::move(buffer_.front());
            buffer_.pop();
            cv_.notify_one();
            return true;
        }

        void write(T&& elem) {
            std::unique_lock<std::mutex> lk(lock_);
            buffer_.emplace(std::forward<T>(elem));
            cv_.notify_one();
        }
    private:
        std::condition_variable cv_;
        std::mutex lock_;
        std::queue<T> buffer_;
        bool eof_;

    };


}


#endif //GAMTOOLS_SM_CHANNEL_H
