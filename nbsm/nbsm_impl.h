//
// Created by ruoshui on 5/10/18.
//

#ifndef GAMTOOLS_SM_NBSM_IMPL_H
#define GAMTOOLS_SM_NBSM_IMPL_H

#include <htslib/sam.h>
#include <util/gam_read_buffer.h>
#include <util/channel.h>
#include "nbsm_options.h"

namespace gamtools {

    class NBSMImpl {
    public:
        NBSMImpl();
        int ParseProgramOptions(int argc, char *argv[]);
        void Initialization();
        void ProcessNBSM();

    private:
        bwaidx_t *mem_idx_;
        bam_hdr_t *bam_hdr_;
        NBSMOptions nbsm_options_;
        Channel<std::unique_ptr<GAMReadBuffer>> read_fastq_;
        Channel<std::unique_ptr<BWAReadBuffer>> input_fastq_;
        Channel<std::unique_ptr<BWAReadBuffer>> output_gam_;
    };
}


#endif //GAMTOOLS_SM_NBSM_IMPL_H
