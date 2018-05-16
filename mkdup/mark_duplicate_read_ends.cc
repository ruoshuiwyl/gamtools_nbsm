//
// Created by ruoshui on 3/14/17.
//

#include <cassert>
#include "mark_duplicate_read_ends.h"




MarkDuplicateFragEnds::MarkDuplicateFragEnds(int8_t read1_tid, int8_t read2_tid, int8_t orientation, int8_t lib_id, int16_t score,
                                             int32_t read1_pos, int64_t name_id)
    :read1_tid_(read1_tid), orientation_(orientation), lib_id_(lib_id),
     score_(score),  read1_pos_(read1_pos), name_id_(name_id) {

}

//bool MarkDuplicateFragEnds::isPaired() {
//    return read2_tid_ != -1;
//}

bool MarkDuplicateFragEnds::ReadEndsComparator(const std::shared_ptr<MarkDuplicateFragEnds> &lhs,
                                               const std::shared_ptr<MarkDuplicateFragEnds> &rhs) {
    const int kEqualFlag = 0;
    int compare_difference = rhs->lib_id_ - lhs->lib_id_;
    if (compare_difference == kEqualFlag) {
        compare_difference = rhs->read1_tid_ - lhs->read1_tid_;
    }
    if (compare_difference == kEqualFlag) {
        compare_difference = rhs->read1_pos_ - lhs->read1_pos_;
    }
    if (compare_difference == kEqualFlag) {
        compare_difference = rhs->orientation_ - lhs->orientation_;
    }
//    if (compare_difference == kEqualFlag) {
//        compare_difference = rhs->read2_tid_ - lhs->read2_tid_;
//    }
    if (compare_difference == kEqualFlag) {
//        compare_difference = MarkDuplicateInputIndex::Compare(rhs->read1_idx_, lhs->read1_idx_);
        compare_difference = rhs->name_id_ - lhs->name_id_;
    }
//    assert(compare_difference != kEqualFlag);
    if (compare_difference > kEqualFlag){
        return true;
    } else {
        return false;
    }
}
MarkDuplicatePairEnds::MarkDuplicatePairEnds(int8_t read1_tid, int8_t read2_tid, int8_t orientation, int8_t lib_id, int16_t score,
                                             int32_t read1_pos, int32_t read2_pos, int64_t name_id)
        : read1_tid_(read1_tid), read2_tid_(read2_tid), orientation_(orientation), lib_id_(lib_id),
          score_(score), read1_pos_(read1_pos), read2_pos_(read2_pos), name_id_(name_id){

}


bool MarkDuplicatePairEnds::ReadEndsComparator(const std::shared_ptr<MarkDuplicatePairEnds> &lhs,
                                               const std::shared_ptr<MarkDuplicatePairEnds> &rhs) {
    const int kEqualFlag = 0;
    int compare_difference = rhs->lib_id_ - lhs->lib_id_;
    if (compare_difference == kEqualFlag) {
        compare_difference = rhs->read1_tid_ - lhs->read1_tid_;
    }
    if (compare_difference == kEqualFlag) {
        compare_difference = rhs->read1_pos_ - lhs->read1_pos_;
    }
    if (compare_difference == kEqualFlag) {
        compare_difference = rhs->orientation_ - lhs->orientation_;
    }
    if (compare_difference == kEqualFlag) {
        compare_difference = rhs->read2_tid_ - lhs->read2_tid_;
    }
    if (compare_difference == kEqualFlag) {
        compare_difference = rhs->read2_pos_ - lhs->read2_pos_;
    }
    if (compare_difference == kEqualFlag) {
        compare_difference = rhs->name_id_, lhs->name_id_;
    }
//    assert(compare_difference != kEqualFlag);
    if (compare_difference > kEqualFlag){
        return true;
    } else {
        return false;
    }
}
















//MarkDuplicateReadEnds::MarkDuplicateReadEnds(){
//
//}
//MarkDuplicateReadEnds::MarkDuplicateReadEnds(int64_t name_id, int8_t lib_id, int8_t read1_tid, int32_t read1_pos,
//                                             int8_t read2_tid, int8_t orientation, int16_t score,
//                                             const MarkDuplicateInputIndex &read1_idx)
//        :name_id_(name_id), lib_id_(lib_id), read1_tid_(read1_tid), read1_pos_(read1_pos),
//         read2_tid_(read2_tid), orientation_(orientation), score_(score), read1_idx_(read1_idx) {
//
//}
//
//MarkDuplicateReadEnds::MarkDuplicateReadEnds(int64_t name_id, int8_t lib_id, int8_t read1_tid, int32_t read1_pos,
//                                             int8_t read2_tid, int32_t read2_pos, int8_t orientation, int16_t score,
//                                             const MarkDuplicateInputIndex &read1_idx,
//                                             const MarkDuplicateInputIndex &read2_idx)
//        :name_id_(name_id), lib_id_(lib_id), read1_tid_(read1_tid), read1_pos_(read1_pos),
//         read2_tid_(read2_tid), read2_pos_(read2_pos), orientation_(orientation), score_(score),
//         read1_idx_(read1_idx), read2_idx_(read2_idx){
//
//}
//bool MarkDuplicateReadEnds::ReadEndsComparator(const std::shared_ptr<MarkDuplicateReadEnds> &lhs,
//                                               const std::shared_ptr<MarkDuplicateReadEnds> &rhs) {
//    const int kEqualFlag = 0;
//    int compare_difference = rhs->lib_id_ - lhs->lib_id_;
//    if (compare_difference == kEqualFlag) {
//        compare_difference = rhs->read1_tid_ - lhs->read1_tid_;
//    }
//    if (compare_difference == kEqualFlag) {
//        compare_difference = rhs->read1_pos_ - lhs->read1_pos_;
//    }
//    if (compare_difference == kEqualFlag) {
//        compare_difference = rhs->orientation_ - lhs->orientation_;
//    }
//    if (compare_difference == kEqualFlag) {
//        compare_difference = rhs->read2_tid_ - lhs->read2_tid_;
//    }
//    if (compare_difference == kEqualFlag) {
//        compare_difference = rhs->read2_pos_ - lhs->read2_pos_;
//    }
//    if (compare_difference == kEqualFlag) {
//        compare_difference = MarkDuplicateInputIndex::Compare(rhs->read1_idx_, lhs->read1_idx_);
//    }
//    if (compare_difference == kEqualFlag) {
//        compare_difference = MarkDuplicateInputIndex::Compare(rhs->read2_idx_, lhs->read2_idx_);
//    }
////    assert(compare_difference != kEqualFlag);
//    if (compare_difference > kEqualFlag){
//        return true;
//    } else {
//        return false;
//    }
//}
//
//bool operator==(const MarkDuplicateReadEnds &lhs, const MarkDuplicateReadEnds &rhs) {
//    if (lhs.name_id_ == rhs.name_id_ && lhs.read1_tid_ == rhs.read1_tid_
//        &&  lhs.read1_pos_ == rhs.read1_pos_ && lhs.read2_tid_ == rhs.read2_tid_
//        &&  lhs.read2_pos_ == rhs.read2_pos_ && lhs.score_ == rhs.score_
//        &&  lhs.orientation_ == rhs.orientation_ && lhs.lib_id_ == rhs.lib_id_) {
//        return true;
//    }
//    return false;
//}
//
//
//bool MarkDuplicateReadEnds::isPaired() {
//    return read2_tid_ != -1;
//}
