//
// Created by ruoshui on 5/10/18.
//

#include <cstring>
#include <cstdlib>

#include <htslib/kstring.h>
#include <htslib/sam.h>

#include "bwa_mem/gam_bwa_mem.h"
#include "bwa_mem/bwa.h"

#include "util/glogger.h"
#include "fastqc/gam_fastq_read_impl.h"
#include "fastqc/filter_processor.h"
#include "nbsm_impl.h"
#include "gamtools_sm_impl.h"
#include "mkdup/gam_mark_duplicate_impl.h"
#include "fastqc/gam_fastq_file.h"

namespace gamtools {

    NBSMImpl::NBSMImpl() {

    }
    int NBSMImpl::ParseProgramOptions(int argc, char **argv) {
        int type = nbsm_options_.ParserCommandLine(argc, argv);
        if (type >= 2) {
            GLOG_ERROR << "Parser Parameter Error" ;
            nbsm_options_.help();
            return 1;
        } else if (type == 1){
            return 1;
        }
        return 0;
    }
    void NBSMImpl::Initialization() {
        mem_idx_ = bwa_idx_load(nbsm_options_.reference_file.c_str(), BWA_IDX_ALL);
        const bntseq_t *bns = mem_idx_->bns;
        bam_hdr_ = bam_hdr_init();
        bam_hdr_->n_targets = mem_idx_->bns->n_seqs;
        bam_hdr_->target_len = (uint32_t *) malloc(bam_hdr_->n_targets * sizeof(uint32_t));
        bam_hdr_->target_name = (char **) malloc(bam_hdr_->n_targets * sizeof(char *));
        char bam_header[512];
        kstring_t str = {0, 0, nullptr};
        for (int i = 0; i < bam_hdr_->n_targets; ++i) {
            bam_hdr_->target_len[i] = mem_idx_->bns->anns[i].len;
            bam_hdr_->target_name[i] = (char *) malloc (strlen(mem_idx_->bns->anns[i].name) + 1);
            strcpy(bam_hdr_->target_name[i] ,mem_idx_->bns->anns[i].name );
            sprintf(bam_header, "@SQ\tSN:%s\tLN:%d\n", bns->anns[i].name, bns->anns[i].len);
            kputsn(bam_header, strlen(bam_header), &str);
        }
//        char *read_group_line;
        sprintf(bam_header, "@RG\tID:%s\tSM:%s", nbsm_options_.sample_id.c_str(), nbsm_options_.sample_name.c_str());
        char * rg_line = bwa_set_rg(bam_header);

        sprintf(bam_header, "@PG\tID:bwa\tPN:bwa\tVN:%s\tCL:%s", "v0.7.15","gamtools" );
        kputsn(bam_header, strlen(bam_header), &str);
        sprintf(bam_header, "@PG\tID:NBSM\tPN:GAMTOOLS_NBSM\tVN:%s\n", "v0.1.0");
        kputsn(bam_header, strlen(bam_header), &str);

        sprintf(bam_header, "%s\n", rg_line);
        free(rg_line);
        kputsn(bam_header, strlen(bam_header), &str);
        bam_hdr_->l_text = str.l;
        bam_hdr_->text = str.s;
    }

    void NBSMImpl::ProcessNBSM() {
        GAMFastqReadImpl read_fastq(nbsm_options_.fastq_file_lists, read_fastq_, nbsm_options_.batch_size);
        FilterProcessor filter(nbsm_options_.filter_options, read_fastq_, input_fastq_, 1, nbsm_options_.batch_size);
        GAMBWAMEM bwa_mem(input_fastq_, output_gam_, nbsm_options_.mem_opt, mem_idx_);
        SMImpl sort_mkdup(bam_hdr_, nbsm_options_.sm_options, nbsm_options_.output_bam_file, output_gam_);

        auto read_thread = read_fastq.spawn();
        auto filter_thread = filter.spawn();
        auto bwamem_thread = bwa_mem.spawn();
        auto sm_thread = sort_mkdup.spawn();

        read_thread.join();
        filter_thread.join();
        bwamem_thread.join();
        sm_thread.join();
        sort_mkdup.OutputBAM();
    }
}