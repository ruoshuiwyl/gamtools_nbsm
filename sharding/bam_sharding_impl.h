//
// Created by ruoshui on 4/24/18.
//

#ifndef GAMTOOLS_SM_BAM_SHARDING_H
#define GAMTOOLS_SM_BAM_SHARDING_H

#include <vector>
#include <cstddef>
#include <thread>


#include <htslib/sam.h>
#include <htslib/bgzf.h>
#include <htslib/hts.h>

#include <nbsm/options.h>
#include <sharding/bam_partition_data.h>
#include <util/create_index.h>
#include "bam_block.h"
#include "gam_block.h"
#include "util/channel.h"

namespace gamtools {
    class Slice;

//    class BAMShardingSort;


    class BAMShardingImpl {
    public:
        explicit BAMShardingImpl(const bam_hdr_t *bam_hdr, const SMOptions &options);
        void SendEof();
        void InitializeSharding();
        void StartSharding();
//        void StartMergeSort(std::string &bam_filename);
        void Sharding(const Slice &slice);
        void FinishSharding();
        std::vector<std::unique_ptr<ShardingPartitionData>>& partition_datas() {
            return  partition_datas_;
        }
//        void ReadGamBlock();
//        void FinishMergeSort();
    private:
        void OutputGAMBlock();
        void GAMBlockSortCompress();
//        void MergeSort();
//        void OutputBAM();

//        typedef Channel<std::unique_ptr<GAMBlock>> GAMBlockChannel;
        Channel<std::unique_ptr<GAMBlock>> sort_channel_;
        Channel<std::unique_ptr<GAMBlock>> output_channel_;
//        Channel<std::unique_ptr<ShardingPartitionData>> merge_channel_;
//        Channel<std::unique_ptr<BAMBlock>> bam_channel_;
        std::unique_ptr<CreateIndex> sort_idx_;
        std::thread output_gamblock_thread_;
        std::vector<std::thread> sort_compress_threads_;
        std::vector<std::unique_ptr<ShardingPartitionData>> partition_datas_;
//        std::vector<std::vector<std::unique_ptr<BAMBlock>>> chunks_;
        const bam_hdr_t *bam_hdr_;
        const SMOptions &options_;
//        htsFile *bam_file_;
    };
}


#endif //GAMTOOLS_SM_BAM_SHARDING_H
