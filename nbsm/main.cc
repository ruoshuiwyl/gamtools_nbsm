//
// Created by ruoshui on 4/23/18.
//



#include "gamtools_sm_impl.h"
#include "nbsm/nbsm_impl.h"

using  namespace gamtools;
int main(int argc, char *argv[] ) {
    NBSMImpl nbsm;
    int err_code;
    if (argc == 1) {
        nbsm.Usage();
        return 0;
    }
    if (!nbsm.ParseProgramOptions(argc, argv)) {
        nbsm.Initialization();
        nbsm.ProcessNBSM();
        err_code = 0;
    } else {
        err_code = 1;
    }
    return err_code;
}