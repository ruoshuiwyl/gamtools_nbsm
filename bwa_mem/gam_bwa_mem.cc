//
// Created by ruoshui on 5/10/18.
//

#include <htslib/sam.h>
#include <cstring>
#include <util/glogger.h>
#include "gam_bwa_mem.h"
#include "bwa.h"
#include "bntseq.h"


namespace gamtools {
    void GAMBWAMEM::ProcessBWAMEM() {
        std::unique_ptr<BWAReadBuffer> read_buffer;
        while (input_queue_.read(read_buffer)) {
            mem_process_seqs(mem_opt_, mem_idx_->bwt, mem_idx_->bns, mem_idx_->pac, processed_num_, read_buffer->size, read_buffer->seqs,
                             mem_pestat_);
            processed_num_ += read_buffer->size;
            output_queue_.write(std::move(read_buffer));
            GLOG_TRACE << "Output GAMBlock Buffer Size: " << output_queue_.size();
        }
        if (input_queue_.eof()) {
            output_queue_.SendEof();
        }
    }

    GAMBWAMEM::GAMBWAMEM(BlockQueue <std::unique_ptr<BWAReadBuffer>> &input,
                         BlockQueue <std::unique_ptr<BWAReadBuffer>> &output,
                         const mem_opt_t *opt, bwaidx_t *mem_idx)
        : input_queue_(input),
          output_queue_(output),
          processed_num_(0),
          mem_opt_(opt),
          mem_idx_(mem_idx),
          mem_pestat_(nullptr) {
    }

    GAMBWAMEM::~GAMBWAMEM() {
    }
    std::thread GAMBWAMEM::spawn() {
        return std::thread(&GAMBWAMEM::ProcessBWAMEM, this);
    }
}

void gam_read_destory( GAMRead *read) {
    free(read->bam);
    free(read->name);
    free(read->seq);
    free(read->qual);
    free(read->dup);
    free(read);
}