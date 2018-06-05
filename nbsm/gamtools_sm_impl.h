//
// Created by ruoshui on 4/23/18.
//



#include <vector>
#include <cstddef>
#include <string>
#include <memory>
#include <htslib/sam.h>
#include <util/create_index.h>
#include <util/array_block_queue.h>
#include <sharding/bam_sharding_impl.h>
#include <util/gam_read_buffer.h>


namespace  gamtools {
    struct SMOptions;
    class Slice;
    class GAMMarkDuplicateImpl;
//    class BAMShardingImpl;
    class SMImpl {
    public:
        explicit SMImpl(const bam_hdr_t *bam_hdr,
                        const SMOptions &options,
                        const std::string &bam_file, 
                        ArrayBlockQueue<std::unique_ptr<BWAReadBuffer>> &input);
//        void Initialize(const SMOptions &options);
        std::thread SpawnSharding();
        void Sharding();
//        std::thread SpawnSortMkdup();
//        void OutputBAM();
        void ProcessMarkDuplicate();
    private:
        void ProcessSharding();
//        void PutSortSlice(const Slice &slice);
//        void ShardingMarkdupInfo(const read_end_t *read1_dup, const read_end_t *read2_dup);
        const bam_hdr_t *bam_hdr_;
        std::string bam_file_;
        const SMOptions &options_;
        std::unique_ptr<BAMShardingImpl> sharding_impl_;
        std::unique_ptr<GAMMarkDuplicateImpl> markdup_impl_;
        ArrayBlockQueue<std::unique_ptr<BWAReadBuffer>> &input_;
        std::atomic<bool> finish_markdup_flag_;
    };
}