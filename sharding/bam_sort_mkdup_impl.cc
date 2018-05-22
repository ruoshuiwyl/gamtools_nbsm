//
// Created by ruoshui on 5/22/18.
//

#include <util/glogger.h>
#include "util/slice.h"
#include "util/create_index.h"



#include <nbsm/options.h>
#include <snappy.h>
#include <mkdup/gam_mark_duplicate_impl.h>
#include <htslib/bgzf.h>
#include "bam_sort_mkdup_impl.h"
#include "sharding/bam_partition_data.h"
#include "sharding/bam_block.h"



namespace gamtools {

    struct BAMSlice {
        int block_idx_;
        std::vector< Slice>::const_iterator slice_itr_;
    };

    class BAMSliceComparator {
    public:
        bool operator() (const BAMSlice &a, const BAMSlice &b) {
            return GAMComparator(*a.slice_itr_, *b.slice_itr_);
        }
    };

    const static int kBAMBlockSize = 65536;

    void BAMSortMkdupImpl::ReadGAMPartitionData() {
        GLOG_INFO << "Start Read gam block";
        for (auto &part : partition_datas_) {
            ReadGAMBlock(part);
        }
        GLOG_INFO << "Finish Read gam block";
    }

    void BAMSortMkdupImpl::SortGAMPartitionData() {

    }


    void BAMSortMkdupImpl::ReadGAMBlock(std::unique_ptr<gamtools::ShardingPartitionData> &part_data) {
        auto &part = part_data->partition_data();
        std::unique_ptr<GAMPartitionData> partition_data_ptr(new GAMPartitionData(part.sharding_id()));
        auto &filename = part.filename() ;
        std::ifstream ifs(filename);
        int block_cnt = part_data->block_cnt();

        char *data = new char[block_size_];
        for (int block_idx = 0; block_idx < block_cnt; ++block_idx) {
            size_t size;
            ifs.read(reinterpret_cast<char*>(size), sizeof(size));
            assert(block_size_ > size);
            ifs.read(data, size);
            std::unique_ptr<Block> gam_block = std::unique_ptr<Block>(new Block(block_size_));
            gam_block->Insert(data, size);
            partition_data_ptr->blocks.push_back(std::move(gam_block));
        }
        gam_part_channel_.write(std::move(partition_data_ptr));
        delete [] data;
        ifs.close();
    }

    void BAMSortMkdupImpl::PartitonDecompressMerge() {
        std::unique_ptr<GAMPartitionData> partition_data_ptr;
        while (gam_part_channel_.read(partition_data_ptr)) {
            Decompress(partition_data_ptr);
            MergePartition(partition_data_ptr);
        }

    }
    void BAMSortMkdupImpl::Decompress(std::unique_ptr<gamtools::GAMPartitionData> &gam_part) {
        std::vector<std::unique_ptr<Block>> decompress_blocks;
        char *uncompress = new char[block_size_];
        for (auto& block : gam_part->blocks) {
            size_t ulength = 0;
            if (!snappy::GetUncompressedLength(block->data(), block->size(), &ulength)) {
                GLOG_ERROR << "Snappy get un compressed length ";
            }
            assert(block_size_ >= ulength);
            std::unique_ptr<Block> dblock(new Block(block_size_));
            if (!snappy::RawUncompress(block->data(), block->size(), uncompress)){
                GLOG_ERROR << "Snappy  uncompressed ";
            }
            dblock->Insert(uncompress, ulength);

            // Build GAM slice ;
            auto &slices = dblock->slices();
            slices.clear();
            const char *alloc_ptr_ = dblock->data();
            size_t use_len= 0;
            while (use_len < dblock->size()) {
                int  bam_size = reinterpret_cast<const int *>(alloc_ptr_)[5] + 24;
                Slice slice(alloc_ptr_, bam_size);
                slices.push_back(slice);
                alloc_ptr_ += bam_size;
                use_len += bam_size;
            }
            decompress_blocks.push_back(std::move(dblock));
        }
        swap(gam_part->blocks, decompress_blocks);
        delete [] uncompress;
    }

