//
// Created by ruoshui on 5/10/18.
//

#ifndef GAMTOOLS_SM_GAM_BWA_MEM_H
#define GAMTOOLS_SM_GAM_BWA_MEM_H

#include <cstdint>
#include <util/array_block_queue.h>
#include <util/gam_read_buffer.h>
#include <lib/htslib-1.3.1/htslib/sam.h>
#include "bwamem.h"

namespace gamtools {
    class GAMBWAMEM {
    public:
        explicit GAMBWAMEM(BlockQueue <std::unique_ptr<BWAReadBuffer>> &input,
                           BlockQueue <std::unique_ptr<BWAReadBuffer>> &output,
                           const mem_opt_t *opt, bwaidx_t *mem_idx);
//        Initilization(const mem_opt_t *opt, const char *ref_file, bam_hdr_t **bam_hdr, const char *read_group_line);
        ~GAMBWAMEM();
        std::thread spawn();
//        GAMFastQSeq *ProcessPairEndReadMap(GAMFastQSeq *fastq_seq);
    private:
        void ProcessBWAMEM();
        bwaidx_t *mem_idx_;
        mem_pestat_t *mem_pestat_;
        const mem_opt_t *mem_opt_ ;
        uint64_t processed_num_;
        BlockQueue<std::unique_ptr<BWAReadBuffer>> &input_;
        BlockQueue<std::unique_ptr<BWAReadBuffer>> &output_;
    };
}


#endif //GAMTOOLS_SM_GAM_BWA_MEM_H
