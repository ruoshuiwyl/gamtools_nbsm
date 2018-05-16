//
// Created by ruoshui on 4/27/18.
//


#include <gtest/gtest.h>
#include "bam_partition_data.h"
#include "gam_block.h"

namespace gamtools_sm {

    TEST(PartitionDataTest, MergeSort) {
        Channel<std::unique_ptr<GAMBlock>> sort_channel;
        std::string filename;
        BAMPartitionData partition_data(sort_channel, filename);
    }


}

int main(int argc, char *argv[]) {
    testing::InitGoogleTest(&argc, argv);
    int result = RUN_ALL_TESTS();
    return result;
}