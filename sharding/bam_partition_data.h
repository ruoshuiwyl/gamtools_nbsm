//
// Created by ruoshui on 4/24/18.
//

#ifndef GAMTOOLS_SM_BAM_PARTITION_H
#define GAMTOOLS_SM_BAM_PARTITION_H

#include <string>
#include <util/channel.h>
#include <sharding/gam_block.h>

namespace gamtools {
    class Slice;
    class Block;
    class PartitionData;
    class BAMPartitionData {
    public:
        explicit BAMPartitionData(Channel <std::unique_ptr<GAMBlock>> &sort_channel, const PartitionData &partdata,
                                          const int block_size);
        ~BAMPartitionData();
        void Add(const Slice &slice);
        void sendEof();
        void MergeSort();
        void ReadGAMBlock();
    private:
        bool InsertBAMSlice(Slice &bam_slice);
        bool append_;
        int block_cnt_;
        int block_size_;
        const PartitionData &paration_data_;
        Channel<std::unique_ptr<GAMBlock>> &sort_channel_;
        std::unique_ptr<GAMBlock> gam_block_ptr_;
        std::unique_ptr<Block> bam_block_ptr_;
        std::vector<std::unique_ptr<GAMBlock>> gam_blocks_;
    };
}


#endif //GAMTOOLS_SM_BAM_PARTITION_H
