//
// Created by ruoshui on 4/26/18.
//

#include <gtest/gtest.h>
#include "gam_block.h"
#include "util/slice.h"

namespace gamtools {
    struct SortData {
        SortData(int64_t key, int64_t name_id) : key_(key), name_id_(name_id), line_id_(1), bam_len_(64){}
        int64_t key_;
        int64_t name_id_;
        int64_t line_id_;
        int64_t bam_len_;
        int64_t data[8];
    };
    static void TestDataToString( char *data){
        int64_t *d = reinterpret_cast<int64_t *>(data);
        std::cout << d[0] << "\t" << d[1] << "\t" << (d[2]) << "\t" << (d[3]) << std::endl;
    }
    static char *TestDatatoChar(SortData  &test_data) {
        char *data = new char[sizeof(SortData)];
        memcpy(data, (void *)&test_data, sizeof(SortData));
        return data;
    }

    TEST(BAMBlockSortTest, Sort) {
        GAMBlock ori_block(1024, false, 0, "test.txt");

        for (int i = 0; i < 10; ++i) {
            SortData data(i, i);
            auto sort_data = TestDatatoChar(data);
            Slice slice(sort_data, sizeof(SortData));
            if (!ori_block.Insert(slice)){
                delete [] sort_data;
                break;
            }
            delete [] sort_data;
        }
        auto &ori_slices = ori_block.slices();
//        std::cout << "before sort" << std::endl;
        for (auto it = ori_slices.begin(); it != ori_slices.end(); ++it) {
//            TestDataToString((char*)it->data());
        }
        auto sort_block = ori_block.BlockSort();
        auto &slices = sort_block->slices();
//        std::cout << "after sort" << std::endl;
        for (auto it = slices.begin(); it != slices.end(); ++it) {
//            TestDataToString((char*)it->data());
        }
    }

    TEST(BAMBlockSortTest, MutliSort) {
        GAMBlock ori_block(1024, false, 0, "test.txt");
        std::vector<int64_t> keys = {1, 1, 1, 4, 4, 4, 7, 7, 9, 10};
        for (int i = 0; i < 10; ++i) {
            SortData data(keys[i], 0);
            data.line_id_ = i;
            auto sort_data = TestDatatoChar(data);
            Slice slice(sort_data, sizeof(SortData));
            if (!ori_block.Insert(slice)){
                delete [] sort_data;
                break;
            }
            delete [] sort_data;
        }
        auto &ori_slices = ori_block.slices();
//        std::cout << "before sort" << std::endl;
//        for (auto it = ori_slices.begin(); it != ori_slices.end(); ++it) {
//            TestDataToString((char*)it->data());
//        }
        auto sort_block = ori_block.BlockSort();
        auto &slices = sort_block->slices();
//        std::cout << "after sort" << std::endl;
        for (auto it = slices.begin(); it != slices.end(); ++it) {
//            TestDataToString((char*)it->data());
        }
    }
}

int main(int argc, char *argv[] ) {
    testing::InitGoogleTest(&argc, argv);
    int result = RUN_ALL_TESTS();
    return result;
}