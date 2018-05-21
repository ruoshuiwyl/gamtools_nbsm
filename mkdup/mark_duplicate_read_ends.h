//
// Created by ruoshui on 3/14/17.
//

#ifndef GAMTOOLS_MARK_DUPLICATE_READ_ENDS_H
#define GAMTOOLS_MARK_DUPLICATE_READ_ENDS_H

#include <cstdint>
#include <memory>
#include <bwa_mem/gam_read.h>



//struct MarkDuplicateInputIndex {
//
//    uint64_t read_sort_idx_ = -1;
//    uint64_t read_input_idx_ = -1;
//    static int Compare(const MarkDuplicateInputIndex &lhs,
//                       const MarkDuplicateInputIndex &rhs);
//};

bool FragReadEndsComparator(const read_end_t *lhs, const read_end_t *rhs);

class MarkDuplicateFragEnds {
public:
    MarkDuplicateFragEnds(){};
    MarkDuplicateFragEnds(int8_t read1_tid, int8_t read2_tid, int8_t orientation, int8_t lib_id, int16_t score,
                              int32_t read1_pos, int64_t name_id);
    ~MarkDuplicateFragEnds(){};
    static bool ReadEndsComparator(const std::shared_ptr<MarkDuplicateFragEnds> &lhs,
                                   const std::shared_ptr<MarkDuplicateFragEnds> &rhs);
//    bool isPaired();
    int8_t read1_tid_ = -1;
    int8_t lib_id_ = 0;
    uint8_t orientation_ = 0;
    int16_t score_ = 0;
    int32_t read1_pos_ = -1;
    int64_t name_id_ = 0;
};


class MarkDuplicatePairEnds {
public:
    MarkDuplicatePairEnds(){};
    MarkDuplicatePairEnds(int8_t read1_tid, int8_t read2_tid, int8_t orientation, int8_t lib_id, int16_t score,
                              int32_t read1_pos, int32_t read2_pos, int64_t name_id);
    ~MarkDuplicatePairEnds(){};
    static bool ReadEndsComparator(const std::shared_ptr<MarkDuplicatePairEnds> &lhs,
                                   const std::shared_ptr<MarkDuplicatePairEnds> &rhs);
    int8_t read1_tid_ = -1;
    int8_t read2_tid_ = -1;
    int8_t orientation_  = 0;
    int8_t lib_id_ = 0;
    int32_t score_ = 0;
    int32_t read1_pos_ = -1;
    int32_t read2_pos_ = -1;
    int64_t name_id_ = 0;
//    MarkDuplicateInputIndex read1_idx_;
//    MarkDuplicateInputIndex read2_idx_;
};

//class MarkDuplicateReadEnds {
//public:
//    MarkDuplicateReadEnds();
//    MarkDuplicateReadEnds(int64_t name_id, int8_t lib_id, int8_t read1_tid, int32_t read1_pos,
//                          int8_t read2_tid, int8_t orientation, int16_t score,
//                          const MarkDuplicateInputIndex &read1_idx);
//    MarkDuplicateReadEnds(int64_t name_id, int8_t lib_id, int8_t read1_tid, int32_t read1_pos,
//                          int8_t read2_tid, int32_t read2_pos, int8_t orientation, int16_t score,
//                          const MarkDuplicateInputIndex &read1_idx, const MarkDuplicateInputIndex &read2_idx);
//
//
//    static bool ReadEndsComparator(const std::shared_ptr<MarkDuplicateReadEnds> &lhs,
//                                   const std::shared_ptr<MarkDuplicateReadEnds> &rhs);
//    friend  bool operator==(const MarkDuplicateReadEnds &lhs, const MarkDuplicateReadEnds &rhs);
//
//    bool isPaired();
////    static const int8_t  ComputeOrientationByte(const bool read1_negative_strand, const bool read2_nagative_strand);
//    int8_t read1_tid_ = -1;
//    int8_t read2_tid_ = -1;
//    int8_t orientation_ ;
//    int8_t lib_id_;
//    int32_t score_ = 0;
//    int32_t read1_pos_ = -1;
//    int32_t read2_pos_ = -1;
//    int64_t name_id_ = 0;
//    MarkDuplicateInputIndex read1_idx_;
//    MarkDuplicateInputIndex read2_idx_;
////    int64_t read1_idx_ = -1;
////    int64_t read2_idx_ = -1;
//};


#endif //GAMTOOLS_MARK_DUPLICATE_READ_ENDS_H
