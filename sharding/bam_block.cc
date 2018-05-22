//
// Created by ruoshui on 4/27/18.
//

#include <cstring>
#include "bam_block.h"
#include "util/slice.h"
#include "util/block.h"
namespace gamtools {
    BAMBlock::BAMBlock(size_t block_size, int sharding_idx, int block_idx, bool eof)
            :Block(block_size),
             sharding_idx_(sharding_idx),
             block_idx_(block_idx),
             eof_(eof) {
    }
    BAMBlock::~BAMBlock() {

    }



}