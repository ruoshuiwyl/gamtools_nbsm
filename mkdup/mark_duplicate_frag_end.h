//
// Created by ruoshui on 5/9/18.
//

#ifndef GAMTOOLS_SM_MARK_DUPLICATE_FRAG_END_H
#define GAMTOOLS_SM_MARK_DUPLICATE_FRAG_END_H


#include <vector>
#include <lib/htslib-1.3.1/htslib/sam.h>
#include <bwa_mem/gam_read.h>
#include <thread>
#include <util/create_index.h>
#include "mark_duplicate_read_ends.h"

namespace gamtools {
    class MarkDuplicateFragEnd {
    public:
        explicit MarkDuplicateFragEnd(const bam_hdr_t *bam_hdr, const PartitionData &partition_data);
        void AddFragEnd(const read_end_t *frag_end);
        void AddPairFlag(const int tid, const int pos);
        std::thread spawn();
        ~MarkDuplicateFragEnd();
    private:
        void ProcessMarkDuplicateFragEnd();
        void StoreFileFragEnds();
        void ComputeMarkDuplicateFragEnds();
        bool AreComparableForDuplicatesFragEnds(const read_end_t *lhs,
                                                const read_end_t *rhs) ;
        void ComputeMarkDuplicateFragments(std::vector<const read_end_t *> &next_chunk, const bool contains_pairs);
        read_end_t * ReadMarkDupFragEnds(const char *p);
        std::vector<const read_end_t *> frag_ends_;
        std::vector<std::vector<bool>> pair_flags_;
        std::vector<std::vector<bool>> negative_pair_flags_;
//        std::string filename_;
        const PartitionData &partition_data_;
        bool append_;
        char *buffer_;
        char *compress_buffer_;
        const int kBufferLen = 2048;
        const int kFragEndLen = 16;
        int kFragEndsNum;
    };
}


#endif //GAMTOOLS_SM_MARK_DUPLICATE_FRAG_END_H
