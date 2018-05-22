//
// Created by ruoshui on 3/13/17.
//

#ifndef GAMTOOLS_GAM_MARK_DUP_READ_END_H
#define GAMTOOLS_GAM_MARK_DUP_READ_END_H


#include <cstdint>
#include <vector>
#include <memory>
#include <htslib/sam.h>
#include <nbsm/options.h>
#include <bwa_mem/gam_read.h>
#include "mark_duplicate_region.h"


namespace gamtools {
    struct SMOptions;
    class CreateIndex;
    class MarkDuplicateFragEnd;
    class GAMMarkDuplicateImpl {
    public:
//    GAMMarkDuplicateImpl(const int chr_count, const std::string &temp_output_path);
        GAMMarkDuplicateImpl(const bam_hdr_t *bam_hdr, const SMOptions &options);
        void InitializeSharding();
        ~GAMMarkDuplicateImpl();

        void StorePairEndRecord(const read_end_t *read1_dup, const read_end_t *read2_dup);

        static const bool IsMarkDuplicate(const uint64_t read_name_id);

        void MarkDuplication(const uint64_t total_read_name_id);

        static void SetMarkDuplicateReadNameID(const uint64_t read_name_id);

        void DebugOutput(bool output);

        void DebugInput(const std::string &input_markdup_file);

    private:

        std::shared_ptr<MarkDuplicateFragEnds> ComputeFragEnds(const read_end_t *gbam);

        std::shared_ptr<MarkDuplicatePairEnds>
        ComputePairEnds(const read_end_t *read1_frag_end,
                        const read_end_t *read2_frag_end);

        const int8_t ComputeOrientationByte(const bool read1_negative_strand, const bool read2_nagative_strand);

//        static void ProcessMarkDuplication(std::unique_ptr<MarkDuplicateRegion> &mark_duplicate,
//                                           const std::string &temp_mark_dup_path);
        inline int SeekMarkDupIndex(int tid, int pos );

//    int StoreFileFragEnds();
//    int StoreFilePairEnds();
//        std::vector<std::unique_ptr<MarkDuplicateRegion>> markdup_regions_;
        std::string temp_mark_dup_path_;
        static std::vector<bool> mark_dup_readname_id_;
        static uint64_t total_read_name_ids_;
        static const int8_t F = 0, R = 1, FF = 2, FR = 3, RR = 4, RF = 5;
//        std::vector<uint64_t> markdup_index_;
        int thread_num_;
        const bam_hdr_t *bam_hdr_;
        const SMOptions &options_;
        std::vector<std::unique_ptr<MarkDuplicateRegion>> markdup_regions_;
        std::vector<std::vector<int>> markdup_sharding_index_;
        std::unique_ptr<MarkDuplicateFragEnd> markdup_frag_end_;
        std::unique_ptr<CreateIndex> markdup_idx_;

    };

    inline int GAMMarkDuplicateImpl::SeekMarkDupIndex(int tid, int pos) {
        int pos_id = pos < 0? 0 : pos >> options_.markdup_region_size;
        return markdup_sharding_index_[tid][pos_id];
    }
}

#endif //GAMTOOLS_GAM_MARK_DUP_READ_END_H
