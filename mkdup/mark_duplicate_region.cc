//
// Created by ruoshui on 3/14/17.
//

#include <cassert>
#include <bits/ios_base.h>
#include <fstream>
#include <algorithm>
#include <memory.h>
#include <snappy.h>
#include <util/glogger.h>
#include "mark_duplicate_region.h"
#include "gam_mark_duplicate_impl.h"
#include "util/create_index.h"

namespace gamtools {
    MarkDuplicateRegion::MarkDuplicateRegion(const PartitionData &partition_data) : partition_data_(partition_data) {
        buffer_ = new char[kBufferLen];
        compress_buffer_ = new char[kBufferLen];
        kPairEndsNum = kBufferLen / kPairEndsLen;
    }

    MarkDuplicateRegion::~MarkDuplicateRegion() {
        delete[] buffer_;
        delete[] compress_buffer_;
    }

    /*
    void MarkDuplicateRegion::AddFragEnd(const std::shared_ptr<MarkDuplicateFragEnds> &frag_end) {
        frag_ends_.push_back(std::move(frag_end));
        if (frag_ends_.size() == kFragEndsNum) {
#ifdef GAMPROF
            GAMProfileCPUTimer gam_profile_timer;
            StoreFileFragEnds();
            auto markdup_output_cpu_time = gam_profile_timer.ThreadElapsed();
            auto markdup_output_real_time = gam_profile_timer.RealElapsed();
            //GAMStatisticsTimer::sMarkDupOutputCpuTime.fetch_add(markdup_output_cpu_time);
            //GAMStatisticsTimer::sMarkDupOutputRealTime.fetch_add(markdup_output_real_time);
#else
            StoreFileFragEnds();
#endif
        }

    }
     */

    void MarkDuplicateRegion::AddPairEnd(const std::shared_ptr<MarkDuplicatePairEnds> &pair_end) {
        assert(pair_end->read1_tid_ < pair_end->read2_tid_ ||
               (pair_end->read1_tid_ == pair_end->read2_tid_ &&
                pair_end->read1_pos_ <= pair_end->read2_pos_));
        pair_ends_.push_back(std::move(pair_end));
        if (pair_ends_.size() == kPairEndsNum) {
#ifdef GAMPROF
            GAMProfileCPUTimer gam_profile_timer;
            StoreFilePairEnds();
            auto markdup_output_cpu_time = gam_profile_timer.ThreadElapsed();
            auto markdup_output_real_time = gam_profile_timer.RealElapsed();
            //GAMStatisticsTimer::sMarkDupOutputCpuTime.fetch_add(markdup_output_cpu_time);
            //GAMStatisticsTimer::sMarkDupOutputRealTime.fetch_add(markdup_output_real_time);
#else
            StoreFilePairEnds();
#endif
        }
    }



/*
    void MarkDuplicateRegion::ComputeMarkDuplicateFragEnds() {
        std::sort(frag_ends_.begin(), frag_ends_.end(), MarkDuplicateFragEnds::ReadEndsComparator);
        std::shared_ptr<MarkDuplicateFragEnds> next_chunk_first = nullptr;
        std::vector<std::shared_ptr<MarkDuplicateFragEnds>> next_chunk;
        bool contains_pairs = false;
        bool contains_frags = false;
        for (auto next : frag_ends_) {
            if (next_chunk_first != nullptr && AreComparableForDuplicatesFragEnds(*next_chunk_first, *next)) {
                next_chunk.emplace_back(next);
                contains_pairs = contains_pairs || next->isPaired();
                contains_frags = contains_frags || !next->isPaired();
            } else {
                if (next_chunk.size() > 1 && contains_frags) {
                    ComputeMarkDuplicateFragments(next_chunk, contains_pairs);
                }
                next_chunk.clear();
                next_chunk.emplace_back(next);
                next_chunk_first = next;
                contains_pairs = next->isPaired();
                contains_frags = !next->isPaired();
            }
        }
        ComputeMarkDuplicateFragments(next_chunk, contains_pairs);
    }
    */

