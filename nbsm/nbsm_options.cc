//
// Created by ruoshui on 5/14/18.
//

#include <iostream>
#include "nbsm_options.h"
#include <string>
#include <util/glogger.h>
#include <bwa_mem/bwamem.h>
#include <fastqc/gam_fastq_file.h>
#include <boost/filesystem.hpp>

namespace po = boost::program_options;
namespace gamtools {
//    NBSMOptions::NBSMOptions(): opt_des_("GAMTOOLS NBSM kNBSMVersion:V0.1.0"),
//                                base_des_("Basic SMOptions"),
//                                filter_des_("Filter SMOptions"),
//                                bwamem_des_("Bwa_mem SMOptions"),
//                                sort_mkdup_des_("Sort  Mark Duplicate SMOptions"){
    NBSMOptions::NBSMOptions() {
        base_des_.add_options()
                // base options
                ("help,h", "Print CheckHelp message")
                ("version,v", "Print version message")
                ("reference_file,r", po::value<std::string>(&reference_file), "Reference sequence file Default(null)")
                ("temp_dir,d", po::value<std::string>(&temporary_directory), "Temporary directory Storage space size must be twice BAM file size Default(null)")
                ("input_fastq1_lists,f", po::value<std::vector<std::string>>(&input_fastq1_lists_), "Input fastq1 file lists")
                ("input_fastq2_lists,b", po::value<std::vector<std::string>>(&input_fastq2_lists_), "Input fastq2 file lists")
                ("input_library_id_lists,l",po::value<std::vector<int>>(&input_library_ids_), "Input fastq file library ID" )
                ("input_lane_id_lists,a",   po::value<std::vector<int>>(&input_lane_ids_),      "Input fastq file lane ID")
                ("output_bam_file,o", po::value<std::string>(&output_bam_file), "Output bam file name defalut(null)")
                ("sample_name,n", boost::program_options::value<std::string>(&sample_name)->default_value("Zebra"), "Sample Name Default(Zebra)")
                ("sample_id,i", boost::program_options::value<std::string>(&sample_id)->default_value("Zebra"), "Sample Name Default(Zebra)")
                ("thread_number,t", boost::program_options::value<int>(&nbsm_thread_num)->default_value(1),"NBSM total thread Default(1)")
                ("batch_size", boost::program_options::value<int>(&batch_size)->default_value(100000), "NBSM process batch read size Default(20 * 10000000)")
                ("read_len", boost::program_options::value<int>(&read_len)->default_value(100), "NBSM process read length Default(100)")
                ("bed_file,B", boost::program_options::value<std::string>(&bed_file), "NBSM process target data bed file");

        // filter fastq options
        filter_des_.add_options()
                ("filter_low_qual", po::value<int>(), "Filter Read low quality threshold Default(10)")
                ("filter_qual_rate", po::value<float>(),"Filter Read low quality rate Default(0.5")
                ("filter_n_nate",  po::value<float>(), "Filter Read N rate threshold Default (0.05)")
                ("filter_adapter1", po::value<std::string>(), "3' adapter sequence Default(AAGTCGGAGGCCAAGCGGTCTTAGGAAGACAA)")
                ("filter_adapter2", po::value<std::string>(), "5' adapter sequence Default(AAGTCGGATCGTAGCCATGTCGTTCTGTGAGCCAAGGAGTTG)")
                ("filter_mis_match", po::value<int>(), "the max mismatch number when match the adapter Default(1)")
                ("filter_match_ratio", po::value<float>(), "adapter's shortest match ratio Default(0.5)")
                ("filter_qual_system", po::value<int>(), "quality system 1:illumina, 2:sanger Default(2:illumina)");
            // bwa mem options
        bwamem_des_.add_options()
                ("bwamem_thread", po::value<int>(), "bwa mem number of threads Default(nbsm thread)")
                ("bawmem_min_seed", po::value<int>(), "bwa mem minimum seed length Default(19)")
                ("bwamem_band_width",po::value<int>(), "bwa mem band width for banded alignment Default(100)")
                ("bwamem_x_dropoff", po::value<int>(), "bwa mem off-diagonal X-dropoff Default(100)")
                ("bwamem_internal_seed", po::value<float>(), "look for internal seeds inside a seed longer than {-k} * FLOAT Defulat(1.5)")
                ("bwamem_skip_number", po::value<int>(), "skip seeds with more than INT occurrences Default(500)")
                ("bwamem_drop_chain", po::value<float>(), "drop chains shorter than FLOAT fraction of the longest overlapping chain Defulat(0.50)")
                ("bwamem_discard_len", po::value<int>(), "discard a chain if seeded bases shorter than INT Defalut(0)")
                ("bwamem_max_rounds", po::value<int>(), "perform at most INT rounds of mate rescues for each read Default(50)")
                ("bwamem_skip_mate_rescue", po::value<int>(), "bwa mem skip mate rescue")
                ("bwamem_skip_pairing", po::value<int>(), "bwa mem skip pairing; mate rescue performed unless -S also in use")
                ("bwamem_match_score", po::value<int>(), "bwa mem score for a sequence match Default(1)")
                ("bwamem_minmatch_score", po::value<int>(), "bwa mem  penalty for a mismatch Default(4)")
                ("bwamem_gap_open_penalties", po::value<int>(), "gap open penalties for deletions and insertions Default[6,6]")
                ("bwamem_gap_extension_score", po::value<int>(), "gap extension penalty; a gap of size k cost '{-O} + {-E}*k' Default[1,1]")
                ("bwamem_clipping_penalty", po::value<int>(), " penalty for 5'- and 3'-end clipping Default[5,5]")
                ("bwamem_unpair_penalty", po::value<int>(), "penalty for an unpaired read pair Default(17)");

        sort_mkdup_des_.add_options()
                ("sm_sort_sharding_size", po::value<int>(), "Sort Sharding size Default(4M)")
                ("sm_sort_block_size", po::value<int>(), "sort region buffer size to save gam record Default(1M)")
                ("sm_mkdup_sharding_size", po::value<int>(), "Mark Duplicate Sharding size Default(8M)")
                ("sm_mkdup_block_size", po::value<int>(), "Mark Duplicate block buffer size for to save mkdup info Default(1M)")
                ("sm_block_sort_thread", po::value<int>(), "sharding stage block sort thread number Default(4)")
                ("sm_read_gam_thread_num", po::value<int>(), "Merge stage read sharding data thread number Default(1)")
                ("sm_merge_thread", po::value<int>(), "Merge sort thread num Default(1)")
                ("sm_compress_bam_thread", po::value<int>(), "Compress bam thread number Default(1)")
                ("sm_mkdup_thread", po::value<int>(), "Mark Duplicate thread number Default (1)");

        opt_des_.add(base_des_).add(filter_des_).add(bwamem_des_).add(sort_mkdup_des_);

        mem_opt = mem_opt_init();
        mem_opt->flag |= MEM_F_PE; //default set pair end read
    }
    NBSMOptions::~NBSMOptions() {
        free (mem_opt);
    }
    int NBSMOptions::ParserCommandLine(int argc, char **argv) {
        po::store(po::parse_command_line(argc, argv, opt_des_), vm);
        po::notify(vm);

        //CheckHelp
        if (!CheckHelp()){
            return 1;
        }
        int status = 0;
        if (status = ParserBasicParameters()) {
            return  status;
        }
        if (status = ParserFilterParameter()) {
             return status;
        }
        if (status = ParserBWAMEMParameter()) {
            return  status;
        }
        if (status = ParserSortMkdupParameter()) {
            return status;
        }
        return 0;
    }

