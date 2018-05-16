//
// Created by ruoshui on 5/9/18.
//

#include <lib/htslib-1.3.1/htslib/sam.h>
#include <fstream>
#include <cassert>
#include <cstring>
#include <algorithm>
#include <snappy.h>
#include "mark_duplicate_frag_end.h"
#include "gam_mark_duplicate_impl.h"


namespace gamtools {

    MarkDuplicateFragEnd::MarkDuplicateFragEnd(const bam_hdr_t *bam_hdr, const std::string &filename)
            : filename_(filename),
              append_(false) {
        pair_flags_.resize(bam_hdr->n_targets);
        for (int i = 0; i < bam_hdr->n_targets; ++i) {
            pair_flags_[i].resize(bam_hdr->target_len[i], 0);
        }
        buffer_ = new char[kBufferLen];
        compress_buffer_ = new char[kBufferLen];
        kFragEndsNum = kBufferLen / sizeof(MarkDuplicateFragEnds);
    }

    MarkDuplicateFragEnd::~MarkDuplicateFragEnd() {
        delete [] buffer_;
        delete [] compress_buffer_;
    }

    void MarkDuplicateFragEnd::AddFragEnd(const std::shared_ptr<MarkDuplicateFragEnds> &frag_end) {
        frag_ends_.push_back(frag_end);
        if (frag_ends_.size() == kFragEndsNum) {
            StoreFileFragEnds();
        }
    }

    void MarkDuplicateFragEnd::AddPairFlag(const int tid, const int pos) {
        pair_flags_[tid][pos] = true;
    }

    void MarkDuplicateFragEnd::ProcessMarkDuplicateFragEnd() {
        //Read Mark duplicate info
        if (append_) {
            std::string frag_file = filename_ + "markdup.frag";
            std::ifstream if_frag(frag_file, std::ifstream::in | std::ifstream::binary);
            size_t compress_len;
            size_t uncompress_len;
            while (if_frag.read((char *) &compress_len, sizeof(size_t))) {
                if_frag.read(compress_buffer_, compress_len);
//                SnappyBlockCodecs::Decompress(compress_buffer_, compress_len, buffer_, &uncompress_len);
                if( snappy::GetUncompressedLength(compress_buffer_, compress_len, &uncompress_len) ) {
                    assert( snappy::RawUncompress(compress_buffer_, compress_len, buffer_));
                }
                const char *buffer_ptr = buffer_;
                while (buffer_ptr - buffer_ < uncompress_len) {
                    std::shared_ptr<MarkDuplicateFragEnds> frag_end(new MarkDuplicateFragEnds);
//                    ReadMarkDupFragEnds(buffer_ptr, frag_end);
                    MarkDuplicateFragEnds freg_end;
                    ReadMarkDupFragEnds(buffer_ptr, freg_end);
                    buffer_ptr += kFragEndLen;
                    frag_ends_.push_back(frag_end);
                }
            }
            if_frag.close();
        }
        ComputeMarkDuplicateFragEnds();
    }

    void MarkDuplicateFragEnd::StoreFileFragEnds() {
        std::string filename = filename_ + "markdup.frag";
        std::ofstream ofs;
        if (append_) {
            ofs.open(filename, std::ofstream::binary | std::ofstream::out | std::ofstream::app);
        } else {
            ofs.open(filename, std::ofstream::binary | std::ofstream::out);
            append_ = true;
        }
        assert(ofs.is_open());
        assert(frag_ends_.size() == kFragEndsNum);
        char data[kFragEndLen];
        int *w32 = (int *) (data);
        int64_t *w64 = (int64_t *) (data + 8);
        char *buffer_ptr = buffer_;
        for (auto &frag_end : frag_ends_) {
            w32[0] = frag_end->read1_tid_ << 24 | frag_end->lib_id_ << 17 | frag_end->orientation_ << 16 | frag_end->score_;
            w32[1] = frag_end->read1_pos_;
            w64[0] = frag_end->name_id_;
            memcpy(buffer_ptr, data, kFragEndLen);
            buffer_ptr += kFragEndLen;
        }
        size_t compress_len;
        snappy::RawCompress(buffer_, buffer_ptr - buffer_, compress_buffer_, &compress_len);
        ofs.write((char *) &compress_len, sizeof(size_t));
        ofs.write(compress_buffer_, compress_len);
        frag_ends_.clear();
        ofs.close();
    }