    void MarkDuplicateRegion::ComputeMarkDuplicatePairEnds() {
        std::sort(pair_ends_.begin(), pair_ends_.end(), MarkDuplicatePairEnds::ReadEndsComparator);
        std::shared_ptr<MarkDuplicatePairEnds> next_chunk_first = nullptr;
        std::vector<std::shared_ptr<MarkDuplicatePairEnds>> next_chunk;
        for (auto &next : pair_ends_) {
            if (next_chunk_first != nullptr
                && AreComparableForDuplicatesPairEnds(*next_chunk_first, *next)) {
                next_chunk.emplace_back(next);
            } else {
                if (next_chunk.size() > 1) {
                    ComputeMarkDuplicatePairEnds(next_chunk);
                }
                next_chunk.clear();
                next_chunk.emplace_back(next);
                next_chunk_first = next;
            }
        }
        if (next_chunk.size() > 1) {
            ComputeMarkDuplicatePairEnds(next_chunk);
        }

    }

    std::thread MarkDuplicateRegion::spawn() {
        return std::thread(&MarkDuplicateRegion::ProcessMarkDuplicate, this);
    }
    void MarkDuplicateRegion::ProcessMarkDuplicate() {
//        std::string file_name = FileName();
        std::string pair_file = partition_data_.filename();

//        GAMProfileCPUTimer gam_profile_timer;
        if (pair_file_append_flag) {
            std::ifstream if_pair(pair_file, std::ifstream::in | std::ifstream::binary);
            if (if_pair.is_open()) {
                GLOG_INFO << "Open file :" << pair_file ;
            } else {
                GLOG_ERROR << "Open file :" << pair_file;
            }
            size_t compress_len;
            size_t uncompress_len;
            while (if_pair.read((char *) &compress_len, sizeof(size_t))) {
                if_pair.read(compress_buffer_, compress_len);
//                SnappyBlockCodecs::Decompress(compress_buffer_, compress_len, buffer_, &uncompress_len);
                if( snappy::GetUncompressedLength(compress_buffer_, compress_len, &uncompress_len) ) {
                    assert( snappy::RawUncompress(compress_buffer_, compress_len, buffer_));
                }
                const char *buffer_ptr = buffer_;
                while (buffer_ptr - buffer_ < uncompress_len) {
                    std::shared_ptr<MarkDuplicatePairEnds> pair_end(new MarkDuplicatePairEnds);
                    ReadMarkDupPairEnds(buffer_ptr, pair_end);
                    buffer_ptr += kPairEndsLen;
                    pair_ends_.emplace_back(std::move(pair_end));
                }
            }
            if_pair.close();
        }
#ifdef GAMPROF
        auto gam_input_cpu_time = gam_profile_timer.ThreadElapsed();
        auto gam_input_real_time = gam_profile_timer.RealElapsed();
        //GAMStatisticsTimer::sMarkDupInputCpuTime.fetch_add(gam_input_cpu_time);
        //GAMStatisticsTimer::sMarkDupInputRealTime.fetch_add(gam_input_real_time);
        gam_profile_timer.ResetRealTime();
        gam_profile_timer.ResetThreadCPUTime();
        ComputeMarkDuplicatePairEnds();
        auto markdup_compute_cpu_time = gam_profile_timer.ThreadElapsed();
        auto markdup_compute_real_time = gam_profile_timer.RealElapsed();
        //GAMStatisticsTimer::sMarkDupComputeCpuTime.fetch_add(markdup_compute_cpu_time);
        //GAMStatisticsTimer::sMarkDupComputeRealTime.fetch_add(markdup_compute_real_time);
        gam_profile_timer.ResetRealTime();
        gam_profile_timer.ResetThreadCPUTime();
#else
        ComputeMarkDuplicatePairEnds();
#endif
        pair_ends_.clear();
        pair_ends_.shrink_to_fit();
        /*
        std::string frag_file = file_name + "frag";
        if (frag_file_append_flag) {
            std::ifstream if_frag(frag_file, std::ifstream::in | std::ifstream::binary);
            size_t compress_len;
            size_t uncompress_len;
            while (if_frag.read((char *) &compress_len, sizeof(size_t))) {
                if_frag.read(compress_buffer_, compress_len);
//                SnappyBlockCodecs::Decompress(compress_buffer_, compress_len, buffer_, &uncompress_len);
                const char *buffer_ptr = buffer_;
                while (buffer_ptr - buffer_ < uncompress_len) {
                    std::shared_ptr<MarkDuplicateFragEnds> frag_end(new MarkDuplicateFragEnds);
                    ReadMarkDupFragEnds(buffer_ptr, frag_end);
                    buffer_ptr += kFragEndsLen;
                    frag_ends_.push_back(std::move(frag_end));
                }
            }
            if_frag.close();
        }
#ifdef GAMPROF
        gam_input_cpu_time = gam_profile_timer.ThreadElapsed();
        gam_input_real_time = gam_profile_timer.RealElapsed();
        //GAMStatisticsTimer::sMarkDupInputCpuTime.fetch_add(gam_input_cpu_time);
        //GAMStatisticsTimer::sMarkDupInputRealTime.fetch_add(gam_input_real_time);
        gam_profile_timer.ResetRealTime();
        gam_profile_timer.ResetThreadCPUTime();
        ComputeMarkDuplicateFragEnds();
        markdup_compute_cpu_time = gam_profile_timer.ThreadElapsed();
        markdup_compute_real_time = gam_profile_timer.RealElapsed();
        //GAMStatisticsTimer::sMarkDupComputeCpuTime.fetch_add(markdup_compute_cpu_time);
        //GAMStatisticsTimer::sMarkDupComputeRealTime.fetch_add(markdup_compute_real_time);
#else
        ComputeMarkDuplicateFragEnds();
#endif
        frag_ends_.clear();
        frag_ends_.shrink_to_fit();
         */
    }
/*
    void MarkDuplicateRegion::ReadMarkDupFragEnds(const char *data, std::shared_ptr<MarkDuplicateFragEnds> &frag_end) {
        const int8_t *w8 = (int8_t *) data;
        const int *w32 = (int *) (data + 4);
        const int64_t *w64 = (int64_t *) (data + 12);
        frag_end->read1_tid_ = w8[0];
        frag_end->read2_tid_ = w8[1];
        frag_end->lib_id_ = w8[2];
        frag_end->orientation_ = w8[3];
        frag_end->score_ = w32[0];
        frag_end->read1_pos_ = w32[1];
        frag_end->name_id_ = w64[0];
//        frag_end->read1_idx_.read_sort_idx_ = w64[1];
//        frag_end->read1_idx_.read_input_idx_ = w64[2];
    }
*/
    void MarkDuplicateRegion::ReadMarkDupPairEnds(const char *data, std::shared_ptr<MarkDuplicatePairEnds> &pair_end) {
        const int8_t *w8 = (int8_t *) data;
        const int32_t *w32 = (int32_t *) (data + 4);
        const int64_t *w64 = (int64_t *) (data + 16);
        pair_end->read1_tid_ = w8[0];
        pair_end->read2_tid_ = w8[1];
        pair_end->orientation_ = w8[2];
        pair_end->lib_id_ = w8[3];
        pair_end->score_ = w32[0];
        pair_end->read1_pos_ = w32[1];
        pair_end->read2_pos_ = w32[2];
        pair_end->name_id_ = w64[0];
    }



