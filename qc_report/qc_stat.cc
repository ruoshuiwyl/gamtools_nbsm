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
            "chr21", "chr22", "chrX",  "chrY" };

    void BaseStat::ReadReferIndex() {
        char data[1024];
        char chr_name[64];
        int chr_len;
        int chr_idx = 0;
        std::ifstream ref_ifs(ref_filename_);
        if (ref_ifs.is_open()){
            while (ref_ifs.getline(data, 1024) ) {
                sscanf(data, "%s %d", chr_name, &chr_len);
                refer_dict_[chr_name] = chr_idx;
                refer_lens_. push_back(chr_len);
                chr_idx++;
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

    void BaseStat::BaseStatisticsRead(const StatisticsSlice &stat) {
        total_reads_++;
        if (stat.tid >= 0) {
            mapped_reads_++;
            mapped_bases_ += stat.qlen;
            if (stat.mapq >= 10) {
                mapq10_mapped_reads_++;
                mapq10_mapped_bases_ += stat.qlen;
            }
            if (stat.tid == chrx_idx_) {
                chrx_depth_ += stat.qlen;
            }
            if (stat.tid == chry_idx_) {
                chry_depth_ += stat.qlen;
            }
        }
        if (stat.is_dup) {
            dup_reads_++;
        }


    }



    TargetStat::TargetStat(const std::string &ref_idx_file, const std::string &bed_file)
            : BaseStat(ref_idx_file), bed_filename_(bed_file) {

    }




    void TargetStat::Init() {
        BaseStat::ReadReferIndex();
        int genome_size = refer_dict_.size();
        target_reads_.resize(genome_size, 0);
        flank_reads_.resize(genome_size, 0);
        mapq10_target_reads_.resize(genome_size, 0);
        mapq10_flank_reads_.resize(genome_size, 0);


        target_bases_.resize(genome_size, 0);
        flank_bases_.resize(genome_size, 0);
        mapq10_target_bases_.resize(genome_size, 0);
        mapq10_flank_bases_.resize(genome_size, 0);

        target_depth_.resize(genome_size);
        flank_depth_.resize(genome_size);
        for (int i = 0; i < genome_size; ++i) {
            target_depth_[i].resize(kMaxDepth, 0);
            flank_depth_[i].resize(kMaxDepth, 0);
        }

        target_coverage_.resize(genome_size, 0);
        flank_coverage_.resize(genome_size, 0);

        target_depth_stat_.resize(genome_size, 0);
        flank_depth_stat_.resize(genome_size, 0);
        ReadBedFile();

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
        std::vector<double> target_depth_radio;
        std::vector<double> flank_depth_radio;
        std::vector<double> total_depth_radio;
        ComputeDepthStat(target_depth_radio, flank_depth_radio, total_depth_radio);
        std::ostringstream oss;
        oss << "\t\t\t\t\tMapping" << std::endl;
        oss << "Capture specificity(%)\t" << std::setprecision(5) << (double)target_total_reads_/ mapped_reads_ <<  std::endl;
        oss << "Capture specificity(mapq > 10) (%)\t" << std::setprecision(5) << (double)mapq10_target_total_reads_/ mapq10_mapped_reads_  << std::endl;
        oss << "Bases mapped to genome\t" << mapped_bases_ <<  std::endl;
        oss << "Reads mapped to genome\t" << mapped_reads_ << std::endl;
        oss << "Bases mapped (mapq >= 10) to genome\t" << mapq10_mapped_bases_ <<  std::endl;
        oss << "Reads mapped (mapq >= 10) to genome\t" << mapq10_mapped_reads_ << std::endl;
        oss << "mapq >= 10 rate(%)\t" << std::setprecision(4)<< mapq10_mapped_reads_ /mapped_reads_<< std::endl;
        double chrx_median = chrx_depth_/ chrx_total_len_;
        double chry_median = chry_depth_/ chry_total_len_;
        oss << "Mean depth of chrX(X)\t" << chrx_median<< std::endl;
        oss << "Mean depth of chrY(X)\t" << chry_median<< std::endl;
        oss << "Gender\t" << (chry_median/ chrx_median >= 0.5 ? 'M' : 'W' )<< std::endl;
        oss << "Duplicate rate(%d)\t" << std::setprecision(4) << (double)dup_reads_/ total_reads_ << std::endl;
//        oss << "GC(%)" << std::endl;
        std::vector<int> depth_stat = {4, 10, 20, 30, 40, 50, 60, 70, 80, 90, 100};
        oss << "\t\t\t\t\tTarget" << std::endl;
        oss << "Bases in target region(bp)\t " << target_total_len_ << std::endl;
        oss << "Reads mapped to target region\t" << target_total_len_ << std::endl;
        oss << "Bases mapped to target region\t" << target_total_len_ << std::endl;
        oss << "Reads mapped (mapq >= 10 )to target region\t" << target_total_len_ <<std::endl;
        oss << "Bases mapped (mapq >= 10)to target region\t" << target_total_len_ <<std::endl;
        oss << "Mean depth of target region(X)\t" << std::setprecision(5) << target_total_len_ <<std::endl;
        oss << "Coverage of target region(%)\t" << std::setprecision(4) <<  target_total_len_ << std::endl;

        for (auto depth :depth_stat ) {
            oss << "Fraction of target region covered >=" << depth << "X(%)\t" << target_depth_radio[depth] << std::endl;
        }

        oss << "\t\t\t\t\tFlank" << std::endl;
        oss << "Bases in flanking region(bp)\t " << target_total_len_ << std::endl;
        oss << "Reads mapped to flanking region\t" << target_total_len_ << std::endl;
        oss << "Bases mapped to flanking region\t" << target_total_len_ << std::endl;
        oss << "Reads mapped (Mapq >= 10 )to flanking region\t" << target_total_len_ <<std::endl;
        oss << "Bases mapped (Mapq >= 10)to flanking region\t" << target_total_len_ <<std::endl;
        oss << "Mean depth of flanking region(X)\t" << std::setprecision(5) << target_total_len_ <<std::endl;
        oss << "Coverage of flanking region(%)\t" << std::setprecision(4) <<  target_total_len_ << std::endl;

        for (auto depth : depth_stat ) {
            oss << "Fraction of flanking region covered >=" << depth << "X(%)\t" << target_depth_radio[depth] << std::endl;
        }


        oss << "\t\t\t\t\tChromosome Depth" << std::endl;
        oss << "Chromosome\tSize\tTotalBase\tCoverPosition\tCoveragePercent\tMeanDepth\tRelativeDepth\tMedianDepth" << std::endl;
        for (auto &chr : kChromosomeName) {
            int chr_idx = refer_dict_[chr];
            ChromosomeStatData stat_data ;
            ComputeChrStat(chr_idx, stat_data);
            oss << chr << "\t" << stat_data.lens << "\t" << stat_data.bases << "\t" << stat_data.cover_pos << "\t" ;
            oss << std::setprecision(4)<< stat_data.cover_precent << "\t"<< stat_data.mean_depth << "\t" <<
                stat_data.relative_depth << "\t" << stat_data.median_depth << std::endl;
        }
        oss << std::endl;
        oss << "\t\t\t\t\tDepth Staticstic" << std::endl;
        oss << "Depth\t TRPercent\t FlankPercent\t TotalPercent" << std::endl;
        oss << "0\t 100\t 100\t 100" << std::endl;

        for (int idx = 1; idx < kMaxDepth; ++idx) {
            if (target_depth_radio[idx] > 10.0) {
                oss << idx << "\t" << std::setprecision(4) << target_depth_radio[idx] << "\t" << flank_depth_radio[idx] << "\t" << total_depth_radio[idx] << std::endl;
            }
        }

        return oss.str();
    }




    void TargetStat::ReadBedFile() {
        int chr_idx;
        char *data = new char[1024];
        char chr_name[64];
        std::ifstream bed_ifs(bed_filename_);
        target_region_.resize(refer_dict_.size());
        flank_region_.resize(refer_dict_.size());
        target_chr_lens_.resize(refer_dict_.size());
        flank_chr_lens_.resize(refer_dict_.size());
        target_total_len_ = 0;
        flank_total_len_ = 0;
        if (bed_ifs.is_open()) {
            while(bed_ifs.getline(data, 1024)) {
                int target_start, target_stop;
                sscanf(data, "%s %d %d", chr_name, target_start, target_stop);
                chr_idx = refer_dict_[chr_name];
                int target_len = target_stop - target_start;
                bed_t bed_region = std::make_tuple(target_start, target_stop, target_len);
                target_region_[chr_idx].push_back(bed_region);
                target_chr_lens_[chr_idx] += target_len;
                target_total_len_ += target_len;
                int flank_start = target_start - flank_extend >=  0 ? target_start - flank_extend : 0;
                int flank_stop = target_stop + flank_extend <  refer_lens_[chr_idx]? target_stop + flank_extend : refer_lens_[chr_idx];
                int flank_len = flank_stop - flank_start;
                auto flank_region = std::make_tuple(flank_start, flank_stop, flank_len);
                flank_region_[chr_idx].push_back(flank_region);
                flank_chr_lens_[chr_idx] += flank_len;
                flank_total_len_ += flank_len;
            }
            bed_ifs.close();
        } else {
            GLOG_ERROR << "Load bed file  *.bed Error";
        }
        delete [] data;
    }

    void TargetStat::ComputeDepthStat(std::vector<double> &target_depth_radio, std::vector<double> &flank_depth_radio,
                                          std::vector<double> &total_depth_radio) {
        std::vector<int64_t> target_depth;
        std::vector<int64_t> flank_depth;
        target_depth.resize(kMaxDepth, 0);
        flank_depth.resize(kMaxDepth, 0);
        for (int idx = kMaxDepth - 1; idx >= 0; --idx) {
            if (idx < kMaxDepth - 1) {
                target_depth[idx] += target_depth[idx + 1];
                flank_depth[idx] += flank_depth[idx + 1];
            }
            for (auto chr_idx : stat_chr_) {
                target_depth[idx] += target_depth_[chr_idx][idx];
                flank_depth[idx] += flank_depth_[chr_idx][idx];
            }
        }
        target_depth_radio.resize(kMaxDepth);
        flank_depth_radio.resize(kMaxDepth);
        total_depth_radio.resize(kMaxDepth);
        int target_total_len;
        int flank_total_len;
        for (int idx = 1; idx < kMaxDepth; ++idx) {
            target_depth_radio[idx] = (double) target_depth[idx]/ target_total_len;
            flank_depth_radio[idx]  = (double) flank_depth[idx] / flank_total_len;
            total_depth_radio[idx] = (double)(target_depth[idx] + flank_depth[idx]) / (target_total_len + flank_total_len );
        }
    }

    void TargetStat::ComputeChrStat(int chr_idx, ChromosomeStatData &stat_data) {
        stat_data.lens = target_chr_lens_[chr_idx];
        stat_data.bases = target_bases_[chr_idx];
        stat_data.cover_pos = target_coverage_[chr_idx];
        stat_data.cover_precent = (double)stat_data.cover_pos / stat_data.lens;
        stat_data.mean_depth = (double)target_depth_stat_[chr_idx] / stat_data.lens;
        stat_data.relative_depth = (double)target_depth_stat_[chr_idx]/target_total_len_;
        auto &depth_dist = target_depth_[chr_idx];
        int64_t midian = target_depth_stat_[chr_idx] >> 1;
        int64_t cnt = 0, next_cnt = 0;
        for (int idx = 0; idx < kMaxDepth; ++idx) {
            next_cnt += depth_dist[idx];
            if (next_cnt >= midian && midian > cnt) {
                stat_data.median_depth = idx;
                break;
            }
            cnt = next_cnt;
        }

    }


    WGSStat::WGSStat(const std::string &ref_idx_file) : BaseStat(ref_idx_file){}


    void WGSStat::Init() {
        BaseStat::ReadReferIndex();
        int genome_size = refer_dict_.size();
        target_coverage_.resize(genome_size, 0);
        depth_stat_.resize(genome_size, 0);
        target_reads_.resize(genome_size, 0);
        target_bases_.resize(genome_size, 0);
        mapq10_target_reads_.resize(genome_size, 0);
        mapq10_target_bases_.resize(genome_size, 0);
        target_depth_.resize(genome_size);
        for (int i = 0; i < genome_size; ++i) {
            target_depth_[i].resize(kMaxDepth, 0);
        }
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
        std::ostringstream oss;
        oss << "\t\t\t\t\tMapping Statistics" << std::endl;
        oss << "Capture specificity(%)\t" << std::setprecision(5) << (double)target_mapped_bases_ / mapped_reads_ <<std::endl;
        oss << "Capture specificity mapq >= 10 (%)\t" << std::setprecision(5) << (double)mapq10_target_mapped_reads_ /mapq10_mapped_reads_ <<  std::endl;
        oss << "Bases mapped to genome\t" << target_mapped_reads_ << std::endl;
        oss << "Reads mapped to genome\t" << target_mapped_bases_ << std::endl;
        oss << "Bases mapped (mapq >= 10) to genome\t" << mapq10_target_mapped_reads_ <<  std::endl;
        oss << "Reads mapped (mapq >= 10) to genome\t" << mapq10_target_mapped_bases_ <<  std::endl;
        oss << "mapq >= 10 rate(%)\t" << mapq10_target_mapped_reads_ / target_mapped_reads_ << std::endl;
        double chrx_median = chrx_depth_/ chrx_total_len_;
        double chry_median = chry_depth_/ chry_total_len_;
        oss << "Mean depth of chrX(X)\t" << chrx_median<< std::endl;
        oss << "Mean depth of chrY(X)\t" << chry_median<< std::endl;
        oss << "Gender\t" << (chry_median/ chrx_median >= 0.5 ? 'M' : 'W' )<< std::endl;
        oss << "Duplicate rate(%d)\t" << std::setprecision(4) << (double)dup_reads_/ total_reads_  << std::endl;
//        oss << "GC(%)" << std::endl;
        std::vector<int> depth_stat = {4, 10, 20, 30, 40, 50, 60, 70, 80, 90, 100};
        oss << "\t\t\t\t\tTarget" << std::endl;
        oss << "Bases in target region(bp)\t " << target_total_lens_ << std::endl;
        oss << "Reads mapped to target region\t" << target_mapped_reads_ << std::endl;
        oss << "Bases mapped to target region\t" << target_mapped_bases_ << std::endl;
        oss << "Reads mapped (mapq >= 10 )to target region\t" << mapq10_target_mapped_reads_ <<std::endl;
        oss << "Bases mapped (mapq >= 10)to target region\t" << mapq10_target_mapped_bases_ <<std::endl;
        oss << "Mean depth of target region(X)\t" << std::setprecision(5) << (double)(total_depth_/target_total_lens_)  <<std::endl;
        oss << "Coverage of target region(%)\t" << std::setprecision(4) <<  (double)(total_coverage_/target_total_lens_) << std::endl;
        std::vector<double> target_depth_radio;

        for (auto depth :depth_stat ) {
            oss << "Fraction of target region covered >=" << depth << "X(%)\t" << std::setprecision(4) <<  target_depth_radio[depth] << std::endl;
        }
        oss << "\t\t\t\t\tChromosome Depth" << std::endl;
        oss << "Chromosome\tSize\tTotalBase\tCoverPosition\tCoveragePercent\tMeanDepth\tRelativeDepth\tMedianDepth" << std::endl;
        for (auto &chr : kChromosomeName) {
            int chr_idx = refer_dict_[chr];
            ChromosomeStatData stat_data ;
            ComputeChrStat(chr_idx, stat_data);
            oss << chr << "\t" << stat_data.lens << "\t" << stat_data.bases << "\t" << stat_data.cover_pos << "\t" ;
            oss << std::setprecision(4)<< stat_data.cover_precent << "\t"<< stat_data.mean_depth << "\t" <<
                stat_data.relative_depth << "\t" << stat_data.median_depth << std::endl;
        }
        oss << std::endl;
        oss << "\t\t\t\t\tDepth Staticstic" << std::endl;
        oss << "Depth\t TRPercent\t FlankPercent\t TotalPercent" << std::endl;
        oss << "0\t 100\t 100\t 100" << std::endl;
        for (int idx = 1; idx < kMaxDepth; ++idx) {
            if (target_depth_radio[idx] > 10.0) {
                oss << idx << "\t" << std::setprecision(4) << target_depth_radio[idx] << std::endl;
            }
        }


    }

    void WGSStat::ComputeChrStat(int chr_idx, gamtools::ChromosomeStatData &stat_data) {
        stat_data.lens = refer_lens_[chr_idx];
        stat_data.bases = target_bases_[chr_idx];
        stat_data.cover_pos = target_coverage_[chr_idx];
        stat_data.cover_precent = (double)stat_data.cover_pos / stat_data.lens;
        stat_data.mean_depth = (double)depth_stat_[chr_idx] / stat_data.lens;
        stat_data.relative_depth = (double)depth_stat_[chr_idx]/target_total_lens_;
        auto &depth_dist = target_depth_[chr_idx];
        int64_t midian = depth_stat_[chr_idx] >> 1;
        int64_t cnt = 0, next_cnt = 0;
        for (int idx = 0 ; idx < kMaxDepth; ++idx) {
            next_cnt += depth_dist[idx];
            if (next_cnt >= midian && midian > cnt) {
                stat_data.median_depth = idx;
                break;
            }
            cnt = next_cnt;
        }
    }

    void WGSStat::ComputeDepthStat(std::vector<double> &target_depth_radio) {
        std::vector<int64_t> target_depth;
        target_depth.resize(kMaxDepth, 0);
        for (int idx = kMaxDepth - 1; idx >= 0; --idx) {
            if (idx < kMaxDepth - 1) {
                target_depth[idx] += target_depth[idx + 1];
            }
            for (auto chr_idx : stat_chr_) {
                target_depth[idx] += target_depth_[chr_idx][idx];
            }
        }
        target_depth_radio.resize(kMaxDepth);
        for (int idx = 0; idx < kMaxDepth; ++idx) {
            target_depth_radio[idx] = (double) target_depth[idx]/ target_total_lens_;
        }
    }

}