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
//    class Block;
    class BAMBlock;
    class PartitionData;
    class ShardingPartitionData {
    public:
        explicit ShardingPartitionData(Channel<std::unique_ptr<GAMBlock>> &sort_channel,
                                  Channel <std::unique_ptr<BAMBlock>> &output_bam_channel,
                                  const PartitionData &partdata,
                                  const int block_size);
        ~ShardingPartitionData();
        void Add(const Slice &slice);
        const PartitionData &partition_data() {
            return partition_data_;
        }
        void sendEof();
        const int block_cnt() {
            return block_cnt_;
        }
    private:
//        void MergeSort();
//        void ReadGAMBlock();
//        bool InsertBAMSlice(Slice &bam_slice);
//        void MergeSendEof();
        bool append_;
        int block_cnt_;
        int block_size_;
        int bam_block_idx_;
        const PartitionData &partition_data_;
        Channel<std::unique_ptr<GAMBlock>> &sort_channel_;
        Channel<std::unique_ptr<BAMBlock>> &output_bam_channel_;
        std::unique_ptr<GAMBlock> gam_block_ptr_;
        std::unique_ptr<BAMBlock> bam_block_ptr_;
        std::vector<std::unique_ptr<GAMBlock>> gam_blocks_;
    };
}


#endif //GAMTOOLS_SM_BAM_PARTITION_H
