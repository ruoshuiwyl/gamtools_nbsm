//
// Created by ruoshui on 4/24/18.
//


#include <lib/htslib-1.3.1/htslib/hts.h>
#include <cassert>
#include "bam_sharding_impl.h"
#include "util/slice.h"
#include "util/create_index.h"
#include "bam_partition_data.h"
#include "gam_block.h"

namespace gamtools {

    BAMShardingImpl::BAMShardingImpl(const bam_hdr_t *bam_hdr, const SMOptions &options)
            : bam_hdr_(bam_hdr),
              options_(options) {
    }

    void BAMShardingImpl::InitializeSharding() {
        sort_idx_ = std::unique_ptr<CreateIndex>( new CreateIndex(bam_hdr_, options_, IndexType::SortIndex));
        auto & parts = sort_idx_->partition_datas();
        for (auto &part : parts) {
            std::unique_ptr<BAMPartitionData> gam_part = std::unique_ptr<BAMPartitionData>( new BAMPartitionData(sort_channel_, part));
            partition_datas_.push_back(std::move(gam_part));
        }
        for (int i = 0; i < options_.block_sort_thread_num; ++i) {
            sort_compress_threads_.push_back(std::thread(&BAMShardingImpl::GAMBlockSortCompress, this));
        }
        output_gamblock_thread_ = std::thread(&BAMShardingImpl::OutputGAMBlock, this);

    }

    void BAMShardingImpl::InitializeMergeSort(std::string &bam_filename) {
        //
        sort_compress_threads_.clear();
        for (int i = 0; i < options_.block_sort_thread_num; ++i) {
            sort_compress_threads_.push_back(std::thread(&BAMShardingImpl::MergeSort, this));
        }
        output_gamblock_thread_ = std::thread(&BAMShardingImpl::OutputBAM, this);
        bam_file_ = hts_open_format(bam_filename.c_str(), "wb", nullptr);
        hts_set_threads(bam_file_, options_.bam_output_thread_num);
        bam_hdr_write(bam_file_->fp.bgzf, bam_hdr_);
    }

    void BAMShardingImpl::Sharding(const Slice &slice) {
        int64_t key = reinterpret_cast<const int64_t *> (slice.data())[0];
        int contig_id = key >> 32;
        int position_id = (key & 0xfffffffff) >> 20;
        auto sharding_idx = sort_idx_->sharding_index();
        if (key != INT64_MAX ) { // map;
            auto sharding_id = sharding_idx[contig_id][position_id];
            partition_datas_[sharding_id]->Add(slice);
        } else { //unmap
//            unmap_partition->AddFastqInfo(slice);
            auto sharding_id = partition_datas_.size() - 1;
            partition_datas_[sharding_id]->Add(slice);
        }
    }



    void BAMShardingImpl::OutputGAMBlock() {
        std::unique_ptr<GAMBlock> gbam_block;
        while (output_channel_.read(gbam_block)) {
            gbam_block->Write();
        }
        assert (output_channel_.eof());
    }

    void BAMShardingImpl::GAMBlockSortCompress() {
        std::unique_ptr<GAMBlock> gam_block;
        while(sort_channel_.read(gam_block)) {
            auto sort_block = gam_block->BlockSort();
            sort_block->Compress();
            output_channel_.write(std::move(sort_block));
        }
    }





    void BAMShardingImpl::FinishSharding() {
        for (auto &part : partition_datas_){
            part->sendEof();
        }
        for (auto &th : sort_compress_threads_) {
            th.join();
        }
        output_channel_.SendEof();
        sort_compress_threads_.clear();
        output_gamblock_thread_.join();
    }

    void BAMShardingImpl::ReadGamBlock() {
        //Read GAMBlock
        for (auto &part : partition_datas_) {
            part->ReadGAMBlock();
            merge_channel_.write(std::move(part));
        }
        merge_channel_.SendEof();
    }

    void BAMShardingImpl::MergeSort() {
        std::unique_ptr<BAMPartitionData> part;
        while (merge_channel_.read(part)) {
            part->MergeSort();
        }
    }

    void BAMShardingImpl::FinishMergeSort() {
        for (auto &th : sort_compress_threads_) {
            th.join();
        }
        output_channel_.SendEof();
        output_gamblock_thread_.join();
    }

    void BAMShardingImpl::OutputBAM() {
        std::unique_ptr<BAMBlock> bam_block;
        int current_sharding_idx = 0;
        std::priority_queue<int> finish_sharding_idxs;

        while(bam_channel_.read(bam_block)) {
            auto sharding_idx = bam_block->sharding_idx();
            chunks_[sharding_idx].push_back(std::move(bam_block));
            if (bam_block->eof()) {
                finish_sharding_idxs.push(sharding_idx);
                while (current_sharding_idx == finish_sharding_idxs.top()) {
                    if (chunks_[current_sharding_idx].empty()) {
                        for (auto &block: chunks_[current_sharding_idx]) {

                            bgzf_write(bam_file_->fp.bgzf, block->data(), block->size());
                        }
                    }
                    current_sharding_idx++;
                    finish_sharding_idxs.pop();
                }
            }
        }
        assert(finish_sharding_idxs.empty());
        if (bam_channel_.eof()) {
            bgzf_close(bam_file_->fp.bgzf);
        }

    }
}