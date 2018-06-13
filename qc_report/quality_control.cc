//
// Created by ruoshui on 6/13/18.
//

#include <fstream>
#include <util/glogger.h>
#include "quality_control.h"




namespace gamtools {

    const static std::vector<std::string> kChromosomeName = {
            "chr1",  "chr2",  "chr3",  "chr4",  "chr5",
            "chr6",  "chr7",  "chr8",  "chr9",  "chr10",
            "chr11", "chr12", "chr13", "chr14", "chr15",
            "chr16", "chr17", "chr18", "chr19", "chr20",
            "chr21", "chr22", "chrX", "chrY" };
    void QualityControl::Init() {
        char data[1024];
        char chr_name[64];
        int chr_len;
        int chr_idx = 0;
        std::ifstream ref_ifs(ref_filename_);
        if (ref_ifs.is_open()){
            while (ref_ifs.getline(data, 1024) ) {
                sscanf(data, "%s %d", chr_name, chr_len);
                refer_dict_[chr_name] = chr_idx;
                refer_lens_[chr_idx++] = chr_len;
            }
            ref_ifs.close();
        } else {
            GLOG_ERROR << "Load Reference file index *.fai Error";
        }

        if (target_) {
            std::ifstream bed_ifs(bed_filename_);
            if (bed_ifs.is_open()) {
                while(bed_ifs.getline(data, 1024)) {
                    int target_start, target_stop;
                    sscanf(data, "%s %d %d", chr_name, target_start, target_stop);
                    chr_idx = refer_dict_[chr_name];
                    int target_len = target_stop - target_start;
                    bed_t bed_region = std::make_tuple(target_start, target_stop, target_len);
                    target_region_[chr_idx].push_back(bed_region);
                    target_chr_lens_[chr_idx] += target_len;
                    int flank_start = target_start - flank_extend >=  0 ? target_start - flank_extend : 0;
                    int flank_stop = target_stop + flank_extend <  refer_lens_[chr_idx]? target_stop + flank_extend : refer_lens_[chr_idx];
                    int flank_len = flank_stop - flank_start;
                    auto flank_region = std::make_tuple(flank_start, flank_stop, flank_len);
                    flank_region_[chr_idx].push_back(flank_region);
                    flank_chr_lens_[chr_idx] += flank_len;
                }

            } else {
                GLOG_ERROR << "Load bed file  *.bed Error";
            }
        } else {
            for (auto &chr_name : kChromosomeName) {
                chr_idx = refer_dict_[chr_name];
                auto bed_region = std::make_tuple(0, refer_lens_[chr_idx], refer_lens_[chr_idx]);
                target_region_[chr_idx].push_back(bed_region);
                target_chr_lens_[chr_idx] = refer_lens_[chr_idx];
            }
        }
    }



    void QualityControl::Statistics(const gamtools::Slice &slice) {
        StatisticsSlice stat(slice);
        if (!target_) {
            if (stat_chr_.count(stat.tid) != 0) {
                if (stat.tid == curr_idx_ && stat.pos == curr_pos_) {

                } else if (stat.pos > curr_pos_) {

                } else if (stat.tid > curr_idx_) {

                } else { // ERROR

                }
            }

        } else {
            if (stat_chr_.count(stat.tid) != 0) {
                if (stat.tid == curr_idx_ && stat.pos == curr_pos_) {

                } else if (stat.pos > curr_pos_) {

                } else if (stat.tid > curr_idx_) {

                } else { // ERROR

                }
            }


        }
    }

}