//
// Created by ruoshui on 5/10/18.
//



//#include <string>
//#include <util/channel.h>
//#include <fastqc/gam_fastq_read_impl.h>
//#include <bwa_mem/gam_bwa_mem.h>
//#include <fastqc/filter_processor.h>
//#include "util/gam_read_buffer.h"
//#include "fastqc/gam_fastq_file.h"
//
//#include "bwa_mem/bwamem.h"
//#include "gamtools_sm_impl.h"
//#include "mkdup/gam_mark_duplicate_impl.h"

//#include "boost/program_options.hpp"
#include "nbsm/nbsm_impl.h"
#include <fastqc/gam_fastq_file.h>
#include <util/glogger.h>

namespace gamtools {
    int nbsm_impl_test(int argc, char *argv[]) {
        NBSMImpl nbsm;
        int err_code;
        if (!nbsm.ParseProgramOptions(argc, argv)) {
            nbsm.Initialization();
            nbsm.ProcessNBSM();
            err_code = 0;
        } else {
            err_code = 1;
        }
        return err_code;
    }
}

int main(int argc, char *argv[]) {
    return gamtools::nbsm_impl_test(argc, argv);

//    std::string fastq1_filename = "/home/ruoshui/data/fq/150625_I742_FCHTFNJADXX_L1_HUMdtdXLEYAAAPEI-147_1.fq.gz";
//    std::string fastq2_filename = "/home/ruoshui/data/fq/150625_I742_FCHTFNJADXX_L1_HUMdtdXLEYAAAPEI-147_2.fq.gz";
//    std::string fastq1_filename = "/home/ruoshui/data/na12878/chr21_1m/readid_fastq1.fq";
//    std::string fastq2_filename = "/home/ruoshui/data/na12878/chr21_1m/readid_fastq2.fq";
//    std::string refer_filename = "/home/ruoshui/data/na12878/chr21_1m/refer/ch21_10m_11m.fa";
//    std::string cache_dir;
//
//    gamtools::GAMFastqFileInfo fastq_file_info;
//    std::vector<gamtools::GAMFastqFileInfo> fastq_file_infos;
//    fastq_file_info.fastq1_filename = fastq1_filename;
//    fastq_file_info.fastq2_filename = fastq2_filename;
//    fastq_file_info.lib_id = 0;
//    fastq_file_info.lane_id = 0;
//    fastq_file_infos.push_back(fastq_file_info);
//    bam_hdr_t *bam_hdr;
//    gamtools::SMOptions options;
//    std::string bam_file = "test.bam";
//    gamtools::Channel<std::unique_ptr<gamtools::GAMReadBuffer>> output_fastq_channel;
//    gamtools::Channel<std::unique_ptr<gamtools::BWAReadBuffer>> input;
//    gamtools::Channel<std::unique_ptr<gamtools::BWAReadBuffer>> output;
//    int batch_size = 1 << 16;
//    gamtools::GAMFastqReadImpl read_fastq(fastq_file_infos, output_fastq_channel, batch_size);
//
//    gamtools::FilterOptions filter_options;
//    gamtools::FilterProcessor filter(filter_options, output_fastq_channel, input, 1, batch_size);
//    gamtools::GAMBWAMEM bwa_mem(input, output, nullptr, nullptr);
//
//    gamtools::SMImpl sort_mkdup(bam_hdr, options, bam_file, output);
//    mem_opt_t *opt = mem_opt_init();
//    opt->flag |= MEM_F_PE;
//    bwa_mem.Initilization(opt, refer_filename.c_str(), nullptr, nullptr);
//    auto read_thread = read_fastq.spawn();
//    auto filter_thread = filter.spawn();
//    auto bwamem_thread = bwa_mem.spawn();
//    auto sm_thread = sort_mkdup.spawn();
//    read_thread.join();
//    filter_thread.join();
//    bwamem_thread.join();
//    sm_thread.join();
//    sort_mkdup.OutputBAM();
//
//    boost::program_options::options_description desc;
    return 0;
}