    int NBSMOptions::ParserBasicParameters() {
        GLOG_INFO << "Start Parse command line";
        if (vm.count("temp_dir")) {
            if (temporary_directory.back() == '/') { // skip last directory separator
                temporary_directory.pop_back();
            }
            GLOG_INFO << "Set temp dir " << temporary_directory ;
            boost::filesystem::path temp_path(temporary_directory);
            if (boost::filesystem::exists(temp_path)) {
                if (boost::filesystem::is_directory(temp_path)) {
//                    boost::filesystem::remove_file(temp_path);
                } else {
                    GLOG_WARNING << "temporary_directory is not directory; please reset temp directory";
                }
            } else {
                GLOG_INFO << "temp directory not exist; create new temp directory";
                if (boost::filesystem::create_directories(temp_path)) {
                    GLOG_INFO << "Create temp directory OK";
                } else {
                    GLOG_ERROR << "Create temp directory failed";
                }
            }
        } else {
            GLOG_ERROR << "No Set temp dir ";
            return 2;
        }

        if (vm.count("reference_file")) {
            GLOG_INFO << "Reference File:" << reference_file ;
            sm_options.ref_file =  reference_file + ".fai";
        } else {
            GLOG_ERROR << "Not set reference file:";
            return 2;
        }

        if (vm.count("input_fastq1_lists")) {
            GLOG_INFO<< "Input Fastq1 File Lists:";
            int file_num = 0;
            for (auto file : input_fastq1_lists_) {
                GLOG_INFO << "file id:" << file_num++ << ":" << file;
            }
        } else {
            GLOG_ERROR << "Not set input_fastq1_lists";
            return 2;
        }

        if (vm.count("input_fastq2_lists")) {
            GLOG_INFO << "Input Fastq1 File Lists:";
            int file_num = 0;
            for (auto file : input_fastq2_lists_) {
                GLOG_INFO << "file id:" << file_num++ << ":" << file ;
            }
        } else {
            GLOG_ERROR << "Not set input_fastq2_lists";
            return 2;
        }

        if (vm.count("input_library_id_lists")) {
            GLOG_INFO << "Input library id lists:";
            for (auto id : input_library_ids_) {
                GLOG_INFO << "file library id:" << id;
            }
        } else {
            GLOG_ERROR << "Not set input_library_id_lists";
            return 2;
        }

        if (vm.count("input_lane_id_lists")) {
            GLOG_INFO << "Input library id lists:";
            for (auto id : input_lane_ids_) {
                GLOG_INFO << "file lane id:" << id;
            }
        } else {
            GLOG_ERROR << "Not set input_lane_id_lists";
            return 2;
        }
        if (input_fastq1_lists_.size() != input_fastq2_lists_.size() ||
            input_fastq1_lists_.size() != input_library_ids_.size() ||
            input_fastq1_lists_.size() != input_lane_ids_.size()) {
            GLOG_ERROR << "PE-fastq file library and lane number not equal";
            return 6;
        } else {
            for (int i = 0; i < input_fastq1_lists_.size(); ++i ) {
                GAMFastqFileInfo fastq_file_info;
                fastq_file_info.fastq1_filename = input_fastq1_lists_[i];
                fastq_file_info.fastq2_filename = input_fastq2_lists_[i];
                fastq_file_info.lib_id = input_library_ids_[i];
                fastq_file_info.lane_id = input_lane_ids_[i];
                fastq_file_lists.push_back(fastq_file_info);
            }
        }
        if (vm.count("output_bam_file")) {
            GLOG_INFO << "Set Output bam filename " << output_bam_file;
            boost::filesystem::path output_bam_path(output_bam_file);
            boost::filesystem::path qc_path = boost::filesystem::change_extension(output_bam_path, "qc");
            statistics_file = qc_path.string();
            sm_options.report_file = statistics_file;
            GLOG_INFO << "Set output QC filename " << statistics_file;
        } else {
            GLOG_ERROR << "Not set output_bam_file";
            return 2;
        }

        if (vm.count("sample_name")) {
            GLOG_INFO << "Set Sample Name " << sample_name;
        } else {
            GLOG_ERROR << "Not Set Sample Name" ;
            return 2;
        }

        if (vm.count("sample_id")) {
            GLOG_INFO << "Set Sample ID: " << sample_id;
        } else {
            GLOG_ERROR << "Not Set Sample ID" ;
            return 2;
        }

        if (vm.count("thread_number")) {
            GLOG_INFO << "Set nbsm thread number";
        } else {
            GLOG_INFO << " nbsm thread number default 20";
        }

        if (vm.count("batch_size")) {
            GLOG_INFO << "Set nbsm batch_size " << batch_size;
        } else {
            GLOG_INFO << "Default set nbsm batch_size " << batch_size;
        }

        if (vm.count(bed_file)) {
            sm_options.bed_file = bed_file;
            GLOG_INFO << "BED file " << bed_file;
        }
        return 0;
    }


