//
// Created by ruoshui on 5/10/18.
//

#ifndef GAMTOOLS_SM_NBSM_IMPL_H
#define GAMTOOLS_SM_NBSM_IMPL_H

#include <htslib/sam.h>

namespace gamtools {
    class NBSMImpl {
    public:
        NBSMImpl();
        void ParseProgramOptions(int argc, char *argv[]);
        void Initialization();
        void ProcessNBSM();
    private:
        bwaidx_t *mem_idx_;
        bam_hdr_t *bam_hdr_;
    };
}


#endif //GAMTOOLS_SM_NBSM_IMPL_H
