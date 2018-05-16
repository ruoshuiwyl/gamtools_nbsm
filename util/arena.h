//
// Created by ruoshui on 4/23/18.
//

#ifndef GAMTOOLS_SM_AREAN_H
#define GAMTOOLS_SM_AREAN_H

#include <cstddef>
#include <vector>
#include <cassert>
#include "util/atomic_ptr.h"

namespace gamtools {
    class Arena {
    public:
        Arena();
        Arena(const Arena &arena) = delete;
        void operator=(const Arena &) = delete;
        ~Arena();
        char *Allocate(size_t bytes);
        char* AllocateAligned(size_t bytes);
        size_t MemoryUsage() const {
            return reinterpret_cast<uintptr_t>(memory_usage_.NoBarrier_Load());
        }
    private:
        char* AllocateFallback(size_t bytes);
        char* AllocateNewBlock(size_t block_bytes);
        AtomicPointer memory_usage_;
        std::vector<char *> blocks_;
        char* alloc_ptr_;
        size_t alloc_bytes_remaining_;
    };

    inline char* Arena::Allocate(size_t bytes) {
        // The semantics of what to return are a bit messy if we allow
        // 0-byte allocations, so we disallow them here (we don't need
        // them for our internal use).
        assert(bytes > 0);
        if (bytes <= alloc_bytes_remaining_) {
            char* result = alloc_ptr_;
            alloc_ptr_ += bytes;
            alloc_bytes_remaining_ -= bytes;
            return result;
        }
        return AllocateFallback(bytes);
    }




}


#endif //GAMTOOLS_SM_AREAN_H
