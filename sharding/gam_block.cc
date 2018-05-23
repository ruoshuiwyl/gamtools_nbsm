//
// Created by ruoshui on 4/25/18.
//

#include <fstream>

#include "gam_block.h"
#include <snappy.h>
#include <iostream>
#include <algorithm>
#include <util/glogger.h>
#include <lib/htslib-1.3.1/htslib/sam.h>
#include "util/slice.h"

namespace gamtools {

    GAMBlock::GAMBlock(size_t block_size, bool append, int block_id, std::string file_name)
    : Block(block_size), append_(append), block_id_(block_id), file_name_(file_name), compress_(false){

    }
    void GAMBlock::Compress() {
        char *compress = new char[snappy::MaxCompressedLength(block_len_)];
        size_t outlen;
        snappy::RawCompress(block_data_, block_len_, compress, &outlen);
        compress_ = true;
        delete [] block_data_;
        block_data_ = compress;
        block_len_  = outlen;
    }

    bool GAMBlock::Decompress() {
        if (compress_) {
            size_t ulength = 0;
            if (!snappy::GetUncompressedLength(block_data_, block_len_, &ulength)) {
                return false;
            }
            assert(block_size_ >= ulength);
            char *uncompress = new char[block_size_];
            if (!snappy::RawUncompress(block_data_, block_len_, uncompress)){
                return false;
            }
            delete [] block_data_;
            block_data_ = uncompress;
            block_len_ = ulength;
            compress_ = false;
            return  true;
        }
        return  false;
    }


    void GAMBlock::BuildIterator() {
        assert(compress_ == false);
        slices_.clear();

        alloc_ptr_ = block_data_;
        size_t use_len= 0;
        while ( use_len < block_len_) {
            size_t bam_size = GAMLen(alloc_ptr_);
            Slice slice(alloc_ptr_, bam_size);
            slices_.push_back(slice);
            alloc_ptr_ += bam_size;
            use_len += bam_size;
        }
    }
    void GAMBlock::Write() {
        std::ofstream ofs;
        if (append_) {
            ofs.open(file_name_, std::ofstream::out | std::ofstream::app);
        } else {
            ofs.open(file_name_, std::ofstream::out );
        }
        if (!ofs.is_open()) {
            GLOG_ERROR << "Open file\t" << file_name_ ;
        } else {
            GLOG_INFO << "Open file\t " << file_name_;
        }
        size_t block_size = size();
        ofs.write((char *)(&block_size), sizeof(size_t));
        ofs.write(data(), block_size);
        ofs.close();
        GLOG_INFO << "Finish write file " << file_name_;
    }

    bool GAMComparator(const Slice &a, const Slice &b)  {
        bool r;
        uint64_t a_key = reinterpret_cast<const uint64_t *>(a.data())[0];
        uint64_t b_key = reinterpret_cast<const uint64_t *>(b.data())[0];
        int64_t a_read_id = reinterpret_cast<const int64_t  *>(a.data())[1];
        int64_t b_read_id = reinterpret_cast<const int64_t  *>(b.data())[1];
        if (a_key < b_key) {
            r = true;
        } else if (a_key > b_key) {
            r = false;
        } else {
            if (a_read_id < b_read_id) {
                r = true;
            } else {
                r = false;
            }
        }
        return r;
    }

    std::unique_ptr<GAMBlock> GAMBlock::BlockSort() {
        std::unique_ptr<GAMBlock> sort_block(new GAMBlock(block_size_, append_, block_id_, file_name_));
        std::sort(slices_.begin(), slices_.end(), GAMComparator);
//        std::stable_sort(slices_.begin(), slices_.end(), GAMComparator);
        for (auto it = slices_.begin(); it != slices_.end(); ++it) {
            sort_block->Insert(*it);
        }
        return sort_block;
    }


    GAMBlock::~GAMBlock() {
//        delete Block::block_data_;
    }



}