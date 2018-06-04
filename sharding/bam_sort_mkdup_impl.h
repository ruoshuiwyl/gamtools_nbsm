//
// Created by ruoshui on 5/22/18.
//

#ifndef GAMTOOLS_SM_BAM_SORT_MKDUP_IMPL_H
#define GAMTOOLS_SM_BAM_SORT_MKDUP_IMPL_H

#include <thread>
#include <vector>
#include <util/channel.h>
#include <atomic>
#include "nbsm/options.h"

namespace gamtools {
    class ShardingPartitionData;
    class Block;
    class Slice;
    class BAMBlock;

    struct GAMPartitionData {
        explicit GAMPartitionData(int idx): sharding_idx(idx){}
        int sharding_idx;
        std::vector<std::unique_ptr<Block>>  blocks;
    };

    class BAMSortMkdupImpl {
    public:
        BAMSortMkdupImpl(std::vector<std::unique_ptr<ShardingPartitionData>> &partition_datas,
                         const SMOptions &sm_opt,
                         const bam_hdr_t *bam_hdr,
                         const std::string &bam_filename);
        void ProcessSortMkdup();

    private:
        void ReadGAMPartitionData();
        void OutputBAM();
        void ReadGAMBlock(std::unique_ptr<ShardingPartitionData> &part_data);
        void PartitionDecompressMerge();
        void Decompress(std::unique_ptr<GAMPartitionData> &gam_part);
        void MergePartition(std::unique_ptr<GAMPartitionData> &gam_part);
        void InsertBAMSlice(gamtools::Slice &slice, std::unique_ptr<BAMBlock> &bam_block_ptr);
        void OutputShardingBAM(int current_sharding_idx);
        std::vector<std::unique_ptr<ShardingPartitionData>> &partition_datas_;
        Channel<std::unique_ptr<GAMPartitionData>> gam_part_channel_;
        Channel<std::unique_ptr<BAMBlock>> output_bam_channel_;
        std::vector<std::vector<std::unique_ptr<BAMBlock>>> chunks_;
        const SMOptions &sm_options_;
        int block_size_;//
        htsFile *bam_file_;
        const bam_hdr_t *bam_hdr_;
        std::atomic<int> gam_block_idx;
        std::string bam_filename_;
    };
}


#endif //GAMTOOLS_SM_BAM_SORT_MKDUP_IMPL_H