    void  BAMSortMkdupImpl::MergePartition(std::unique_ptr<GAMPartitionData> &gam_part){
        auto bam_block_ptr = std::unique_ptr<BAMBlock>( new BAMBlock(kBAMBlockSize, gam_part->sharding_idx, 0 ,false));
                auto &gam_blocks = gam_part->blocks;
        if (gam_blocks.empty()) {
            bam_block_ptr->SendEof();
            output_bam_channel_.write(std::move(bam_block_ptr));
            return ;
        }
        if (gam_blocks.size() > 1) {
            std::priority_queue<BAMSlice, std::vector<BAMSlice>, BAMSliceComparator> bam_heap;
            for (int i = 0; i < gam_blocks.size(); ++i) {
                BAMSlice bam_slice;
                bam_slice.block_idx_ = i;
                bam_slice.slice_itr_ = gam_blocks[i]->slices().begin();
                if (bam_slice.slice_itr_ != gam_blocks[i]->slices().end()) {
                    bam_heap.push(bam_slice);
                }
            }
            while (!bam_heap.empty()) {
                auto data = bam_heap.top();
                int64_t read_id = reinterpret_cast<const int64_t *>(data.slice_itr_->data())[1];
                const bool markdup_flag = GAMMarkDuplicateImpl::IsMarkDuplicate(read_id); // Markdup read
                const char *bam = data.slice_itr_->data() + 20;
                if (markdup_flag) { // flag add mark dup flag 0x400
                    int bam_flag = reinterpret_cast< const int *>(bam)[4] | (0x400 << 16);
                    reinterpret_cast< int *>(const_cast<char *> (bam))[4] = bam_flag;
                }
                Slice slice(bam, reinterpret_cast<const int *>(bam)[0]);
                InsertBAMSlice(slice, bam_block_ptr);
                ++data.slice_itr_;
                bam_heap.pop();
                if (data.slice_itr_ != gam_blocks[data.block_idx_]->slices().end()) {
                    bam_heap.push(data);
                }
            }

        } else {
            auto &slice = gam_blocks[0]->slices();
            for (auto it = slice.begin(); it != slice.end(); ++it){
                int64_t read_id = reinterpret_cast<const int64_t *>(it->data())[1];
                const bool markdup_flag = GAMMarkDuplicateImpl::IsMarkDuplicate(read_id); // Markdup read
                const char *bam = it->data() + 20;
                if (markdup_flag) { // flag add mark dup flag 0x400
                    int bam_flag = reinterpret_cast< const int *>(bam)[4] | (0x400 << 16);
                    reinterpret_cast< int *>(const_cast<char *> (bam))[4] = bam_flag;
                }
                Slice slice(bam, reinterpret_cast<const int *>(bam)[0]);
                InsertBAMSlice(slice, bam_block_ptr);
            }
        }
        bam_block_ptr->SendEof();
        output_bam_channel_.write(std::move(bam_block_ptr));
    }

    void
    BAMSortMkdupImpl::InsertBAMSlice(gamtools::Slice &slice, std::unique_ptr<BAMBlock> &bam_block_ptr) {
        if (!bam_block_ptr->Insert(slice)) {
            if (bam_block_ptr->full()) {
                int sharding_idx = bam_block_ptr->sharding_idx();
                int bam_block_idx = bam_block_ptr->block_idx() + 1;
                output_bam_channel_.write(std::move(bam_block_ptr));
                bam_block_ptr = std::unique_ptr<BAMBlock>(
                        new BAMBlock(kBAMBlockSize, sharding_idx, bam_block_idx, false));
                bam_block_ptr->Insert(slice);
            } else {
                GLOG_ERROR << "BAM Block insert failed but not full";
            }
        }
    }


    void BAMSortMkdupImpl::OutputBAM() {
        std::unique_ptr<BAMBlock> bam_block;
        int current_sharding_idx = 0;
        std::priority_queue<int> finish_sharding_idxs;
        while(output_bam_channel_.read(bam_block)) {
            auto sharding_idx = bam_block->sharding_idx();
            bool eof = bam_block->eof();
            chunks_[sharding_idx].push_back(std::move(bam_block));
            if (eof) {
                finish_sharding_idxs.push(sharding_idx);
                while (current_sharding_idx == finish_sharding_idxs.top()) {
                    if (!chunks_[current_sharding_idx].empty()) {
                        for (auto &block: chunks_[current_sharding_idx]) {
                            auto &slices = block->slices();
                            for (auto it = slices.begin(); it != slices.end(); ++it) {
                                int block_len = it->size();
                                int ok = (bgzf_flush_try(bam_file_->fp.bgzf, it->size() + 4) >= 0);
                                if (ok) {
                                    ok = bgzf_write(bam_file_->fp.bgzf, (char *)&block_len  ,4); // write bam length
                                }
                                if (ok) {
                                    ok = bgzf_write(bam_file_->fp.bgzf, block->data() + 4, 32); // write bam core data 32B
                                }
                                if (ok) {
                                    // write bam data variant length
                                    ok = bgzf_write(bam_file_->fp.bgzf, block->data() + 36, block_len - 32);
                                }
                            }
                        }
                    }
                    current_sharding_idx++;
                    finish_sharding_idxs.pop();
                }
            }
        }
        assert(finish_sharding_idxs.empty());
        if (output_bam_channel_.eof()) {
            bgzf_close(bam_file_->fp.bgzf);
        }
    }
}