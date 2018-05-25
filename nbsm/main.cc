//
// Created by ruoshui on 4/23/18.
//



#include "gamtools_sm_impl.h"
#include "nbsm/nbsm_impl.h"

using  gamtools;
int main(int argc, char *argv[] ) {
    NBSMImpl nbsm;
    int err_code;
    if (!nbsm.ParseProgramOptions(argc, argv)) {
        nbsm.CheckHelp();
        nbsm.CkeckVersion();

        nbsm.Initialization();
        nbsm.ProcessNBSM();
        err_code = 0;
    } else {
        err_code = 1;
    }
    return err_code;



    return 0;
}