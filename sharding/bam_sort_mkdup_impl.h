//
// Created by ruoshui on 5/22/18.
//

#ifndef GAMTOOLS_SM_BAM_SORT_MKDUP_IMPL_H
#define GAMTOOLS_SM_BAM_SORT_MKDUP_IMPL_H

#include <thread>
#include <vector>
#include <util/array_block_queue.h>
#include <atomic>
#include <util/bounded_queue.h>
#include <util/order_block_queue.h>
#include <qc_report/qc_stat_impl.h>
#include "nbsm/options.h"

namespace gamtools {
    class ShardingPartitionData;
    class Block;
    class Slice;
    class BAMBlock;

    struct GAMPartitionData {
        explicit GAMPartitionData(int chr_idx, int idx): tid(chr_idx), sharding_idx(idx), bam_block_ptr(nullptr){}
        int tid;
        int sharding_idx;
        std::unique_ptr<Block> bam_block_ptr;
        std::vector<std::unique_ptr<Block>>  blocks;
        inline int order(){
            return sharding_idx;
        }
        void InsertBAMSlice(gamtools::Slice &slice);
        void End();
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
        std::unique_ptr<GAMPartitionData> ReadGAMBlock(std::unique_ptr<ShardingPartitionData> &part_data);
//        void PartitionDecompressMerge();
//        void Decompress(std::unique_ptr<GAMPartitionData> &gam_part);
        void MergePartition(std::unique_ptr<GAMPartitionData> &gam_part);
//        void InsertBAMSlice(gamtools::Slice &slice, std::unique_ptr<BAMBlock> &bam_block_ptr);
//        void OutputShardingBAM(int current_sharding_idx);
        void OutputShardingBAM(std::unique_ptr<GAMPartitionData> &bam_chunk);
        std::vector<std::unique_ptr<ShardingPartitionData>> &partition_datas_;
//        ArrayBlockQueue<std::unique_ptr<GAMPartitionData>> gam_part_channel_;
//        OrderBlockQueue<std::unique_ptr<GAMPartitionData>> gam_part_queue_;
//        ArrayBlockQueue<std::unique_ptr<BAMBlock>> output_bam_channel_;
        OrderBlockQueue<std::unique_ptr<GAMPartitionData>> output_bam_queue_;
//        OrderBlockQueue<std::unique_ptr<QCShardingData>> quality_control_queue_;
        BoundedQueue<std::unique_ptr<QCShardingData>> quality_control_queue_;
//        std::vector<std::unique_ptr<GAMPartitionData>> bam_chunks_;
//        std::vector<std::vector<std::unique_ptr<BAMBlock>>> chunks_;
        const SMOptions &sm_options_;
        int block_size_;//
        htsFile *bam_file_;
        const bam_hdr_t *bam_hdr_;
        std::atomic<int> gam_block_idx_;
        std::string bam_filename_;
        std::unique_ptr<QCStatImpl> qc_stat_impl_;

    };
}


#endif //GAMTOOLS_SM_BAM_SORT_MKDUP_IMPL_H
