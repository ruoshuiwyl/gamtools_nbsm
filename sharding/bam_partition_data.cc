//
// Created by ruoshui on 4/24/18.
//


#include <queue>

#include <util/slice.h>
#include <iostream>
#include <fstream>
#include <mkdup/gam_mark_duplicate_impl.h>
#include <util/glogger.h>
#include "bam_partition_data.h"
#include "gam_block.h"
#include "bam_block.h"

#include "util/create_index.h"

namespace gamtools {
    const static int kBAMBlockSize = 65536;
    ShardingPartitionData::ShardingPartitionData(ArrayBlockQueue <std::unique_ptr<GAMBlock>> &sort_channel, const PartitionData &part_data,
                                                     const int block_size)
            : append_(false),
              block_cnt_(0),
              block_size_(block_size),
              sort_channel_(sort_channel),
              partition_data_(part_data) {
        gam_block_ptr_ = std::unique_ptr<GAMBlock>(new GAMBlock(block_size_, append_, block_cnt_, partition_data_.filename()));
        block_cnt_++;
    }

    ShardingPartitionData::~ShardingPartitionData() {

    }

    void ShardingPartitionData::Add(const Slice &slice) {
        if (!gam_block_ptr_->Insert(slice)) {
            if (gam_block_ptr_->full()) {
                sort_channel_.write(std::move(gam_block_ptr_));
                append_ = true;
                gam_block_ptr_ = std::unique_ptr<GAMBlock>(new GAMBlock(block_size_, append_, block_cnt_, partition_data_.filename()));
                block_cnt_++;
            }
        }
    }

    void ShardingPartitionData::sendEof() {
        if (!gam_block_ptr_->empty()) {
            sort_channel_.write(std::move(gam_block_ptr_));
        } else {
            block_cnt_--;
        }
    }

//    struct BAMSlice {
//        int block_idx_;
//        std::vector< Slice>::const_iterator slice_itr_;
//    };
//
//    class BAMSliceComparator {
//    public:
//        bool operator() (const BAMSlice &a, const BAMSlice &b) {
//            return GAMComparator(*a.slice_itr_, *b.slice_itr_);
//        }
//    };

//    void BAMPartitionData::MergeSort() {
//        bam_block_idx_ = 0;
//        bam_block_ptr_ = std::unique_ptr<BAMBlock>( new BAMBlock(kBAMBlockSize, partition_data_.sharding_id(), bam_block_idx_ ,false));
//        bam_block_idx_++;
//        if (gam_blocks_.empty()) {
//            MergeSendEof();
//            return ;
//        }
//        if (gam_blocks_.size() > 1) {
//            std::priority_queue<BAMSlice, std::vector<BAMSlice>, BAMSliceComparator> bam_heap;
//            for (int i = 0; i < gam_blocks_.size(); ++i) {
//                BAMSlice bam_slice;
//                bam_slice.block_idx_ = i;
//                bam_slice.slice_itr_ = gam_blocks_[i]->slices().begin();
//                if (bam_slice.slice_itr_ != gam_blocks_[i]->slices().end()) {
//                    bam_heap.push(bam_slice);
//                }
//            }
//            while (!bam_heap.empty()) {
//                auto data = bam_heap.top();
//                int64_t read_id = reinterpret_cast<const int64_t *>(data.slice_itr_->data())[1];
//                const bool markdup_flag = GAMMarkDuplicateImpl::IsMarkDuplicate(read_id); // Markdup read
//                const char *bam = data.slice_itr_->data() + 20;
//                if (markdup_flag) { // flag add mark dup flag 0x400
//                    int bam_flag = reinterpret_cast< const int *>(bam)[4] | (0x400 << 16);
//                    reinterpret_cast< int *>(const_cast<char *> (bam))[4] = bam_flag;
//                }
//                Slice slice(bam, reinterpret_cast<const int *>(bam)[0]);
//                if (!InsertBAMSlice(slice)) {
//                    GLOG_ERROR << "Insert bam slice " << __FUNCTION__ << std::endl;
//                }
//                ++data.slice_itr_;
//                bam_heap.pop();
//                if (data.slice_itr_ != gam_blocks_[data.block_idx_]->slices().end()) {
//                    bam_heap.push(data);
//                }
//            }
//
//        } else {
//            auto &slice = gam_blocks_[0]->slices();
//            for (auto it = slice.begin(); it != slice.end(); ++it){
//                int64_t read_id = reinterpret_cast<const int64_t *>(it->data())[1];
//                const bool markdup_flag = GAMMarkDuplicateImpl::IsMarkDuplicate(read_id); // Markdup read
//                const char *bam = it->data() + 20;
//                if (markdup_flag) { // flag add mark dup flag 0x400
//                    int bam_flag = reinterpret_cast< const int *>(bam)[4] | (0x400 << 16);
//                    reinterpret_cast< int *>(const_cast<char *> (bam))[4] = bam_flag;
//                }
//                Slice slice(bam, reinterpret_cast<const int *>(bam)[0]);
//                if (!InsertBAMSlice(slice)) {
//                    GLOG_ERROR << "Insert bam slice " << __FUNCTION__ << std::endl;
//                }
//            }
//        }
//        MergeSendEof();
//    }

//    bool BAMPartitionData::InsertBAMSlice(Slice &bam_slice) {
//        bool result = true;
//        if (!bam_block_ptr_->Insert(bam_slice)) {
//            if (bam_block_ptr_->full()) {
//                output_bam_channel_.write(std::move(bam_block_ptr_));
//                bam_block_ptr_ = std::unique_ptr<BAMBlock>(new BAMBlock(kBAMBlockSize,  partition_data_.sharding_id(), bam_block_idx_ ,false));
//                bam_block_idx_++;
//                if (!bam_block_ptr_->Insert(bam_slice) ){
//                    result = false;
//                }
//            } else {
//                assert(true);
//            }
//        }
//        return result;
//    }

//    void BAMPartitionData::MergeSendEof() {
//        bam_block_ptr_->SendEof();
//        output_bam_channel_.write(std::move(bam_block_ptr_));
//    }

//    void BAMPartitionData::ReadGAMBlock() {
//        auto &filename = partition_data_.filename();
//        std::ifstream ifs(filename);
//        for (int i = 0; i < block_cnt_; ++i) {
//            size_t size;
//            char *data = new char[block_size_];
//            ifs.read(reinterpret_cast<char*>(size), sizeof(size));
//            ifs.read(data, size);
//            std::unique_ptr<GAMBlock> gam_block = std::unique_ptr<GAMBlock>(new GAMBlock(block_size_, false, i, filename));
//            gam_block->Insert(data, size);
//            gam_blocks_.push_back(std::move(gam_block));
//            delete data;
//        }
//        ifs.close();
//    }

}