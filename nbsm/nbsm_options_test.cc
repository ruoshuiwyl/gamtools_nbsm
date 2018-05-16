//
// Created by ruoshui on 5/14/18.
//

#include "nbsm_options.h"
using namespace gamtools;
int main(int argc, char *argv[] ) {

    NBSMOptions nbsm_options;
    nbsm_options.ParserCommandLine(argc, argv);

    return 0;
}