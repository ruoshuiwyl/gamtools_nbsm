//
// Created by ruoshui on 5/29/18.
//

#ifndef GAMTOOLS_SM_THREAD_POOL_H
#define GAMTOOLS_SM_THREAD_POOL_H

#include <functional>
#include <mutex>
#include <condition_variable>

namespace gamtools{
    class ThreadPool {
    public :
        ThreadPool(int thread_num);
        void submit(std::function<void()> &&f);
        void wait();
        void close();
        struct Impl;
    private:
        std::unique_ptr<Impl> impl_;
        int finish_;
        std::mutex mtx_;
        std::condition_variable finish_cv_;


    };
}


#endif //GAMTOOLS_SM_THREAD_POOL_H
