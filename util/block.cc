//
// Created by ruoshui on 4/26/18.
//

#include <cassert>
#include <cstring>
#include <iostream>
#include "block.h"
#include "util/slice.h"

namespace gamtools {

    Block::Block(const size_t block_size)
            :full_(false), block_size_(block_size),  alloc_ptr_(nullptr), alloc_bytes_remaining_(block_size), block_len_(0) {
        assert(block_size > 0);
        block_data_ = new char[block_size];
    }
    Block::~Block() {
        delete [] block_data_;
    }
    bool Block::empty() {
        return slices_.empty();
    }
    bool Block::full() {
        return full_;
    }
    bool Block::Insert(const Slice &slice) {
        size_t len = slice.size();
        if (len <= alloc_bytes_remaining_) {
            if (alloc_ptr_ == nullptr) {
                alloc_ptr_ = block_data_;
            }
            memcpy(alloc_ptr_, slice.data(), slice.size());
            Slice s(alloc_ptr_, slice.size());
            slices_.push_back(s);
            alloc_ptr_ += slice.size();
            alloc_bytes_remaining_ -= slice.size();
            block_len_ += slice.size();

        } else {
            alloc_bytes_remaining_ = 0;
            full_ = true;
            return false;
        }
        return true;
    }
    bool Block::Insert(const char *data, int len) {
        Slice slice(data, len);
        return Insert(slice);
    }
}