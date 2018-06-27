//
// Created by ruoshui on 5/10/18.
//

#ifndef GAMTOOLS_SM_NBSM_IMPL_H
#define GAMTOOLS_SM_NBSM_IMPL_H

#include <hts/htslib/sam.h>
#include <util/gam_read_buffer.h>
#include <util/array_block_queue.h>
#include <util/bounded_queue.h>
#include "nbsm_options.h"

namespace gamtools {

    class NBSMImpl {
    public:
        NBSMImpl();
        int ParseProgramOptions(int argc, char *argv[]);

        void Initialization();
        void ProcessNBSM();
    private:
        void InitBwaIdxBamHdr();
        bwaidx_t *mem_idx_;
        bam_hdr_t *bam_hdr_;
        NBSMOptions nbsm_options_;
//        ArrayBlockQueue<std::unique_ptr<GAMReadBuffer>> read_fastq_channel_;
        BoundedQueue<std::unique_ptr<GAMReadBuffer>> read_fastq_queue_;
        BoundedQueue<std::unique_ptr<BWAReadBuffer>> input_read_queue_;
//        ArrayBlockQueue<std::unique_ptr<BWAReadBuffer>> input_fastq_channel_;
        ArrayBlockQueue<std::unique_ptr<BWAReadBuffer>> output_gam_channel_;
    };
}


#endif //GAMTOOLS_SM_NBSM_IMPL_H