    void MarkDuplicateRegion::StoreFilePairEnds() {
        std::string filename = partition_data_.filename();
//        filename += "pair";
        std::ofstream ofs;
        if (pair_file_append_flag) {
            ofs.open(filename, std::ofstream::binary | std::ofstream::out | std::ofstream::app);
        } else {
            ofs.open(filename, std::ofstream::binary | std::ofstream::out);
            pair_file_append_flag = true;
        }
        assert(ofs.is_open());
        assert(pair_ends_.size() == kPairEndsNum);
        char data[kPairEndsLen];
        int8_t *w8 = (int8_t *) data;
        int32_t *w32 = (int32_t *) (data + 4);
        int64_t *w64 = (int64_t *) (data + 16);
        char *buffer_ptr = buffer_;
        for (auto &pair_end : pair_ends_) {
            w8[0] = pair_end->read1_tid_;
            w8[1] = pair_end->read2_tid_;
            w8[2] = pair_end->orientation_;
            w8[3] = pair_end->lib_id_;
            w32[0] = pair_end->score_;
            w32[1] = pair_end->read1_pos_;
            w32[2] = pair_end->read2_pos_;
            w64[0] = pair_end->name_id_;
            mempcpy(buffer_ptr, data, kPairEndsLen);
            buffer_ptr += kPairEndsLen;
        }
        pair_ends_.clear();
        size_t compress_len;
        snappy::RawCompress(buffer_, buffer_ptr - buffer_, compress_buffer_, &compress_len);
        ofs.write((char *) &compress_len, sizeof(size_t));
        ofs.write(compress_buffer_, compress_len);
        ofs.close();
    }

//
//    bool MarkDuplicateRegion::AreComparableForDuplicatesFragEnds(const MarkDuplicateFragEnds &lhs,
//                                                                 const MarkDuplicateFragEnds &rhs) {
//        bool comparable = lhs.lib_id_ == rhs.lib_id_;
//        if (comparable) {
//            comparable = lhs.read1_tid_ == rhs.read1_tid_
//                         && lhs.read1_pos_ == rhs.read1_pos_
//                         && lhs.orientation_ == rhs.orientation_;
//        }
//        return comparable;
//    }

