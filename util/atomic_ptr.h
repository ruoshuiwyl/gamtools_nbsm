//
// Created by ruoshui on 4/24/18.
//

#ifndef GAMTOOLS_SM_ATOMIC_PTR_H
#define GAMTOOLS_SM_ATOMIC_PTR_H

#include <atomic>

namespace gamtools {

    class AtomicPointer {
    private:
        std::atomic<void*> rep_;
    public:
        AtomicPointer() { }
        explicit AtomicPointer(void* v) : rep_(v) { }
        inline void* Acquire_Load() const {
            return rep_.load(std::memory_order_acquire);
        }
        inline void Release_Store(void* v) {
            rep_.store(v, std::memory_order_release);
        }
        inline void* NoBarrier_Load() const {
            return rep_.load(std::memory_order_relaxed);
        }
        inline void NoBarrier_Store(void* v) {
            rep_.store(v, std::memory_order_relaxed);
        }
    };
}
#endif //GAMTOOLS_SM_ATOMIC_PTR_H
