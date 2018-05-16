//
// Created by ruoshui on 3/15/17.
//


#include <gtest/gtest.h>
#include "mark_duplicate_read_ends.h"
#include "mark_duplicate_region.h"
#include "gam_mark_duplicate_impl.h"
//
//class SingleChrMarkDupTest : public testing::Test {
//protected:
//    static void SetUpTestCase() {
//        pair_ends = new std::vector<MarkDuplicateReadEnds>;
//        frag_ends = new std::vector<MarkDuplicateReadEnds>;
//        std::vector<int8_t> frag_dir = {0, 0, 0, 1, 1, 0, 1, 1};
//        std::vector<int8_t> pair_dir = {2, 3, 5, 4};
//        int lib_id = 1;
//        int tid = 0;
//        for (int i = 0; i < 8; i += 2){
//            int read1_pos = ((i>>1) + 1);
//            int read2_pos = ((i>>1) + 1) * 101;
//            MarkDuplicateReadEnds frag_end11(i, lib_id, tid, read1_pos, tid, frag_dir[i], 3000, 0);
//            MarkDuplicateReadEnds frag_end12(i, lib_id, tid, read2_pos, tid, frag_dir[i + 1], 3000, 0);
//            MarkDuplicateReadEnds pair_end1(i, lib_id, tid, read1_pos, tid, read2_pos, pair_dir[i >> 1], 6000, 0, 0);
//            frag_ends->push_back(frag_end11);
//            frag_ends->push_back(frag_end12);
//            pair_ends->push_back(pair_end1);
//            MarkDuplicateReadEnds frag_end21(i + 1, lib_id, tid, read1_pos, 0, frag_dir[i], 4000, 0);
//            MarkDuplicateReadEnds frag_end22(i + 1, lib_id, tid, read2_pos, 0, frag_dir[i + 1], 4000, 0);
//            MarkDuplicateReadEnds pair_end2(i + 1, lib_id, tid, read1_pos, tid, read2_pos, pair_dir[i >> 1], 8000, 0, 0);
//            frag_ends->push_back(frag_end21);
//            frag_ends->push_back(frag_end22);
//            pair_ends->push_back(pair_end2);
//        }
//        int mtid = -1;
//        for ( int j = 8, i = 1; j < 17; j += 3) {
//            int read1_pos = i;
//            MarkDuplicateReadEnds frag_end1(j, lib_id, tid, read1_pos, mtid, (i >> 1) & 1, 3000, 0, 0, 0);
//            MarkDuplicateReadEnds frag_end2(j + 1, lib_id, tid, read1_pos, mtid, (i >> 1) & 1, 4000, 0, 0, 0);
//            MarkDuplicateReadEnds frag_end3(j + 2, lib_id, tid, read1_pos, mtid, ((i >> 1) & 1) ^ 1, 3000, 0, 0, 0);
//            frag_ends->push_back(frag_end1);
//            frag_ends->push_back(frag_end2);
//            frag_ends->push_back(frag_end3);
//            i += 2;
//        }
//
//    }
//    static void TearDownTestCase() {
//        frag_ends->clear();
//        pair_ends->clear();
//        delete frag_ends;
//        delete pair_ends;
//        frag_ends = nullptr;
//        pair_ends = nullptr;
//    }
//    static std::vector<MarkDuplicateReadEnds> *pair_ends;
//    static std::vector<MarkDuplicateReadEnds> *frag_ends;
//
//
//};
//std::vector<MarkDuplicateReadEnds> * SingleChrMarkDupTest::pair_ends = nullptr;
//std::vector<MarkDuplicateReadEnds> * SingleChrMarkDupTest::frag_ends = nullptr;
//
//
//TEST_F(SingleChrMarkDupTest, MarkDuplicateStoreAndLoad){
//    MarkDuplicateRegion single_chr_markdup(0);
//    std::string temp_markdup_path = "/home/ruoshui/test_log/mark_dup/";
//    for ( int i = 0; i < frag_ends->size(); ++i) {
//        single_chr_markdup.AddFragEnd(frag_ends);
//        if ( i == 10){
//            single_chr_markdup.StoreFileFragEnds(temp_markdup_path);
//        }
//    }
//    for (int i = 0; i < pair_ends->size(); ++i){
//        single_chr_markdup.AddPairEnd((*pair_ends)[i]);
//        if ( i == 3) {
//            single_chr_markdup.StoreFilePairEnds(temp_markdup_path);
//        }
//    }
//    int total_read_id = 17;
//    GAMMarkDuplicateImpl::SetMarkDuplicateReadNameNumber(total_read_id);
//    single_chr_markdup.ProcessMarkDuplicate(temp_markdup_path);
//    std::vector<bool> actual_result = {
//            true, false, true, false, true, false, true, false, true, true,
//            false, true, true, false, true, false, false
//    };
//    for ( int i = 0; i < total_read_id; ++i ) {
//        EXPECT_EQ(actual_result[i], GAMMarkDuplicateImpl::IsMarkDuplicate(i));
//    }
//}

#include <gtest/gtest.h>
#include <fstream>

