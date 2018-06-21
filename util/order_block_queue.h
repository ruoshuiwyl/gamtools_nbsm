//
// Created by ruoshui on 6/19/18.
//

#ifndef GAMTOOLS_SM_ORDER_BLOCK_QUEUE_H
#define GAMTOOLS_SM_ORDER_BLOCK_QUEUE_H
#include <mutex>
#include <condition_variable>
#include <boost/circular_buffer.hpp>
#include "block_queue.h"

namespace gamtools {
    template<typename T>
    class OrderBlockQueue : public BlockQueue<T> {
    public:
        explicit OrderBlockQueue(int max_size)
                : eof_(false),
                  mtx_(),
                  not_empty_cv_(),
                  not_full_cv_(),
                  queue_(max_size),
                  order_id_(-1) {

        }

        OrderBlockQueue(const OrderBlockQueue &) = delete;

        void operator=(const OrderBlockQueue &) = delete;

        void SendEof() {
            std::lock_guard<std::mutex> lk(mtx_);
            eof_ = true;
            not_empty_cv_.notify_all();
        }

        bool eof() {
            std::lock_guard<std::mutex> lk(mtx_);
            return queue_.empty() && eof_;
        }

        size_t size() {
            std::lock_guard<std::mutex> lk(mtx_);
            return queue_.size();
        }

        bool read(T &elem) {
            std::unique_lock<std::mutex> lk(mtx_);
            not_empty_cv_.wait(lk, [&] {
                return eof_ || !queue_.empty();
            });
            if (eof_ && queue_.empty()) {
                return false;
            }
            elem = std::move(queue_.front());
            queue_.pop_front();
            not_full_cv_.notify_one();
            return true;
        }

        void write(T &&elem) {
            std::unique_lock<std::mutex> lk(mtx_);
            int order = elem->order();
            not_full_cv_.wait(lk, [&] {
                return !queue_.full() && (order == order_id_ + 1);
            });
            queue_.push_back(std::forward<T>(elem));
            order_id_++;
            not_empty_cv_.notify_one();
        }

    private:
        bool eof_;
        int order_id_;
        std::mutex mtx_;
        std::condition_variable not_empty_cv_;
        std::condition_variable not_full_cv_;
        boost::circular_buffer<T> queue_;

    };
}
#endif //GAMTOOLS_SM_ORDER_BLOCK_QUEUE_H
