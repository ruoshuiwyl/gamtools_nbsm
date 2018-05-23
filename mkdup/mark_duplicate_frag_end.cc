//
// Created by ruoshui on 5/9/18.
//

#include <lib/htslib-1.3.1/htslib/sam.h>
#include <fstream>
#include <cassert>
#include <cstring>
#include <algorithm>
#include <snappy.h>
#include <bwa_mem/gam_read.h>
#include <util/glogger.h>
#include "mark_duplicate_frag_end.h"
#include "gam_mark_duplicate_impl.h"


namespace gamtools {

    MarkDuplicateFragEnd::MarkDuplicateFragEnd(const bam_hdr_t *bam_hdr, const PartitionData &partition_data)
            : partition_data_(partition_data),
              append_(false) {
        pair_flags_.resize(bam_hdr->n_targets);
        negative_pair_flags_ .resize(bam_hdr->n_targets);
        for (int i = 0; i < bam_hdr->n_targets; ++i) {
            pair_flags_[i].resize(bam_hdr->target_len[i] + 256, 0);
            negative_pair_flags_[i].resize(1024, 0);
        }
        buffer_ = new char[kBufferLen];
        compress_buffer_ = new char[kBufferLen];
        kFragEndsNum = kBufferLen / sizeof(MarkDuplicateFragEnds);
    }

    MarkDuplicateFragEnd::~MarkDuplicateFragEnd() {
        delete [] buffer_;
        delete [] compress_buffer_;
    }

    void MarkDuplicateFragEnd::AddFragEnd(const read_end_t *frag_end) {
        frag_ends_.push_back(frag_end);
        if (frag_ends_.size() == kFragEndsNum) {
            StoreFileFragEnds();
        }
    }

    void MarkDuplicateFragEnd::AddPairFlag(const int tid, const int pos) {
        if (pos >= 0) {
            assert(tid < pair_flags_.size() && pos < pair_flags_[tid].size());
            pair_flags_[tid][pos] = true;
        } else {
            assert(tid < pair_flags_.size() && -pos < pair_flags_[tid].size());
            negative_pair_flags_[tid][-pos] = true;
        }
    }
    std::thread MarkDuplicateFragEnd::spawn() {{
        return std::thread(&MarkDuplicateFragEnd::ProcessMarkDuplicateFragEnd, this);
    }}

    void MarkDuplicateFragEnd::ProcessMarkDuplicateFragEnd() {
        //Read Mark duplicate info
        GLOG_INFO << "Start process mark duplicate sing end mapped";
        if (append_) {
            std::string frag_file = partition_data_.filename();
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
//                    std::shared_ptr<MarkDuplicateFragEnds> frag_end(new MarkDuplicateFragEnds);
//                    ReadMarkDupFragEnds(buffer_ptr, frag_end);
//                    MarkDuplicateFragEnds freg_end;
                    read_end_t * frag_end = ReadMarkDupFragEnds(buffer_ptr);
                    buffer_ptr += kFragEndLen;
                    frag_ends_.push_back(frag_end);
                }
            }
            if_frag.close();
        }
        ComputeMarkDuplicateFragEnds();
        GLOG_INFO << "Finish process mark duplicate sing end mapped";
    }

    void MarkDuplicateFragEnd::StoreFileFragEnds() {
        std::string filename = partition_data_.filename();
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
            w32[0] = frag_end->tid << 24 | frag_end->lib_id << 17 | frag_end->orientation << 16 | frag_end->score;
            w32[1] = frag_end->pos;
            w64[0] = frag_end->read_id;
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
        std::sort(frag_ends_.begin(), frag_ends_.end(), FragReadEndsComparator);
        const read_end_t* next_chunk_first = nullptr;
        std::vector<const read_end_t*> next_chunk;
        bool contains_pairs = false;
//        bool contains_frags = false;
        for (auto &frag_end : frag_ends_) {
            const read_end_t *next = &(*frag_end);
            if (next_chunk_first != nullptr && AreComparableForDuplicatesFragEnds(next_chunk_first, next)) {
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
                if (next->pos >= 0) {
                    contains_pairs = pair_flags_[next->tid][next->pos];
                } else {
                    contains_pairs = negative_pair_flags_[next->tid][-next->pos];
                }
//                contains_frags = pair_flags_[next->read1_tid_][next->read1_pos_];
            }
        }
        ComputeMarkDuplicateFragments(next_chunk, contains_pairs);
    }

    bool MarkDuplicateFragEnd::AreComparableForDuplicatesFragEnds(const read_end_t *lhs,
                                                                  const read_end_t *rhs) {
        bool comparable = lhs->lib_id == rhs->lib_id;
        if (comparable) {
        comparable = lhs->tid == rhs->tid
                     && lhs->pos == rhs->pos
                     && lhs->orientation == rhs->orientation;
        }
        return comparable;
    }

    void
    MarkDuplicateFragEnd::ComputeMarkDuplicateFragments(std::vector<const read_end_t *> &next_chunk,
                                                        const bool contains_pairs) {
        if (contains_pairs) {
            for (auto &end : next_chunk) {
                GAMMarkDuplicateImpl::SetMarkDuplicateReadNameID(end->read_id);
            }
        } else {
            int16_t max_score = 0;
            const read_end_t * best = nullptr;
            for (auto end : next_chunk) {
                if (end->score > max_score || best == nullptr) {
                    max_score = end->score;
                    best = end;
                }
            }
            for (auto &end : next_chunk) {
                if (end != best) {
                    GAMMarkDuplicateImpl::SetMarkDuplicateReadNameID(end->read_id);
                }
            }
        }
    }

    read_end_t * MarkDuplicateFragEnd::ReadMarkDupFragEnds(const char *p) {
        int32_t *w32 = (int32_t *)p;
        read_end_t *frag_end = (read_end_t *) malloc(sizeof(read_end_t));
        frag_end->tid =(int8_t) (w32[0] >> 24) & 0xff;
        frag_end->lib_id    = (int8_t )(w32[0]>>17) & 0x7f;
        frag_end->orientation = (int8_t )(w32[0]>>16 )& 0x1;
        frag_end->score       = (int16_t) w32[0] & 0xffff;
        frag_end->pos = w32[1];
        frag_end->read_id = *(int64_t *)(p + 16);
    }
}