    void MarkDuplicateFragEnd::ComputeMarkDuplicateFragEnds() {
        std::sort(frag_ends_.begin(), frag_ends_.end(), MarkDuplicateFragEnds::ReadEndsComparator);
        MarkDuplicateFragEnds* next_chunk_first = nullptr;
        std::vector<MarkDuplicateFragEnds*> next_chunk;
        bool contains_pairs = false;
//        bool contains_frags = false;
        for (auto &frag_end : frag_ends_) {
            MarkDuplicateFragEnds *next = &(*frag_end);
            if (next_chunk_first != nullptr && AreComparableForDuplicatesFragEnds(*next_chunk_first, *next)) {
                next_chunk.push_back(next);
//                contains_pairs = contains_pairs || next->isPaired();
//                contains_frags = contains_frags || !next->isPaired();
            } else {
                if (!next_chunk.empty()) {
                    ComputeMarkDuplicateFragments(next_chunk, contains_pairs);
                }
                next_chunk.clear();
                next_chunk.push_back(next);
                next_chunk_first = next;
                contains_pairs = pair_flags_[next->read1_tid_][next->read1_pos_];
//                contains_frags = pair_flags_[next->read1_tid_][next->read1_pos_];
            }
        }
        ComputeMarkDuplicateFragments(next_chunk, contains_pairs);
    }

    bool MarkDuplicateFragEnd::AreComparableForDuplicatesFragEnds(const MarkDuplicateFragEnds &lhs,
                                            const MarkDuplicateFragEnds &rhs) {
        bool comparable = lhs.lib_id_ == rhs.lib_id_;
        if (comparable) {
        comparable = lhs.read1_tid_ == rhs.read1_tid_
                     && lhs.read1_pos_ == rhs.read1_pos_
                     && lhs.orientation_ == rhs.orientation_;
        }
        return comparable;
    }

    void
    MarkDuplicateFragEnd::ComputeMarkDuplicateFragments(std::vector<MarkDuplicateFragEnds *> &next_chunk,
                                                       const bool contains_pairs) {
        if (contains_pairs) {
            for (auto &end : next_chunk) {
                GAMMarkDuplicateImpl::SetMarkDuplicateReadNameID(end->name_id_);
            }
        } else {
            int16_t max_score = 0;
            MarkDuplicateFragEnds * best = nullptr;
            for (auto &end : next_chunk) {
                if (end->score_ > max_score || best == nullptr) {
                    max_score = end->score_;
                    best = end;
                }
            }
            for (auto &end : next_chunk) {
                if (end != best) {
                    GAMMarkDuplicateImpl::SetMarkDuplicateReadNameID(end->name_id_);
                }
            }
        }
    }

    void MarkDuplicateFragEnd::ReadMarkDupFragEnds(const char *p, MarkDuplicateFragEnds &frag_end) {
        int32_t *w32 = (int32_t *)p;
        frag_end.read1_tid_ =(int8_t) (w32[0] >> 24) & 0xff;
        frag_end.lib_id_    = (int8_t )(w32[0]>>17) & 0x7f;
        frag_end.orientation_ = (int8_t )(w32[0]>>16 )& 0x1;
        frag_end.score_       = (int16_t) w32[0] & 0xffff;
        frag_end.read1_pos_ = w32[1];
        frag_end.name_id_ = *(int64_t *)(p + 16);
    }
}