//
// Created by ruoshui on 4/26/18.
//


#include <gtest/gtest.h>
#include <util/slice.h>


#include "gam_block.h"
namespace gamtools{
    TEST(BAMBlockTest, Empty) {
        std::string filename = "test.txt";
        GAMBlock bam_block(1024, false, 0, filename);
        assert(bam_block.empty());
        const char *test = "hello, world";
        Slice slice(test, strlen(test));
        assert(bam_block.Insert(slice));
        Slice slice1(test, 1024);
        assert(!bam_block.Insert(slice1));
    }
    TEST(BAMBlockTest, Insert) {
        std::string filename = "test.txt";
        GAMBlock bam_block(1024, false, 0, filename);
        assert(bam_block.empty());
        std::string test = "hello, world";

        for ( int i = 0; i < 1028; ++i) {
            std::string test_string = test + std::to_string(i);
            Slice slice(test_string.data(), test_string.size());
            if (!bam_block.Insert(slice)) {
                break;
            }
        }
//        std::cout << bam_block.data() << std::endl;
        assert(bam_block.full());
//        auto &slices = bam_block.slices();
//        for (auto it = slices.begin(); it != slices.end(); ++it) {
//            std::cout << it->ToString() << std::endl;
//        }

    }

    struct TestData {
        TestData(int64_t key) : key_(key), name_id_(1), line_id_(1), bam_len_(64){}
        int64_t key_;
        int64_t name_id_;
        int64_t line_id_:32;
        int64_t bam_len_:32;
        int64_t data[8];
    };
    static std::string TestDataToString( char *data){
        int64_t *d = reinterpret_cast<int64_t *>(data);
        std::cout << d[0] << "\t" << d[1] << "\t" << (d[2] >> 32) << "\t" << (d[2] && 0xffffffff) << std::endl;
    }
    static char *TestDatatoChar(TestData  &test_data) {
        char *data = new char[sizeof(TestData)];
        memcpy(data, (void *)&test_data, sizeof(TestData));
        return data;
    }
    TEST(BAMBlockTest, Compress) {
        std::string filename = "test.txt";
        GAMBlock bam_block(1024, false, 0, filename);
        for (int i = 0; i < 1024; ++i) {
            TestData test_data(i);
            char *data = TestDatatoChar(test_data);
            Slice slice(data, sizeof(TestData));
            if (!bam_block.Insert(slice)) {
                delete [] data;
                break;
            }
            delete [] data;
        }
        bam_block.Compress();
        assert(bam_block.Decompress()) ;
        bam_block.BuildIterator();

        auto &slices = bam_block.slices();
        for (auto it = slices.begin(); it != slices.end(); ++it) {
            std::cout << TestDataToString((char*)it->data());
        }
    }

}

int main(int argc, char *argv[] ) {
    testing::InitGoogleTest(&argc, argv);
    int result = RUN_ALL_TESTS();
    return result;
}