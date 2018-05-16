//
// Created by ruoshui on 5/9/18.
//

#ifndef GAMTOOLS_SM_MARK_DUPLICATE_FRAG_END_H
#define GAMTOOLS_SM_MARK_DUPLICATE_FRAG_END_H


#include <vector>
#include <lib/htslib-1.3.1/htslib/sam.h>
#include "mark_duplicate_read_ends.h"

namespace gamtools {
    class MarkDuplicateFragEnd {
    public:
        explicit MarkDuplicateFragEnd(const bam_hdr_t *bam_hdr, const std::string &filename);
        void AddFragEnd(const std::shared_ptr<MarkDuplicateFragEnds> &frag_end);
        void AddPairFlag(const int tid, const int pos);
        void ProcessMarkDuplicateFragEnd();
        ~MarkDuplicateFragEnd();
    private:
        void StoreFileFragEnds();
        void ComputeMarkDuplicateFragEnds();
        bool AreComparableForDuplicatesFragEnds(const MarkDuplicateFragEnds &lhs,
                                                                     const MarkDuplicateFragEnds &rhs) ;
        void ComputeMarkDuplicateFragments(std::vector<MarkDuplicateFragEnds*> &next_chunk, const bool contains_pairs);
        void ReadMarkDupFragEnds(const char *p, MarkDuplicateFragEnds &frag_end);
        std::vector<std::shared_ptr<MarkDuplicateFragEnds>> frag_ends_;
        std::vector<std::vector<bool>> pair_flags_;
        std::string filename_;
        bool append_;
        char *buffer_;
        char *compress_buffer_;
        const int kBufferLen = 2048;
        const int kFragEndLen = 16;
        int kFragEndsNum;
    };
}


#endif //GAMTOOLS_SM_MARK_DUPLICATE_FRAG_END_H
