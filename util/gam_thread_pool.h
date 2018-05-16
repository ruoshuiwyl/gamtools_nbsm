//
// Created by ruoshui on 4/26/17.
//

#ifndef GAMTOOLS_GAM_THREAD_POOL_H
#define GAMTOOLS_GAM_THREAD_POOL_H


#include <functional>
#include <thread>
#include <vector>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <future>

namespace gamtools {
    class GAMThreadPool {
    public :
        using Task = std::function<void()>;

        explicit GAMThreadPool(size_t size);

        ~GAMThreadPool();

        template<class F, class... Args>
        auto CommitTask(F &&f, Args &&... args) -> std::future<typename std::result_of<F(Args...)>::type>;
        void ShutDown();
//    void Restart();
    private:
        void Schedual();

        std::vector<std::thread> workers_;
        std::queue<Task> tasks_;
        std::mutex queue_mutex_;
        std::condition_variable condition_;
//    std::condition_variable queue_empty_;
        std::atomic<bool> active_;
    };

    template<class F, class... Args>
    auto GAMThreadPool::CommitTask(F &&f, Args &&... args)
    -> std::future<typename std::result_of<F(Args...)>::type> {
        if (!active_.load()) {
            throw std::runtime_error("task exectutor have closed commit.");
        }
        using ResType = decltype(f(args...));
        auto task = std::make_shared<std::packaged_task<ResType()>>(
                std::bind(std::forward<F>(f), std::forward<Args>(args)...));
        {
            std::lock_guard<std::mutex> lock{queue_mutex_};
            tasks_.emplace([task]() {
                (*task)();
            });
            condition_.notify_one();
        }
        std::future<ResType> future = task->get_future();
        return future;
    }
}



#endif //GAMTOOLS_GAM_THREAD_POOL_H
