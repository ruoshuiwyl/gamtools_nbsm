//
// Created by ruoshui on 5/10/18.
//

#include <cstring>
#include <cstdlib>
#include <bwa_mem/bwa.h>
#include <htslib/kstring.h>
#include "bwa_mem/bwa.h"
#include "nbsm_impl.h"

namespace gamtools {
    void NBSMImpl::Initialization() {
        mem_idx_ = bwa_idx_load("", BWA_IDX_ALL);
        const bntseq_t *bns = mem_idx_->bns;
        bam_hdr_t *bam_hdr_ptr = bam_hdr_init();
        bam_hdr_ptr->n_targets = mem_idx_->bns->n_seqs;
        bam_hdr_ptr->target_len = (uint32_t *) malloc(bam_hdr_ptr->n_targets * sizeof(uint32_t));
        bam_hdr_ptr->target_name = (char **) malloc(bam_hdr_ptr->n_targets * sizeof(uint32_t));
        char bam_header[512];
        kstring_t str = {0, 0, nullptr};
        for (int i = 0; i < bam_hdr_ptr->n_targets; ++i) {
            bam_hdr_ptr->target_len[i] = mem_idx_->bns->anns[i].len;
            bam_hdr_ptr->target_name[i] = (char *) malloc (strlen(mem_idx_->bns->anns[i].name) + 1);
            strcpy(bam_hdr_ptr->target_name[i] ,mem_idx_->bns->anns[i].name );
            sprintf(bam_header, "@SQ\tSN:%s\tLN:%d\n", bns->anns[i].name, bns->anns[i].len);
            kputsn(bam_header, strlen(bam_header), &str);
        }
        char *read_group_line;
        char * rg_line = bwa_set_rg(read_group_line);
        free(rg_line);
        sprintf(bam_header, "@PG\tID:bwa\tPN:bwa\tVN:%s\tCL:%s", "v0.7.15","gamtools" );
        kputsn(bam_header, strlen(bam_header), &str);
        sprintf(bam_header, "@PG\tID:NBSM\tPN:GAMTOOLS_NBSM\tVN:%s\n", "v0.1.0");
        kputsn(bam_header, strlen(bam_header), &str);
        sprintf(bam_header, "%s\n", rg_line);
        kputsn(bam_header, strlen(bam_header), &str);



    }
}