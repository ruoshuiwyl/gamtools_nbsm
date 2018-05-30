//
// Created by ruoshui on 3/13/17.
//

#include <thread>
#include <fstream>
#include <cassert>
#include <util/create_index.h>
#include <bwa_mem/gam_read.h>
#include <util/glogger.h>

#include "util/boost/threadpool.hpp"
#include "gam_mark_duplicate_impl.h"
#include "mark_duplicate_frag_end.h"
//#include "nbsm/options.h"
//#include "util/create_index.h"

namespace gamtools {
    std::vector<bool> GAMMarkDuplicateImpl::mark_dup_readname_id_;
    uint64_t GAMMarkDuplicateImpl::total_read_name_ids_ = 0;

//GAMMarkDuplicateImpl::GAMMarkDuplicateImpl(const int chr_count,
//                                           const std::string &temp_output_path) : temp_mark_dup_path_(temp_output_path) {
//    for ( int chr_index = 0; chr_index < chr_count; ++chr_index) {
//        markdup_regions_.emplace_back(chr_index);
//    }
//}

    GAMMarkDuplicateImpl::GAMMarkDuplicateImpl(const bam_hdr_t *bam_hdr,
                                               const gamtools::SMOptions &options)
            :bam_hdr_(bam_hdr), options_(options), thread_num_(options.mark_dup_thread_num){

    }

    void GAMMarkDuplicateImpl::InitializeSharding() {
        markdup_idx_ = std::unique_ptr<CreateIndex>( new CreateIndex(bam_hdr_, options_, IndexType::MarkDupIndex));
        markdup_sharding_index_ = markdup_idx_->sharding_index();
        auto & parts = markdup_idx_->partition_datas();
        for (int part_idx = 0; part_idx < parts.size() - 1; ++part_idx) {
            auto mkdup_region = std::unique_ptr<MarkDuplicateRegion>(new MarkDuplicateRegion(parts[part_idx]));
            markdup_regions_.push_back(std::move(mkdup_region));
        }
        markdup_frag_end_ = std::unique_ptr<MarkDuplicateFragEnd> (new MarkDuplicateFragEnd(bam_hdr_, parts[parts.size() - 1]));
    }


    GAMMarkDuplicateImpl::~GAMMarkDuplicateImpl() {

    }

    std::shared_ptr<MarkDuplicateFragEnds> GAMMarkDuplicateImpl::ComputeFragEnds(const read_end_t *gbam) {
        std::shared_ptr<MarkDuplicateFragEnds> frag_ends(new MarkDuplicateFragEnds);
        frag_ends->read1_tid_ = gbam->tid;
        frag_ends->read1_pos_ = gbam->pos;
        frag_ends->lib_id_ = gbam->lib_id;
        frag_ends->orientation_ = gbam->orientation;


//        uint64_t *w = (uint64_t *) (gbam + 4);
////        frag_ends->read2_tid_ = -1;
//        frag_ends->read1_tid_ = (w[0] >> 8) & 0xff;
//        frag_ends->read1_pos_ = (w[0] >> 32) & 0xffffffff;
//        frag_ends->score_ = (w[0] >> 16) & 0xffff;
//        frag_ends->lib_id_ = (w[0] >> 1) & 0x7f;
//        frag_ends->orientation_ = w[0] & 0x1;
//        frag_ends->name_id_ = w[3];
        return std::move(frag_ends);
    }

    std::shared_ptr<MarkDuplicatePairEnds>
    GAMMarkDuplicateImpl::ComputePairEnds(const read_end_t *read1_frag_end,
                                          const read_end_t *read2_frag_end) {

        assert(read1_frag_end->lib_id == read2_frag_end->lib_id);
        std::shared_ptr<MarkDuplicatePairEnds> pair_read_ends(new MarkDuplicatePairEnds);
        pair_read_ends->lib_id_ = read1_frag_end->lib_id;
        pair_read_ends->name_id_ = read1_frag_end->read_id;
        if (read2_frag_end->tid > read1_frag_end->tid ||
            (read2_frag_end->tid == read1_frag_end->tid &&
             read2_frag_end->pos >= read1_frag_end->pos)) {
            pair_read_ends->read1_tid_ = read1_frag_end->tid;
            pair_read_ends->read1_pos_ = read1_frag_end->pos;
            pair_read_ends->read2_tid_ = read2_frag_end->tid;
            pair_read_ends->read2_pos_ = read2_frag_end->pos;
            pair_read_ends->orientation_ = ComputeOrientationByte(
                    read1_frag_end->orientation == GAMMarkDuplicateImpl::R,
                    read2_frag_end->orientation == GAMMarkDuplicateImpl::R);
            if (pair_read_ends->read1_tid_ == pair_read_ends->read2_tid_
                && pair_read_ends->read1_pos_ == pair_read_ends->read2_pos_
                && pair_read_ends->orientation_ == GAMMarkDuplicateImpl::RF) {
                pair_read_ends->orientation_ = GAMMarkDuplicateImpl::FR;
            }
            pair_read_ends->score_ = read1_frag_end->score + read2_frag_end->score;
        } else {
            pair_read_ends->read1_tid_ = read2_frag_end->tid;
            pair_read_ends->read1_pos_ = read2_frag_end->pos;
            pair_read_ends->read2_tid_ = read1_frag_end->tid;
            pair_read_ends->read2_pos_ = read1_frag_end->pos;
            pair_read_ends->orientation_ = ComputeOrientationByte(
                    read2_frag_end->orientation == GAMMarkDuplicateImpl::R,
                    read1_frag_end->orientation == GAMMarkDuplicateImpl::R);
            pair_read_ends->score_ = read1_frag_end->score + read2_frag_end->score;
        }
        return std::move(pair_read_ends);
    }

