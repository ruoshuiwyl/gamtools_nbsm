//
// Created by ruoshui on 5/23/18.
//

#ifndef GAMTOOLS_SM_DEBUG_UTIL_H
#define GAMTOOLS_SM_DEBUG_UTIL_H
#ifdef DEBUG
#include <htslib/sam.h>
#include "slice.h"
#endif

namespace gamtools {
#ifdef DEBUG
    extern bam_hdr_t *g_bam_hdr;
    void DebugGAMSlice(const Slice &slice);
    void DebugBAMSlice(const Slice &slice);
#endif
}


#endif //GAMTOOLS_SM_DEBUG_UTIL_H
