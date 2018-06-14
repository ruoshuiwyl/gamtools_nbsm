//
// Created by ruoshui on 6/14/18.
//

#include <tuple>
#include <util/glogger.h>
#include "qc_stat.h"

namespace gamtools {

    const static std::vector<std::string> kChromosomeName = {
            "chr1",  "chr2",  "chr3",  "chr4",  "chr5",
            "chr6",  "chr7",  "chr8",  "chr9",  "chr10",
            "chr11", "chr12", "chr13", "chr14", "chr15",
            "chr16", "chr17", "chr18", "chr19", "chr20",
            "chr21", "chr22", "chrX", "chrY" };

    void BaseStat::ReadReferIndex() {

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
        for (auto &chr_name : kChromosomeName) {
            chr_idx = refer_dict_[chr_name];
            stat_chr_.insert(chr_idx);
        }
    }



    void TargetStat::Init() {
        BaseStat::ReadReferIndex();
        int chr_idx;
        char *data = new char[1024];
        char chr_name[64];
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
            bed_ifs.close();
        } else {
            GLOG_ERROR << "Load bed file  *.bed Error";
        }
        delete [] data;
    }



    void TargetStat::StatisticsRead(const StatisticsSlice &stat) {
        if (stat.tid > target_read_idx_) {
            target_read_idx_ = stat.tid;
            target_read_reg_ = 0;
        }
        int overlap_end = stat.pos + stat.rlen;
        while (std::get<1>(target_region_[target_read_idx_][target_read_reg_]) <= stat.pos) {
            target_read_reg_++;
        }
        bed_t &target_bed = target_region_[target_read_idx_][target_read_reg_];
        if (!(std::get<0>(target_bed) >= overlap_end || std::get<1>(target_bed) <= stat.pos)) {
            target_reads_[target_read_idx_]++;
            target_bases_[target_read_idx_] += stat.qlen;
            if (stat.mapq >= kMapq ) {
                mapq10_target_reads_[target_read_idx_]++;
                mapq10_target_bases_[target_read_idx_] +=  stat.qlen;
            }
        }
        while (std::get<1>(target_region_[flank_read_idx_][flank_read_reg_]) <= stat.pos) {
            flank_read_reg_++;
        }
        bed_t &flank_bed = flank_region_[target_read_idx_][target_read_reg_];
        if (!(std::get<0>(flank_bed) >= overlap_end || std::get<1>(flank_bed) <= stat.pos)) {
            flank_reads_[target_read_idx_]++;
            flank_bases_[target_read_idx_] += stat.qlen;
            if (stat.mapq >= kMapq) {
                mapq10_flank_reads_[target_read_idx_]++;
                mapq10_flank_bases_[target_read_idx_] +=  stat.qlen;
            }
        }

    }

    void TargetStat::StatisticsDepth(int tid, int pos, int depth) {
        if (tid > target_depth_idx_) {
            target_depth_idx_ = tid;
            target_depth_reg_ = 0;
            flank_depth_idx_ = tid;
            flank_depth_reg_ = 0;
        }
        while (std::get<1>(target_region_[target_depth_idx_][target_depth_reg_]) <= pos) {
            target_depth_reg_++;
        }
        bed_t &target_bed = target_region_[target_read_idx_][target_read_reg_];
        if (std::get<0>(target_bed) >= pos && std::get<1>(target_bed) < pos) {
            target_depth_[target_depth_idx_][depth >= kMaxDepth? kMaxDepth - 1: depth] ++;
            if (depth > 0 ) {
                target_coverage_[target_depth_idx_]++;
                target_depth_stat_[target_depth_idx_] += depth;
            }
        }
        while (std::get<1>(flank_region_[target_depth_idx_][target_depth_reg_]) <= pos) {
            target_depth_reg_++;
        }
        bed_t &flank_bed = flank_region_[flank_depth_idx_][flank_depth_reg_];
        if (std::get<0>(target_bed) >= pos && std::get<1>(target_bed) < pos) {
            flank_depth_[flank_depth_idx_][depth >= kMaxDepth? kMaxDepth - 1: depth] ++;
            if (depth > 0 ) {
                flank_coverage_[flank_depth_idx_]++;
                flank_depth_stat_[flank_depth_idx_] += depth;
            }
        }

    }



    std::string TargetStat::Report() {

        std::ostringstream oss;

        oss << "\t\t\t\t\tTarget" << std::endl;




        oss << "\t\t\t\t\tChromosome Depth" << std::endl;
        oss << "Chromosome\tSize\tTotalBase\tCoverPosition\tCoveragePercent\tMeanDepth\tRelativeDepth\tMedianDepth" << std::endl;
        for (auto &chr : kChromosomeName) {
            int chr_idx = refer_dict_[chr];
            int lens;
            int bases;
            int cover_pos;
            double cover_precent;
            double mean_depth;
            double relative_depth;
            int median_depth;
            oss << chr << "\t" << lens << "\t" << bases << "\t" << cover_pos << "\t" << cover_precent << "\t" ;
            oss << mean_depth << "\t" << relative_depth << "\t" << median_depth << std::endl;
        }

        oss << "\t\t\t\t\tDepth Staticstic" << std::endl;
        oss << "Depth\t TRPercent\t FlankPercent\t TotalPercent" << std::endl;
        for (int i = 0; i < kMaxDepth; ++i) {
            double target_radio;
            double flank_radio;
            double total_radio;
            oss << i << "\t" << target_radio << "\t" << flank_radio << "\t" << total_radio << std::endl;
        }


        return oss.str();
    }



    void WGSStat::Init() {
        BaseStat::ReadReferIndex();
    }

    void WGSStat::StatisticsDepth(int tid, int pos, int depth) {
        depth_stat_[tid] += depth;
        target_coverage_[tid]++;
        target_depth_[tid][depth >= kMaxDepth? kMaxDepth - 1 : depth]++;
    }

    void WGSStat::StatisticsRead(const gamtools::StatisticsSlice &stat) {
        target_reads_[stat.tid]++;
        target_bases_[stat.tid] += stat.qlen;
        if (stat.mapq >= kMapq) {
            mapq10_target_reads_[stat.tid]++;
            mapq10_target_bases_[stat.tid] += stat.qlen;
        }
    }
    std::string WGSStat::Report() {



    }

}