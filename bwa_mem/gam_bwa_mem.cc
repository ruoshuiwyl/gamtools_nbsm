//
// Created by ruoshui on 5/10/18.
//

#include <lib/htslib-1.3.1/htslib/sam.h>
#include <cstring>
#include "gam_bwa_mem.h"
#include "bwa.h"
#include "bntseq.h"


namespace gamtools {
    void GAMBWAMEM::ProcessBWAMEM() {
        std::unique_ptr<BWAReadBuffer> read_buffer;
        while (input_.read(read_buffer)) {
            mem_process_seqs(mem_opt_, mem_idx_->bwt, mem_idx_->bns, mem_idx_->pac, processed_num_, read_buffer->size, read_buffer->seqs,
                             mem_pestat_);
            processed_num_ += read_buffer->size;
            output_.write(std::move(read_buffer));
        }
        if (input_.eof()) {
            output_.SendEof();
        }
    }

    GAMBWAMEM::GAMBWAMEM(Channel<std::unique_ptr<BWAReadBuffer>> &input,
                         Channel<std::unique_ptr<BWAReadBuffer>> &output,
                         const mem_opt_t *opt, bwaidx_t *mem_idx)
        : input_(input),
          output_(output),
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