    int NBSMOptions::ParserFilterParameter() {
        //Filter
        if (vm.count("filter_low_qual")) {
            filter_options.low_qual = vm["filter_low_qual"].as<int>();
            GLOG_INFO << "Set filter low qual" << filter_options.low_qual  ;
        } else {

        }
        if (vm.count("filter_qual_rate")) {
            filter_options.qual_rate = vm["filter_qual_rate"].as<float>();
            GLOG_INFO << "Set filter qual rate" << filter_options.qual_rate;
        } else {
            GLOG_INFO << "Default set filter qual rate" << filter_options.qual_rate;
        }
        if (vm.count("filter_n_nate")) {
            filter_options.n_base_rate = vm["filter_n_nate"].as<float>();
            GLOG_INFO << "Set filter n nate " << filter_options.n_base_rate;
        } else {
            GLOG_INFO << "Default set filter n nate " << filter_options.n_base_rate;
        }

        if (vm.count("filter_adapter1")) {
            filter_options.adapter1 = vm["filter_adapter1"].as<std::string>();
//            GLOG_INFO << "Set filter adapter1 " << filter_options.adapter1;
        } else {
//            GLOG_INFO << "Default set filter adapter1 " << filter_options.adapter1;
        }
        if (vm.count("filter_adapter2")) {
            filter_options.adapter2 = vm["filter_adapter2"].as<std::string>();
//            GLOG_INFO << "Set filter adapter2 " << filter_options.adapter2;
        }
        if (vm.count("filter_mis_match")) {
            filter_options.adapter_mis_match = vm["filter_mis_match"].as<int>();
            GLOG_INFO << "Set filter mis match " << filter_options.adapter_mis_match;
        }

        if (vm.count("filter_match_ratio")) {
            filter_options.adapter_match_ratio = vm["filter_match_ratio"].as<float>();
            GLOG_INFO << "Set filter match ratio " << filter_options.adapter_match_ratio;
        }

        if (vm.count("filter_qual_system")) {
            filter_options.qual_sys = vm["filter_qual_system"].as<int>() == 1? 33 : 64;
            GLOG_INFO << "Set filter quality system " << filter_options.qual_sys;
        }
        filter_options.filter_thread_num = nbsm_thread_num / 5 > 1 ? nbsm_thread_num / 5 : 1;
        GLOG_INFO << "filter options low qual set " << filter_options.low_qual  ;
        GLOG_INFO << "filter options filter adapter1 set " << filter_options.adapter1;
        GLOG_INFO << "filter options filter adapter2 set " << filter_options.adapter2;
        return 0;
    }


