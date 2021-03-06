//
// Created by ruoshui on 5/7/18.
//

#include <assert.h>
#include "gam_read_buffer.h"

namespace gamtools {
    GAMReadBuffer::GAMReadBuffer(int lane_id) : size_(0), lane_id_(lane_id) {

    }

    void GAMReadBuffer::AddPERead(GAMRead *read1, GAMRead *read2) {
        reads1_.push_back(read1);
        reads2_.push_back(read2);
        size_++;
    }

    GAMRead* GAMReadBuffer::read1(int index) {
        return reads1_[index];
    }
    GAMRead* GAMReadBuffer::read2(int index) {
        return reads2_[index];
    }

    void GAMReadBuffer::AddFastqInfo(gamtools::BaseFqInfo &fq_info1, gamtools::BaseFqInfo &fq_info2) {
        fq_info1_.Add(fq_info1);
        fq_info2_.Add(fq_info2);
    }

    GAMReadBuffer::~GAMReadBuffer() {

    }


    BWAReadBuffer::BWAReadBuffer(int batch) : size(0), batch_size(batch) {
        seqs = (GAMRead *) malloc(batch_size * sizeof(GAMRead) );
        assert (seqs);

    }

    BWAReadBuffer::~BWAReadBuffer() {
        for (int i = 0; i < size; ++i) {
            free(seqs[i].name);
            free(seqs[i].seq);
            free(seqs[i].qual);
//            free(seqs[i].dup);
            free(seqs[i].bam);
        }
        free(seqs);
    }


    bool BWAReadBuffer::AddPairEndRead(GAMRead *read1, GAMRead *read2) {
        if (size < batch_size) {
            GAMReadCopy(&seqs[size++], read1);
            GAMReadCopy(&seqs[size++], read2);
//            seqs[size++] = *read1;
//            seqs[size++] = *read2;
            return true;
        } else {
            return false;
        }

    }

    bool BWAReadBuffer::empty() {
        return size == 0;
    }

    void BWAReadBuffer::GAMReadCopy(GAMRead *dst, const GAMRead *src) {
        dst->is_clean = src->is_clean;
        dst->l_seq = src->l_seq;    // read_len
        dst->bam_num = src->bam_num;  // bam number
        dst->lane_id = src->lane_id;  // lane_id
        dst->lib_id = src->lib_id;   //
        dst->id  = src->id;
        dst->read_id = src->read_id;
        dst->name = src->name;
        dst->comment = src->comment;
        dst->seq = src->seq;
        dst->qual = src->qual;
        dst->bam = src->bam;
//        char *dup;
        dst->dup = src->dup;
    }
}