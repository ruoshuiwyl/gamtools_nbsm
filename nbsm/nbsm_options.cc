//
// Created by ruoshui on 5/14/18.
//

#include "nbsm_options.h"

namespace po = boost::program_options;
namespace gamtools {
    NBSMOptions::NBSMOptions(): opt_des_("GAMTOOLS NBSM Version:V0.1.0"),
                                base_des_("Basic SmOptions"),
                                filter_des_("Filter SmOptions"),
                                bwamem_des_("Bwa_mem SmOptions"),
                                sort_mkdup_des_("Sort & Mark Duplicate SmOptions"){
        base_des_.add_options()
                // base options
                ("help,H", "produce help message")
                ("version,V", "print version message")
                ("reference_file,R", po::value<std::string>(&reference_file_), "Reference sequence file Default(null)")
                ("temp_dir,D", po::value<std::string>(&temporary_directory_), "Temporary directory Storage space size must be twice BAM file size Default(null)")
                ("input_fastq1_lists,F", po::value<std::vector<std::string>>(&input_fastq1_lists_), "Input fastq1 file lists")
                ("input_fastq2_lists,R", po::value<std::vector<std::string>>(&input_fastq2_lists_), "Input fastq2 file lists")
                ("input_library_id_lists,B",po::value<std::vector<int>>(&input_library_ids_), "Input fastq file library ID" )
                ("input_lane_id_lists,L",   po::value<std::vector<int>>(&input_lane_ids_),      "Input fastq file lane ID")
                ("ouput_bam_file,O", po::value<std::string>(&output_bam_file_), "Output bam file name defalut(null)")
                ("sample_name", boost::program_options::value<std::string>(&sample_name_)->default_value("Zebra"), "Sample Name Default(Zebra)")
                ("sample_id", boost::program_options::value<std::string>(&sample_id_)->default_value("Zebra"), "Sample Name Default(Zebra)")
                ("thread_number,T", boost::program_options::value<int>(&nbsm_thread_num)->default_value(20),"NBSM total thread Default(20)")


            // filter fastq options
            filter_des_.add_options()
                    ("filter_low_qual", po::value<int>(), "Filter Read low quality threshold Default(10)")
                    ("filter_qual_rate", po::value<float>(),"Filter Read low quality rate Default(0.5")
                    ("filter_n_nate",  po::value<float>(), "Filter Read N rate threshold Default (0.05)")
                    ("filter_adapter1", po::value<std::string>(), "3' adapter sequence Default(AAGTCGGAGGCCAAGCGGTCTTAGGAAGACAA)")
                    ("filter_adapter2", po::value<std::string>(), "5' adapter sequence Default(AAGTCGGATCGTAGCCATGTCGTTCTGTGAGCCAAGGAGTTG)")
                    ("filter_mis_match", po::value<int>(), "the max mismatch number when match the adapter Default(1)")
                    ("filter_match_ratio", po::value<float>(), "adapter's shortest match ratio Default(0.5)")
                    ("filter_qual_system", po::value<int>(), "quality system 1:illumina, 2:sanger Default(2:illumina)")
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
                    ("bwamem_unpair_penalty", po::value<int>(), "penalty for an unpaired read pair Default(17)")

        sort_mkdup_des_.add_options()
                ("sm_sort_sharding_size", po::value<int>(), "Sort Sharding size Default(1M)")
                ("sm_sort_block_size", po::value<int>(), "sort region buffer size to save gam record Default(1M)")
                ("sm_mkdup_sharding_size", po::value<int>(), "Mark Duplicate Sharding size Default(8M)")
                ("sm_mkdup_block_size", po::value<int>(), "Mark Duplicate block buffer size for to save mkdup info Default(1M)")
                ("sm_block_sort_thread", po::value<int>(), "sharding stage block sort thread number Default(4)")
                ("sm_merge_thread", po::value<int>(), "Merge sort thread num Default(4)")
                ("sm_compress_bam_thread", po::value<int>(), "Compress bam thread number Default(4)")
                ("sm_mkdup_thread", po::value<int>(), "Mark Duplicate thread number Default (4)")
                opt_des_.add(base_des_).add(filter_des_).add(bwamem_des_).add(sort_mkdup_des_);
                ;
    }

    void NBSMOptions::ParserCommandLine(int argc, char **argv) {

    }




}