    int NBSMOptions::ParserBWAMEMParameter() {
        // bwa_mem parameter parse
        if (vm.count("bwamem_thread")) {
            mem_opt->n_threads = vm["bwamem_thread"].as<int>();
            GLOG_INFO << "Set bwa mem thread " << mem_opt->n_threads;
        } else {
            mem_opt->n_threads = nbsm_thread_num;
            GLOG_INFO << "Default set bwa mem thread num " << mem_opt->n_threads;
        }
        if (vm.count("bawmem_min_seed")) {
            mem_opt->min_seed_len = vm["bawmem_min_seed"].as<int>();
            GLOG_INFO << "Set bwa mem min seed " << mem_opt->min_seed_len;
        }
        if (vm.count("bwamem_band_width")) {
            mem_opt->w = vm["bwamem_band_width"].as<int>();
            GLOG_INFO << "Set bwa mem band wider " << mem_opt->w;
        }
        if (vm.count("bwamem_x_dropoff")) {
            mem_opt->zdrop = vm["bwamem_x_dropoff"].as<int>();
            GLOG_INFO << "Set bwa mem x drop off " << mem_opt->zdrop;
        }
        if (vm.count("bwamem_internal_seed")) {
            mem_opt->split_factor = vm["bwamem_internal_seed"].as<float>();
            GLOG_INFO << "Set bwa mem internal seeds " << mem_opt->split_factor;
        }
        if (vm.count("bwamem_skip_number")) {
            mem_opt->max_occ = vm["bwamem_skip_number"].as<int>();
            GLOG_INFO << "Set bwa mem skip seeds " << mem_opt->split_factor;
        }
        if (vm.count("bwamem_drop_chain")) {
            mem_opt->drop_ratio = vm["bwamem_drop_chain"].as<float>();
            GLOG_INFO << "Set bwa mem drop chain " << mem_opt->drop_ratio;
        }
        if (vm.count("bwamem_discard_len")) {
            mem_opt->min_chain_weight = vm["bwamem_discard_len"].as<int>();
            GLOG_INFO << "Set bwa mem discard len " << mem_opt->min_chain_weight;
        }

        if (vm.count("bwamem_max_rounds")) {
            mem_opt->max_matesw = vm["bwamem_max_rounds"].as<int>();
            GLOG_INFO << "Set bwa mem max round " << mem_opt->max_matesw;
        }

        if (vm.count("bwamem_skip_mate_rescue")) {
            mem_opt->flag |= MEM_F_NO_RESCUE;
            GLOG_INFO << "Set bwamem_skip_mate_rescue";
        }
        if (vm.count("bwamem_skip_pairing")) {
            mem_opt->flag |= MEM_F_NOPAIRING;;
            GLOG_INFO << "Set bwamem_skip_mate_rescue";
        }
        if (vm.count("bwamem_match_score")) {
            mem_opt->a = vm["bwamem_match_score"].as<int>();
            GLOG_INFO << "Set bwa mem match score" << mem_opt->a;
        }
        if (vm.count("bwamem_minmatch_score")) {
            mem_opt->b = vm["bwamem_minmatch_score"].as<int>();
            GLOG_INFO << "Set bwa mem mismatch score" << mem_opt->b;
        }
        if (vm.count("bwamem_gap_open_penalties")) {
            mem_opt->o_del = mem_opt->o_ins = vm["bwamem_gap_open_penalties"].as<int>();
            GLOG_INFO << "Set bwa mem gap open penalties" << mem_opt->o_del << "\t" << mem_opt->o_del;
        }
        if (vm.count("bwamem_gap_extension_score")) {
            mem_opt->e_del = mem_opt->e_ins = vm["bwamem_gap_extension_score"].as<int>();
            GLOG_INFO << "Set bwa mem gap extension penalties" << mem_opt->e_del << "\t" << mem_opt->e_del;
        }
        if (vm.count("bwamem_clipping_penalty")) {
            mem_opt->pen_clip5 = mem_opt->pen_clip5 = vm["bwamem_clipping_penalty"].as<int>();
            GLOG_INFO << "Set bwa mem clipping penalty " << mem_opt->pen_clip3 << "\t" << mem_opt->pen_clip5;
        }
        if (vm.count("bwamem_unpair_penalty")) {
            mem_opt->pen_unpaired = vm["bwamem_unpair_penalty"].as<int>();
            GLOG_INFO << "Set bwa mem unpair_penalty " << mem_opt->pen_unpaired;
        }
        return 0;
    }

