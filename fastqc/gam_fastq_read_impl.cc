//
// Created by ruoshui on 5/10/18.
//

#include "gam_fastq_read_impl.h"

#include "util/gam_read_buffer.h"
#include "gam_fastq_file.h"

namespace gamtools {
    GAMFastqReadImpl::GAMFastqReadImpl(std::vector<gamtools::GAMFastqFileInfo> &fastq_file_lists,
                                       BlockQueue <std::unique_ptr<GAMReadBuffer>> &output_fastq_channel,
                                       const int batch_size)
            : output_fastq_channel_(output_fastq_channel), fastq_batch_size_(batch_size) {
        for (auto &fastq_file :fastq_file_lists ) {
            std::unique_ptr<GAMFastqFile> fastq( new GAMFastqFile(fastq_file));
            fastq_file_lists_.push_back(std::move(fastq));
        }
    }

    void GAMFastqReadImpl::ReadFastQ() {
        for (auto &fast_file : fastq_file_lists_) {
            std::unique_ptr<GAMReadBuffer> read_buffer = fast_file->ReadFastq(fastq_batch_size_);
            while ( read_buffer != nullptr) {
                output_fastq_channel_.write(std::move(read_buffer));
                read_buffer = fast_file->ReadFastq(fastq_batch_size_);
            }
        }
        output_fastq_channel_.SendEof();
    }

    std::thread GAMFastqReadImpl::spawn() {
        return std::thread(&GAMFastqReadImpl::ReadFastQ, this);
    }
}