//
// Created by ruoshui on 5/14/18.
//

#ifndef GAMTOOLS_SM_NBSM_CONFIG_H
#define GAMTOOLS_SM_NBSM_CONFIG_H
#include <boost/program_options.hpp>
#include <nbsm/options.h>
#include <bwa_mem/bwamem.h>

namespace gamtools {

    const std::string kNBSMVersion = "0.1.0";
    const std::string kBWAMEMVersion = "0.7.15-r1140";

    class GAMFastqFileInfo;
    class NBSMOptions {
    public:
        NBSMOptions();
        ~NBSMOptions();
        /*
         * return parameter error code
         * 0 : ok
         * 1 : return help or version
         * 2 : parameter must set but not set
         * 3 : parameter number error
         * 4 : parameter repeat
         * 5 : unknown parameter
         * 6 : other error
       */
        int ParserCommandLine(int argc, char *argv[]);
        void help();
//        BaseOptions base_options;
        FilterOptions filter_options;
        SMOptions sm_options;
        mem_opt_t *mem_opt;
        std::vector<GAMFastqFileInfo> fastq_file_lists;
        std::string output_bam_file;
        std::string reference_file;
        std::string sample_id;
        std::string sample_name;
        int batch_size;
        int read_len;
        int nbsm_thread_num;
        std::string temporary_directory;
    private:


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
