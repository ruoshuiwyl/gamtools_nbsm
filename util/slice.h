//
// Created by ruoshui on 4/23/18.
//

#ifndef GAMTOOLS_SM_SLICE_H
#define GAMTOOLS_SM_SLICE_H
#include <cstddef>
#include <string>

namespace  gamtools {
    class Slice {
    public:
        Slice() : data_(""), size_(0){}
        Slice(const char *data, size_t n) : data_(data), size_(n){

        }
        const char *data() const {
            return data_;
        }
        const size_t size() const {
            return size_;
        }
        std::string ToString() const { return std::string(data_, size_); }

    private:
        const char *data_;
        size_t size_;
    };
}

#endif //GAMTOOLS_SM_SLICE_H