TEST(GAMMarkDuplicate, UNITTEST) {
    std::shared_ptr<MarkDuplicatePairEnds> lhs{new MarkDuplicatePairEnds};
    std::shared_ptr<MarkDuplicatePairEnds> rhs{new MarkDuplicatePairEnds};
//    lhs->lib_id_ = 1;
//    lhs->name_id_ = 111643;
//    lhs->read1_tid_ = 0;
//    lhs->read1_pos_ = 9435002;
//    lhs->read1_idx_.read_sort_idx_ = (uint64_t(lhs->read1_tid_) << 32) | ((lhs->read1_pos_ + 1) << 1 ) | 0;
//    lhs->read1_idx_.read_input_idx_ = (lhs->name_id_ << 1 | 0) << 8 ;
//    lhs->read2_tid_ = 0;
//    lhs->read2_pos_ = 9435257 + 100;
//    lhs->read2_idx_.read_sort_idx_ = (uint64_t(lhs->read2_tid_) << 32) | (int(9435257) << 1 ) | 0;;
//    lhs->read2_idx_.read_input_idx_ = (lhs->name_id_ << 1 | 1) << 8;
//    lhs->orientation_ = MarkDuplicateReadEnds::FR;
//    lhs->score_ = 3933 * 2;
//
//
//    rhs->lib_id_ = 1;
//    rhs->name_id_ = 14216330;
//    rhs->read1_tid_ = 0;
//    rhs->read1_pos_ = 9435002;
//    rhs->read1_idx_.read_sort_idx_ = (uint64_t(rhs->read1_tid_) << 32) | ((rhs->read1_pos_ + 1) << 1 ) | 0;
//    rhs->read1_idx_.read_input_idx_ = (rhs->name_id_ << 1 | 0) << 8 ;
//    rhs->read2_tid_ = 0;
//    rhs->read2_pos_ = 9435257 + 100;
//    rhs->read2_idx_.read_sort_idx_ = (uint64_t(rhs->read2_tid_) << 32) | (int(9435257) << 1 ) | 0;;
//    rhs->read2_idx_.read_input_idx_ = (rhs->name_id_ << 1 | 1) << 8;
//    rhs->orientation_ = MarkDuplicateReadEnds::FR;
//    rhs->score_ = 3933 * 2;
    int lhs_read1_pos = 121485201;
    int lhs_read2_pos = 121485301;
    lhs->lib_id_ = 1;
    lhs->name_id_ = 146865929;
    lhs->read1_tid_ = 1;
    lhs->read1_pos_ = lhs_read1_pos;
//    lhs->read1_idx_.read_sort_idx_ = (uint64_t(lhs->read1_tid_) << 32) | ((lhs_read1_pos+ 1) << 1 ) | 0;
//    lhs->read1_idx_.read_input_idx_ = (lhs->name_id_ << 1 | 0) << 8 ;
    lhs->read2_tid_ = 1;
    lhs->read2_pos_ = lhs_read2_pos;
//    lhs->read2_idx_.read_sort_idx_ = (uint64_t(lhs->read2_tid_) << 32) | (lhs_read2_pos << 1 ) | 0;;
//    lhs->read2_idx_.read_input_idx_ = (lhs->name_id_ << 1 | 1) << 8;
    lhs->orientation_ = 3;
    lhs->score_ = 3933 * 2;

    int rhs_read1_pos = 121485201;
    int rhs_read2_pos = 121485301;
    rhs->lib_id_ = 1;
    rhs->name_id_ = 143451340;
    rhs->read1_tid_ = 1;
    rhs->read1_pos_ = rhs_read1_pos;
//    rhs->read1_idx_.read_sort_idx_ = (uint64_t(rhs->read1_tid_) << 32) | ((rhs_read2_pos + 1) << 1 ) | 0;
//    rhs->read1_idx_.read_input_idx_ = (rhs->name_id_ << 1 | 0) << 8 ;
    rhs->read2_tid_ = 1;
    rhs->read2_pos_ = rhs_read2_pos;
//    rhs->read2_idx_.read_sort_idx_ = (uint64_t(rhs->read2_tid_) << 32) | (rhs_read1_pos << 1 ) | 0;;
//    rhs->read2_idx_.read_input_idx_ = (rhs->name_id_ << 1 | 1) << 8;
    rhs->orientation_ = 3;
    rhs->score_ = 3933 * 2;

    std::cerr << int(MarkDuplicatePairEnds::ReadEndsComparator(lhs, rhs)) << std::endl;
    std::cerr << int(MarkDuplicatePairEnds::ReadEndsComparator(rhs, lhs)) << std::endl;
//    std::string test_input_file = "/home/ruoshui/test_log/chr21_1m/output/test";
//    std::ofstream ofs{test_input_file, std::ofstream::binary | std::ofstream::out};
//    MarkDuplicateRegion::WriteMarkDupReadEnd(ofs,lhs);
//    MarkDuplicateRegion::WriteMarkDupReadEnd(ofs,rhs);
//    ofs.close();
//    std::shared_ptr<MarkDuplicateReadEnds> r1(new MarkDuplicateReadEnds);
//    std::shared_ptr<MarkDuplicateReadEnds> r2(new MarkDuplicateReadEnds);
//    std::ifstream ifs{test_input_file, std::ofstream::binary | std::ofstream::in};
//    MarkDuplicateRegion::ReadMarkDupReadEnd(ifs, r1);
//
//    MarkDuplicateRegion::ReadMarkDupReadEnd(ifs, r2);
//    std::cerr << r1->name_id_ << "\t" << r2->name_id_ << std::endl;
//    std::cerr << r1->read1_pos_ << "\t" << r2->read1_pos_ << std::endl;





}

int main( int argc, char *argv[]) {

    return 0;
}

