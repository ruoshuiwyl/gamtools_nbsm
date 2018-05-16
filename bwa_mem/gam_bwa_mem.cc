//
// Created by ruoshui on 5/10/18.
//

#include <lib/htslib-1.3.1/htslib/sam.h>
#include <cstring>
#include "gam_bwa_mem.h"
#include "bwa.h"
#include "bntseq.h"


namespace gamtools {
    static const std::string kBWAMEMVersion = "0.7.15-r1140";
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

    GAMBWAMEM::GAMBWAMEM(gamtools::Channel<std::unique_ptr<gamtools::BWAReadBuffer>> &input,
                         gamtools::Channel<std::unique_ptr<gamtools::BWAReadBuffer>> &output)
        : input_(input), output_(output), processed_num_(0) {

    }
    void GAMBWAMEM::Initilization(const mem_opt_t *opt, const char *ref_file, bam_hdr_t **bam_hdr, const char *read_group_line) {
        mem_opt_ = opt;
        mem_idx_ = bwa_idx_load(ref_file, BWA_IDX_ALL);
        bam_hdr_t *bam_hdr_ptr = bam_hdr_init();
        const bntseq_t *bns = mem_idx_->bns;
        bam_hdr_ptr->n_targets = mem_idx_->bns->n_seqs;
        bam_hdr_ptr->target_len = (uint32_t *) malloc(bam_hdr_ptr->n_targets * sizeof(uint32_t));
        bam_hdr_ptr->target_name = (char **) malloc(bam_hdr_ptr->n_targets * sizeof(uint32_t));
        char bam_header[512];
        for (int i = 0; i < bam_hdr_ptr->n_targets; ++i) {
            bam_hdr_ptr->target_len[i] = mem_idx_->bns->anns[i].len;
            bam_hdr_ptr->target_name[i] = (char *) malloc (strlen(mem_idx_->bns->anns[i].name) + 1);
            strcpy(bam_hdr_ptr->target_name[i] ,mem_idx_->bns->anns[i].name );
            sprintf(bam_header, "@SQ\tSN:%s\tLN:%d\n", bns->anns[i].name, bns->anns[i].len);
        }
        char * rg_line = bwa_set_rg(read_group_line);
        free(rg_line);
        sprintf(bam_header, "@PG\tID:bwa\tPN:bwa\tVN:%s\tCL:%s", kBWAMEMVersion.c_str(), "");
        sprintf(bam_header, "@PG\tID:NBSM\tPN:GAMTOOLS_NBSM\tVN:%s\n", "v0.1.0");
        sprintf(bam_header, "%s\n", read_group_line);

        bam_hdr_ptr->l_text;
        bam_hdr_ptr->target_name;
        *bam_hdr = bam_hdr_ptr;

    }

    GAMBWAMEM::~GAMBWAMEM() {

    }

    std::thread GAMBWAMEM::spawn() {
        return std::thread(&GAMBWAMEM::ProcessBWAMEM, this);
    }

    const bntseq_t* GAMBWAMEM::bntseq() const {
        return mem_idx_->bns;
    }
}