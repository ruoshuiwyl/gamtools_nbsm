//
// Created by ruoshui on 5/18/18.
//
#ifndef GAMTOOLS_SM_OPTIONS_H
#define GAMTOOLS_SM_OPTIONS_H
#include <string>
namespace  gamtools {
    struct SMOptions {

        // sort region size shift size, actually 1 << shift_size
        // Default : 22 (4M)
        int sort_region_size;

        //sort region buffer size to save gam record;
        //Default : 1M
        int sort_block_size;
        // markdup region size
        // Default : 8M
        int markdup_region_size;

        // markdup region size
        // Default : 1M
        int markdup_block_size;

        // nbsm will create max file number in directory
        //Default : 50
        int max_file_size;

        // nbsm store Intermediate file directory
        // Default : current directory
        std::string directory;


        // Default : thread number 1;
        int block_sort_thread_num;

        // Default : thread number 1;
        int read_gam_thread_num;

        int stat_thread_num;

        // Default : thread number 1;
        int merge_sort_thread_num;

        // Default : thread number 1;
        int bam_output_thread_num;

        // Default : thread number 1;
        int mark_dup_thread_num;

        std::string report_file;

        std::string ref_file;

        std::string bed_file;

        SMOptions ();
    };
    struct FilterOptions {

        //Default  : true
        bool filter_adapter;

        //default : false
        bool is_adpt_list ;

        // Default "AAGTCGGAGGCCAAGCGGTCTTAGGAAGACAA"
        // Default "AAGTCGGATCGTAGCCATGTCGTTCTGTGAGCCAAGGAGTTG")
        std::string adapter1;
        std::string adapter2;

        // Default : quality system sanger
        int qual_sys;

        // Default : 5
        int low_qual;    //low quality
        // Default : 0.5
        float qual_rate;
        // Default : 0.05
        float n_base_rate;
        // Default : 1
        int adapter_mis_match;


        // adapter's shortest match ratio
        // Default : 0.5
        float adapter_match_ratio;

        // Filter thread num
        // Default : 2
        int filter_thread_num;

        FilterOptions();
    };
}

#endif //GAMTOOLS_SM_OPTIONS_H

