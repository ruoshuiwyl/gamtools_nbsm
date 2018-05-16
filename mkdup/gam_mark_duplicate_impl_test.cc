////
//// Created by ruoshui on 3/15/17.
////
//
//#include <gtest/gtest.h>
//#include "mark_duplicate_read_ends.h"
//#include "gam_mark_duplicate_impl.h"
//
//class MarkDupReadEndTest: public testing::Test {
//protected:
//    static void SetUpTestCase() {
//        pair_ends = new std::vector<MarkDuplicateReadEnds>;
//        frag_ends = new std::vector<MarkDuplicateReadEnds>;
//        int lib_id = 1;
//        for (int i = 0; i < 6; i += 3) {
//            int tid = i / 3;
//            int read1_pos = 1;
//            int read2_pos = 201;
//            MarkDuplicateReadEnds frag_end11(i, lib_id, tid, read1_pos, tid, 0, 4000, 0, 0, 0);
//            MarkDuplicateReadEnds frag_end12(i, lib_id, tid, read2_pos, tid, 1, 4000, 0, 0, 0);
//            MarkDuplicateReadEnds pair_end1(i, lib_id, tid, read1_pos, tid, read2_pos, 3, 8000, 0, 0);
//            frag_ends->push_back(frag_end11);
//            frag_ends->push_back(frag_end12);
//            pair_ends->push_back(pair_end1);
//            MarkDuplicateReadEnds frag_end21(i + 1, lib_id, tid, read1_pos, 0, 0, 3000, 0, 0, 0);
//            MarkDuplicateReadEnds frag_end22(i + 1, lib_id, tid, read2_pos, 0, 1, 3000, 0, 0, 0);
//            MarkDuplicateReadEnds pair_end2(i + 1, lib_id, tid, read1_pos, tid, read2_pos, 3, 6000, 0, 0);
//            frag_ends->push_back(frag_end21);
//            frag_ends->push_back(frag_end22);
//            pair_ends->push_back(pair_end2);
//            MarkDuplicateReadEnds frag_end31(i + 2, lib_id, tid, read1_pos, 0, 1, 3000, 0, 0, 0);
//            MarkDuplicateReadEnds frag_end32(i + 2, lib_id, tid, read2_pos, 0, 0, 3000, 0, 0, 0);
//            MarkDuplicateReadEnds pair_end3(i + 2, lib_id, tid, read1_pos, tid, read2_pos, 5, 6000, 0, 0);
//            frag_ends->push_back(frag_end31);
//            frag_ends->push_back(frag_end32);
//            pair_ends->push_back(pair_end3);
//        }
//
//        for ( int i = 6; i < 12; i += 3) {
//            int tid = (i - 6) / 3;
//            int mtid = -1;
//            int read1_pos  = 3;
//            MarkDuplicateReadEnds frag_end1(i, lib_id, tid, read1_pos, mtid, 0, 3000, 0, 0, 0);
//            MarkDuplicateReadEnds frag_end2(i + 1, lib_id, tid, read1_pos, mtid, 0, 4000, 0, 0, 0);
//            MarkDuplicateReadEnds frag_end3(i + 2, lib_id, tid, read1_pos, mtid, 1, 3000, 0, 0, 0);
//            frag_ends->push_back(frag_end1);
//            frag_ends->push_back(frag_end2);
//            frag_ends->push_back(frag_end3);
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
//};
//
//std::vector<MarkDuplicateReadEnds> * MarkDupReadEndTest::pair_ends = nullptr;
//std::vector<MarkDuplicateReadEnds> * MarkDupReadEndTest::frag_ends = nullptr;
//
//
//TEST_F(MarkDupReadEndTest, MarkDupUnitTest) {
//    std::string temp_markdup_path = "/home/ruoshui/test_log/chr21_1m/output";
//    GAMMarkDuplicateImpl gam_mark_dup_impl(2, temp_markdup_path);
//    for (int i = 0; i < pair_ends->size(); ++i){
//        auto pair_end = (*pair_ends)[i];
//        char *read1_gbam = new char[24];
//        char *read2_gbam = new char[24];
//        uint64_t value = 0;
//        memcpy(read1_gbam, (char*)&value, 1);
//        int read1_orientation, read2_orientation;
//        if (pair_end.orientation_ == 3) {
//            read1_orientation = 0;
//            read2_orientation = 1;
//        } else if (pair_end.orientation_ == 5) {
//            read1_orientation = 1;
//            read2_orientation = 0;
//        }
//        value = uint64_t(pair_end.read1_tid_) << 24 | uint64_t(read1_orientation) << 16 |
//                uint64_t(pair_end.score_>>1) ;
//        memcpy(read1_gbam + 1, (char*)&value, 4);
//        value =  pair_end.read1_pos_;
//        memcpy(read1_gbam + 5, (char*)&value, 4);
//        value = uint64_t(pair_end.name_id_) << 8 | uint64_t(pair_end.lib_id_);
//        memcpy(read1_gbam + 9, (char*)&value, 8);
//
//        value = 0;
//        memcpy(read2_gbam, (char*)&value, 1);
//        value = uint64_t(pair_end.read2_tid_) << 24 | uint64_t(read2_orientation) << 16 |
//                uint64_t(pair_end.score_>>1) ;
//        memcpy(read2_gbam + 1, (char*)&value, 4);
//        value = pair_end.read2_pos_;
//        memcpy(read2_gbam + 5, (char*)&value, 4);
//        value = uint64_t(pair_end.name_id_) << 8 | uint64_t(pair_end.lib_id_);
//        memcpy(read2_gbam + 9, (char*)&value, 8);
//        gam_mark_dup_impl.StorePairEndRecord(read1_gbam, read2_gbam);
//    }
//    for (int i = 0; i < frag_ends->size(); ++i){
//        auto frag_end = (*frag_ends)[i];
//        if (frag_end.read2_tid_ == -1) {
//            char *read1_gbam = new char[24];
//            uint64_t value = 0;
//            memcpy(read1_gbam, (char *) &value, 1);
//            value = uint64_t(frag_end.read1_tid_) << 24 | uint64_t(frag_end.orientation_) << 16 |
//                    uint64_t(frag_end.score_>>1) ;
//            memcpy(read1_gbam + 1, (char*)&value, 4);
//            value =  frag_end.read1_pos_;
//            memcpy(read1_gbam + 5, (char*)&value, 4);
//            value = uint64_t(frag_end.name_id_) << 8 | uint64_t(frag_end.lib_id_);
//            memcpy(read1_gbam + 9, (char *) &value, 8);
//            gam_mark_dup_impl.StorePairEndRecord(read1_gbam, nullptr);
//        }
//    }
//    gam_mark_dup_impl.MarkDuplication(12);
//    std::vector<bool> result = {false, true, false, false, true, false, true, false, false, true, false, false};
//    for ( int i = 0; i < 12; ++i){
//        EXPECT_EQ(result[i], gam_mark_dup_impl.IsMarkDuplicate(i));
//    }
//
//
//}
//