    bool MarkDuplicateRegion::AreComparableForDuplicatesPairEnds(const MarkDuplicatePairEnds &lhs,
                                                                 const MarkDuplicatePairEnds &rhs) {
        bool comparable = lhs.lib_id_ == rhs.lib_id_;
        if (comparable) {
            comparable = lhs.read1_tid_ == rhs.read1_tid_
                         && lhs.read1_pos_ == rhs.read1_pos_
                         && lhs.orientation_ == rhs.orientation_;
        }
        if (comparable) {
            comparable = lhs.read2_tid_ == rhs.read2_tid_
                         && lhs.read2_pos_ == rhs.read2_pos_;
        }
        return comparable;
    }

    void
    MarkDuplicateRegion::ComputeMarkDuplicatePairEnds(std::vector<std::shared_ptr<MarkDuplicatePairEnds>> &next_chunk) {
        int16_t max_score = 0;
        std::shared_ptr<MarkDuplicatePairEnds> best = nullptr;
        for (auto &next : next_chunk) {
            if (best == nullptr || next->score_ > max_score) {
                max_score = next->score_;
                best = next;
            }
        }
        for (auto &end: next_chunk) {
            if (end != best) {
                GAMMarkDuplicateImpl::SetMarkDuplicateReadNameID(end->name_id_);
            }
        }

    }
//
//    void
//    MarkDuplicateRegion::ComputeMarkDuplicateFragments(std::vector<std::shared_ptr<MarkDuplicateFragEnds>> &next_chunk,
//                                                       const bool contains_pairs) {
//        if (contains_pairs) {
//            for (auto &end : next_chunk) {
//                if (!end->isPaired()) {
//                    GAMMarkDuplicateImpl::SetMarkDuplicateReadNameID(end->name_id_);
//                }
//            }
//        } else {
//            int16_t max_score = 0;
//            std::shared_ptr<MarkDuplicateFragEnds> best = nullptr;
//            for (auto &end : next_chunk) {
//                if (end->score_ > max_score || best == nullptr) {
//                    max_score = end->score_;
//                    best = end;
//                }
//            }
//            for (auto &end : next_chunk) {
//                if (end != best) {
//                    GAMMarkDuplicateImpl::SetMarkDuplicateReadNameID(end->name_id_);
//                }
//            }
//        }
//    }
}