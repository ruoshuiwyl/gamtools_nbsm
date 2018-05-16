//
// Created by ruoshui on 5/14/18.
//

#ifndef GAMTOOLS_SM_NBSM_CONFIG_H
#define GAMTOOLS_SM_NBSM_CONFIG_H
#include <boost/program_options.hpp>
#include <nbsm/options.h>

namespace gamtools {


    class NBSMOptions {
    public:
        NBSMOptions();
        /*
         * return parameter error code
         * 0 : ok
         * 1 : parameter must set but not set
         * 2 : parameter number error
         * 3 : parameter repeat
         * 4 : unknown parameter
         * 5 : other error
       */
        int ParserCommandLine(int argc, char *argv[]);
//        BaseOptions base_options;
//        FilterOptions filter_options;
//        BWAMEMOptions bwamem_options;
//        SORTMKDUPOptions sm_options;
    private:
        int nbsm_thread_num;
        std::string sample_id_;
        std::string sample_name_;
        std::string reference_file_;
        std::string output_bam_file_;
        std::string temporary_directory_;
        std::vector<int> input_library_ids_;
        std::vector<int> input_lane_ids_;
        std::vector<std::string> input_fastq1_lists_;
        std::vector<std::string> input_fastq2_lists_;
        boost::program_options::options_description opt_des_;
        boost::program_options::options_description base_des_;
        boost::program_options::options_description bwamem_des_;
        boost::program_options::options_description filter_des_;
        boost::program_options::options_description sort_mkdup_des_;
    };
}


#endif //GAMTOOLS_SM_NBSM_CONFIG_H
