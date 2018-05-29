//
// Created by ruoshui on 3/14/17.
//

#ifndef GAMTOOLS_SINGLE_CHROMOSOME_MARK_DUPLICATE_H
#define GAMTOOLS_SINGLE_CHROMOSOME_MARK_DUPLICATE_H

#include <string>
#include <vector>
#include <bits/unique_ptr.h>
#include <memory>
#include <thread>
#include "mark_duplicate_read_ends.h"

namespace gamtools {
    class PartitionData;
    class MarkDuplicateRegion {
    public:
//        MarkDuplicateRegion(const int region_id);
        explicit MarkDuplicateRegion(const PartitionData &partition_data);

        ~MarkDuplicateRegion();

//    void AddFragEnd(const std::shared_ptr<MarkDuplicateReadEnds> &frag_end);
//        void AddFragEnd(const std::shared_ptr<MarkDuplicateFragEnds> &frag_end);

        void AddPairEnd(const std::shared_ptr<MarkDuplicatePairEnds> &pair_end);

        std::thread spawn();
        void ProcessMarkDuplicate();

//    int FragReadEndSize(){return frag_ends_.size();}
//    int PairReadEndSize(){return pair_ends_.size();}
//    static std::ostream & WriteMarkDupReadEnd(std::ostream &out, const std::shared_ptr<MarkDuplicateReadEnds> &read_end);
//    static std::istream & ReadMarkDupReadEnd(std::istream &in, std::shared_ptr<MarkDuplicateReadEnds> &read_end);
    private:

        void ReadMarkDupPairEnds(const char *data, std::shared_ptr<MarkDuplicatePairEnds> &pair_end);

//        void ReadMarkDupFragEnds(const char *data, std::shared_ptr<MarkDuplicateFragEnds> &frag_end);

//        void StoreFileFragEnds();

        void StoreFilePairEnds();

//        std::string FileName();

        void ComputeMarkDuplicatePairEnds();

//        void ComputeMarkDuplicateFragEnds();

//    bool AreComparableForDuplicates(const MarkDuplicateReadEnds &lhs,
//                                    const MarkDuplicateReadEnds &rhs, const bool compare_read2);
//        bool AreComparableForDuplicatesFragEnds(const MarkDuplicateFragEnds &lhs, const MarkDuplicateFragEnds &rhs);

        bool AreComparableForDuplicatesPairEnds(const MarkDuplicatePairEnds &lhs, const MarkDuplicatePairEnds &rhs);

        void ComputeMarkDuplicatePairEnds(std::vector<std::shared_ptr<MarkDuplicatePairEnds>> &next_chunk);

//        void ComputeMarkDuplicateFragments(std::vector<std::shared_ptr<MarkDuplicateFragEnds>> &next_chunk,
//                                           const bool contains_pairs);

//        int region_id_;
        bool pair_file_append_flag = false;
//        bool frag_file_append_flag = false;
        char *buffer_;
        char *compress_buffer_;
        const int kBufferLen = 4096;
//        const int kPairEndsNum = 73;  // 73 * 56 + 8 = 4096
//        const int kFragEndsNum = 113; // 113 * 36 + 28 = 4096
        const int kPairEndsLen = 24;
        int kPairEndsNum;
//        const int kFragEndsLen = 36;
//    const int kPairBufferOffset = 8;
//    const int kFragBufferOffset = 28;
//        std::vector<std::shared_ptr<MarkDuplicateFragEnds>> frag_ends_;
        std::vector<std::shared_ptr<MarkDuplicatePairEnds>> pair_ends_;

        const PartitionData &partition_data_;
    };
}


#endif //GAMTOOLS_SINGLE_CHROMOSOME_MARK_DUPLICATE_H
