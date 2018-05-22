//
// Created by ruoshui on 5/10/18.
//

#ifndef GAMTOOLS_SM_NBSM_STATIC_H
#define GAMTOOLS_SM_NBSM_STATIC_H

#include <cstdint>

namespace gamtools {
    //Count Read int a Sample
    struct ReadID{
        static int64_t sTotalReadID;
        static int64_t gTotalReadID(){
            return sTotalReadID;
        }
        static void gIncreamReadID() {
            sTotalReadID++;
        }

    };
//    static int64_t sTotalReadNum;
//    int64_t ReadID::gTotalReadNum() {
//        return sTotalReadNum;
//    }
//    inline void ReadID::gIncreamReadNum() {
//        sTotalReadNum++;
//    }
}

#endif //GAMTOOLS_SM_NBSM_STATIC_H