    const int8_t GAMMarkDuplicateImpl::ComputeOrientationByte(const bool read1_negative_strand,
                                                              const bool read2_nagative_strand) {
        if (read1_negative_strand) {
            if (read2_nagative_strand) {
                return GAMMarkDuplicateImpl::RR;
            } else {
                return GAMMarkDuplicateImpl::RF;
            }
        } else {
            if (read2_nagative_strand) {
                return GAMMarkDuplicateImpl::FR;
            } else {
                return GAMMarkDuplicateImpl::FF;
            }
        }
    }

    void GAMMarkDuplicateImpl::StorePairEndRecord(read_end_t *read1_dup, read_end_t *read2_dup) {
        if (read1_dup != nullptr && read2_dup != nullptr) {
            auto pair_end = ComputePairEnds(read1_dup, read2_dup);

            int pair_index = SeekMarkDupIndex(pair_end->read1_tid_, pair_end->read1_pos_);

#ifdef DEBUG
            GLOG_DEBUG << "sharind_idx:"<<  pair_index << "\tpair_end:" << (int)pair_end->read1_tid_ << ":" << pair_end->read1_pos_
                    << "\t read1_dup:" << (int)read1_dup->tid << ":" << read1_dup->pos << "\t read2_dup " << (int)read2_dup->tid
                    << ":" << read2_dup->pos;
#endif
            markdup_frag_end_->AddPairFlag(read1_dup->tid, read1_dup->pos);
            markdup_frag_end_->AddPairFlag(read2_dup->tid, read2_dup->pos);
            if (pair_index < markdup_regions_.size()) {
                markdup_regions_[pair_index]->AddPairEnd(pair_end);
            } else {
                GLOG_ERROR << __FUNCTION__ <<  "Mark duplicate stage pair_index out of markdup_regions_";
            }
            free(read1_dup);
            free(read2_dup);
        } else if (read1_dup != nullptr && read2_dup == nullptr) {
            markdup_frag_end_->AddFragEnd(read1_dup);
        } else if (read1_dup == nullptr && read2_dup != nullptr) {
            markdup_frag_end_->AddFragEnd(read2_dup);

        }
    }


    const bool GAMMarkDuplicateImpl::IsMarkDuplicate(const uint64_t read_name_id) {
        assert(read_name_id < mark_dup_readname_id_.size());
        return mark_dup_readname_id_[read_name_id];
    }

    void GAMMarkDuplicateImpl::MarkDuplication(const uint64_t total_read_name_id) {
        total_read_name_ids_ = total_read_name_id;
        assert(mark_dup_readname_id_.empty());
        mark_dup_readname_id_.resize(total_read_name_id, false);

        std::thread frag_mark_duplicate_thread = markdup_frag_end_->spawn();
        boost::threadpool::pool pl(thread_num_);
        for (int chr_index = 0; chr_index < markdup_regions_.size(); chr_index += thread_num_) {
//            std::vector<std::thread> threads;

            for (int i = chr_index; (i - chr_index < thread_num_) && (i < markdup_regions_.size()); ++i) {
//                threads.push_back(markdup_regions_[i]->spawn());
                pl.schedule([&, i]{
                    markdup_regions_[i]->ProcessMarkDuplicate();
                });
            }
            pl.wait();
//            for (auto &thread : threads) {
//                thread.join();
//            }
//            threads.clear();
        }
//        DebugOutput(true);
        pl.clear();
        frag_mark_duplicate_thread.join();
    }

    void GAMMarkDuplicateImpl::SetMarkDuplicateReadNameID(const uint64_t read_name_id) {
        assert(read_name_id <= total_read_name_ids_);
        mark_dup_readname_id_[read_name_id] = true;
    }

    void GAMMarkDuplicateImpl::DebugOutput(bool output) {
        if (output) {
            std::string file;
//            temp_mark_dup_path_ = GamtoolsGlobalParameter::MarkDupOutputPathname();
            if (temp_mark_dup_path_.back() == '/') {
                file = temp_mark_dup_path_ + "markdup.txt";
            } else {
                file = temp_mark_dup_path_ + "/markdup.txt";
            }
            std::ofstream markdup_file(file, std::ofstream::out);
            if (!markdup_file.is_open()) {
                return;
            }
            for (int i = 0; i < mark_dup_readname_id_.size(); ++i) {

                markdup_file << (mark_dup_readname_id_.at(i) ? '1' : '0');
            }
            markdup_file.close();
        }

    }

    void GAMMarkDuplicateImpl::DebugInput(const std::string &input_markdup_file) {
        std::ifstream markdup_file(input_markdup_file, std::ifstream::in);

        char id;
        while (markdup_file >> id) {
            if (id == '0') {
                mark_dup_readname_id_.push_back(false);
            }
            if (id == '1') {
                mark_dup_readname_id_.push_back(true);
            }
        }
        markdup_file.close();
    }

}