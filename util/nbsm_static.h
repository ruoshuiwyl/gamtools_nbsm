//
// Created by ruoshui on 5/10/18.
//

#ifndef GAMTOOLS_SM_NBSM_STATIC_H
#define GAMTOOLS_SM_NBSM_STATIC_H

#include <cstdint>

namespace gamtools {
    //Count Read int a Sample
    static int64_t sTotalReadNum;
    inline int64_t gTotalReadNum() {
        return sTotalReadNum;
    }
    inline void gIncreamReadNum() {
        sTotalReadNum++;
    }
}

#endif //GAMTOOLS_SM_NBSM_STATIC_H
