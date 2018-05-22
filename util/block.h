//
// Created by ruoshui on 4/26/18.
//

#ifndef GAMTOOLS_SM_BLOCK_H
#define GAMTOOLS_SM_BLOCK_H
#include <cstddef>
#include <vector>
namespace gamtools {

    class Slice;
    class Block {
    public:
        explicit Block(const size_t block_size);
        bool empty();
        bool full();
        virtual bool Insert(const char *data, int len);
        virtual bool Insert(const Slice &slice);
        virtual ~Block();
        std::vector<Slice> &slices() {
            return slices_;
        }
        const std::vector<Slice> &slices() const {
            return slices_;
        }
        const size_t size() const  {
            return block_len_;
        }
        const char *data() const {
            return block_data_;
        }

    protected:
        bool full_;
        char *block_data_;
        char *alloc_ptr_;
        size_t alloc_bytes_remaining_;
        size_t block_len_;
        const size_t block_size_;
        std::vector<Slice> slices_;
    };
}


#endif //GAMTOOLS_SM_BLOCK_H
