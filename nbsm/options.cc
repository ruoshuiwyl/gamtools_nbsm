//
// Created by ruoshui on 5/2/18.
//


#include "nbsm/options.h"
#include <unistd.h>
#include <cstring>


namespace  gamtools{
    SMOptions::SMOptions()
        :sort_region_size(1<<20),
         sort_block_size(1<<20),
         markdup_region_size(10 * (1<<20)),
         markdup_block_size(1<<20),
         block_sort_thread_num(2),
         merge_sort_thread_num(4),
         bam_output_thread_num(4) {

        char dirname[4096];
        char *dir = getcwd(dirname, 4096);
        directory.assign(dir, strlen(dir));
    }


    FilterOptions::FilterOptions()
            :  filter_adapter(true),
               is_adpt_list(false),
               qual_sys(33),
               n_base_rate(0.05),
               low_qual(10),
               qual_rate(0.05),
               adapter_mis_match(1),
               adapter_match_ratio(0.5),
               adapter1("AAGTCGGAGGCCAAGCGGTCTTAGGAAGACAA"),
               adapter2("AAGTCGGATCGTAGCCATGTCGTTCTGTGAGCCAAGGAGTTG") {

    }

}