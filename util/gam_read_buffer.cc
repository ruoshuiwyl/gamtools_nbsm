//
// Created by ruoshui on 5/7/18.
//

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
    }

    BWAReadBuffer::~BWAReadBuffer() {
        free(seqs);
    }


    bool BWAReadBuffer::AddPairEndRead(GAMRead *read1, GAMRead *read2) {
        if (size < batch_size) {
            seqs[size++] = *read1;
            seqs[size++] = *read2;
            return true;
        } else {
            return false;
        }

    }
}