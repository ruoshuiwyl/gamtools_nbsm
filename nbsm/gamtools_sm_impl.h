//
// Created by ruoshui on 4/23/18.
//



#include <vector>
#include <cstddef>
#include <string>
#include <memory>
#include <htslib/sam.h>
#include <util/create_index.h>
#include <util/channel.h>
#include <sharding/bam_sharding_impl.h>
#include <util/gam_read_buffer.h>


namespace  gamtools {
    struct SmOptions;
    class Slice;
    class GAMMarkDuplicateImpl;
//    class BAMShardingImpl;
    class SMImpl {
    public:
        explicit SMImpl(const bam_hdr_t *bam_hdr,
                        const SmOptions &options,
                        const std::string &bam_file, 
                        Channel<std::unique_ptr<BWAReadBuffer>> &input);
//        void Initialize(const SmOptions &options);
        std::thread spawn();
        void ProcessSharding();
        void OutputBAM();
    private:
        void PutSortSlice(const Slice &slice);
        void ShardingMarkdupInfo(const char *read1_dup, const char *read2_dup);
        const bam_hdr_t *bam_hdr_;
        std::string bam_file_;
        const SmOptions &options_;
        std::unique_ptr<BAMShardingImpl> sharding_impl_;
        std::unique_ptr<GAMMarkDuplicateImpl> markdup_impl_;
        Channel<std::unique_ptr<BWAReadBuffer>> &input_;
    };
}