    int NBSMOptions::ParserSortMkdupParameter() {
        if (vm.count("sm_sort_sharding_size")) {
            sm_options.sort_region_size = vm["sm_sort_sharding_size"].as<int>();
            GLOG_INFO << "Set sm sort sharding size " << sm_options.sort_region_size;
        }
        if (vm.count("sm_sort_block_size")) {
            sm_options.sort_block_size = vm["sm_sort_block_size"].as<int>();
            GLOG_INFO << "Set sm sort block size " << sm_options.sort_block_size;
        }
        if (vm.count("sm_mkdup_sharding_size")) {
            sm_options.markdup_region_size = vm["sm_mkdup_sharding_size"].as<int>();
            GLOG_INFO << "Set sm mark duplicate block size " << sm_options.markdup_region_size;
        }
        if (vm.count("sm_mkdup_block_size")) {
            sm_options.markdup_block_size = vm["sm_mkdup_block_size"].as<int>();
        }

        if (vm.count("sm_read_gam_thread_num")) {
            sm_options.read_gam_thread_num = vm["sm_read_gam_thread_num"].as<int>();
        } else {
            sm_options.read_gam_thread_num = nbsm_thread_num / 5 > 1 ? nbsm_thread_num / 5 : 1;
        }

        if (vm.count("sm_block_sort_thread")) {
            sm_options.block_sort_thread_num = vm["sm_block_sort_thread"].as<int>();
        } else {
            sm_options.block_sort_thread_num = nbsm_thread_num / 4 > 1 ? nbsm_thread_num / 4 : 1 ;
        }
        if (vm.count("sm_merge_thread")) {
            sm_options.merge_sort_thread_num = vm["sm_merge_thread"].as<int>();
        } else {
            sm_options.merge_sort_thread_num = nbsm_thread_num / 2 > 1 ? nbsm_thread_num / 2 : 1;
        }
        if (vm.count("sm_compress_bam_thread")) {
            sm_options.bam_output_thread_num = vm["sm_compress_bam_thread"].as<int>();
        } else {
            sm_options.bam_output_thread_num = nbsm_thread_num / 2 > 1 ? nbsm_thread_num / 2: 1;
        }
        if (vm.count("sm_mkdup_thread")) {
            sm_options.mark_dup_thread_num = vm["sm_mkdup_thread"].as<int>();
        } else {
            sm_options.mark_dup_thread_num = nbsm_thread_num;
        }
        sm_options.directory = temporary_directory;
        GLOG_INFO << "Set block sort thread number " << sm_options.block_sort_thread_num;
        GLOG_INFO << "Set intermediate  file" << sm_options.directory;
        GLOG_INFO << "Set sm mark duplicate block size " << sm_options.markdup_block_size;
        GLOG_INFO << "Set sharding stage sm  sort block thread num " << sm_options.block_sort_thread_num;
        GLOG_INFO << "Set merge stage merge thread num " << sm_options.block_sort_thread_num;
        GLOG_INFO << "Set merge stage bam compress thread num " << sm_options.bam_output_thread_num;
        GLOG_INFO << "Set mark duplicate thread num " << sm_options.mark_dup_thread_num;
        return 0;
    }

    int NBSMOptions::CheckHelp() {
        int status = 1;
        if (vm.count("help")) {
            std::cout << opt_des_;
            status = 0;
        } else if (vm.count("version")) {
            std::cout << "GAMTOOLS NBSM\tVersion: " << kNBSMVersion << std::endl;
            std::cout << "BWA MEM\tVersion: " << kBWAMEMVersion << std::endl;
            status = 0;
        } else {
            status = 1;
        }
        return status;
    }




}