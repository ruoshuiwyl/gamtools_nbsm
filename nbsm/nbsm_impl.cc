//
// Created by ruoshui on 5/10/18.
//

#include <cstring>
#include <cstdlib>

#include <hts/htslib/kstring.h>
#include <hts/htslib/sam.h>
#include <sharding/bam_sort_mkdup_impl.h>

#include "bwa_mem/gam_bwa_mem.h"
#include "bwa_mem/bwa.h"

#include "util/glogger.h"

#include "fastqc/gam_fastq_file.h"
#include "fastqc/gam_fastq_read_impl.h"
#include "fastqc/fastq_info_statistics.h"
#include "fastqc/filter_processor.h"
#include "mkdup/gam_mark_duplicate_impl.h"

#include "nbsm_impl.h"
#include "gamtools_sm_impl.h"
#include "util/bounded_queue.h"



#ifdef DEBUG
#include "util/debug_util.h"
#endif

namespace gamtools {

    NBSMImpl::NBSMImpl()
    :  read_fastq_queue_(8),
       input_read_queue_(8) {

    }
    int NBSMImpl::ParseProgramOptions(int argc, char **argv) {
        for (int i = 0; i < argc; ++i){
            command_string_ += argv[i];
        }
        return nbsm_options_.ParserCommandLine(argc, argv);
    }
    void NBSMImpl::Initialization() {
        GLogger::InitLog(nbsm_options_.temporary_directory);
        InitBwaIdxBamHdr();
    }

    void NBSMImpl::InitBwaIdxBamHdr() {
        mem_idx_ = bwa_idx_load(nbsm_options_.reference_file.c_str(), BWA_IDX_ALL);
        const bntseq_t *bns = mem_idx_->bns;
        bam_hdr_ = bam_hdr_init();
        bam_hdr_->n_targets = mem_idx_->bns->n_seqs;
        bam_hdr_->target_len = (uint32_t *) malloc(bam_hdr_->n_targets * sizeof(uint32_t));
        bam_hdr_->target_name = (char **) malloc(bam_hdr_->n_targets * sizeof(char *));
        char *bam_header = new char[8192];
        kstring_t str = {0, 0, nullptr};
        sprintf(bam_header, "@HD\tVN:1.4\tSO:coordinate\n");
        kputsn(bam_header, strlen(bam_header), &str);
        for (int i = 0; i < bam_hdr_->n_targets; ++i) {
            bam_hdr_->target_len[i] = mem_idx_->bns->anns[i].len;
            bam_hdr_->target_name[i] = (char *) malloc (strlen(mem_idx_->bns->anns[i].name) + 1);
            strcpy(bam_hdr_->target_name[i] ,mem_idx_->bns->anns[i].name );
            sprintf(bam_header, "@SQ\tSN:%s\tLN:%d\n", bns->anns[i].name, bns->anns[i].len);
            kputsn(bam_header, strlen(bam_header), &str);
        }
//        char *read_group_line;
//        sprintf(bam_header, "@RG\tID:%s\tSM:%s\tLB:%s\tPL:%s\tCN:%s", nbsm_options_.sample_id.c_str(), nbsm_options_.sample_name.c_str());
        sprintf(bam_header, "%s", nbsm_options_.read_group.c_str());
        char * rg_line = bwa_set_rg(bam_header);
        if (command_string_.size() < 7936){
            sprintf(bam_header, "@PG\tID:bwa\tPN:bwa\tVN:%s\tCL:%s\n", kBWAMEMVersion, command_string_ );
        } else {
            sprintf(bam_header, "@PG\tID:bwa\tPN:bwa\tVN:%s\tCL:%s\n", kBWAMEMVersion, command_string_.substr(0, 7936));
        }

        kputsn(bam_header, strlen(bam_header), &str);
        sprintf(bam_header, "@PG\tID:fastAln \tPN:GAMTOOLS_NBSM\tVN:%s\n", kfastAlnVersion);
        kputsn(bam_header, strlen(bam_header), &str);

        sprintf(bam_header, "%s\n", rg_line);
        free(rg_line);
        kputsn(bam_header, strlen(bam_header), &str);
        bam_hdr_->l_text = str.l;
        bam_hdr_->text = str.s;
        delete [] bam_header;
#ifdef DEBUG
        g_bam_hdr = bam_hdr_;
#endif
    }

    void NBSMImpl::ProcessNBSM() {

        std::unique_ptr<FastqInfoStatistics> fastq_info_stats(
                new FastqInfoStatistics(nbsm_options_.fastq_file_lists.size(),
                                        nbsm_options_.statistics_file,
                                        nbsm_options_.fastq_file_lists)
        );
        int batch_size = nbsm_options_.batch_size * nbsm_options_.nbsm_thread_num;
        GAMFastqReadImpl read_fastq(nbsm_options_.fastq_file_lists, read_fastq_queue_, batch_size);
        FilterProcessor filter(nbsm_options_.filter_options, read_fastq_queue_, input_read_queue_,
                               batch_size, std::move(fastq_info_stats));
        GAMBWAMEM bwa_mem(input_read_queue_, output_gam_channel_, nbsm_options_.mem_opt, mem_idx_);
        SMImpl sort_mkdup(bam_hdr_, nbsm_options_.sm_options, nbsm_options_.output_bam_file, output_gam_channel_);

        // Read->Filter -> Align -> Sharding
        auto read_thread = read_fastq.spawn();
        auto filter_thread = filter.spawn();
        auto bwamem_thread = bwa_mem.spawn();
        auto sm_thread = sort_mkdup.SpawnSharding();
        read_thread.join();
        filter_thread.join();
        bwamem_thread.join();
        sm_thread.join();

        bwa_idx_destroy(mem_idx_);
        // mkdup -> merge_sort -> output_bam
//        auto sm_output_thread = sort_mkdup.SpawnSortMkdup();
//        sm_output_thread.join();

        sort_mkdup.ProcessMarkDuplicate();

    }

    void NBSMImpl::Usage() {
        nbsm_options_.usage();
    }


}