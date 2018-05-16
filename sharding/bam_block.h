//
// Created by ruoshui on 4/27/18.
//

#ifndef GAMTOOLS_SM_BAM_BLOCK_H
#define GAMTOOLS_SM_BAM_BLOCK_H

#include <cstddef>
#include "util/block.h"

namespace gamtools {
    class Slice;
    class BAMBlock : public Block {
    public:
        explicit BAMBlock(size_t block_size, int sharding_idx, int block_idx, bool eof);
        ~BAMBlock();
        bool Insert(Slice &slice);
        const bool eof() const {
            return eof_;
        }
        const int block_idx() const {
            return block_idx_;
        }
        const int sharding_idx() const {
            return sharding_idx_;
        }
    private:
        bool eof_;
        int block_idx_;
        int sharding_idx_;
    };
}


#endif //GAMTOOLS_SM_BAM_BLOCK_H
