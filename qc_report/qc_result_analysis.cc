//
// Created by ruoshui on 6/25/18.
//

#include "qc_result_analysis.h"

#include <sstream>
#include <iomanip>

namespace gamtools {


    QCResultAnalysis::QCResultAnalysis(const QCBaseData &qc_base_data)
            : qc_base_data_(qc_base_data),
              target_(qc_base_data.target) {
        int total_chr = qc_base_data_.refer_dict.size();
        flank_results_.resize(total_chr + 1);
        target_results_.resize(total_chr + 1);
        mapping_results_.resize(total_chr + 1);
        for (int i = 0; i <= total_chr; ++i) {
            flank_results_[i].reset(i);
            target_results_[i].reset(i);
            mapping_results_[i].reset(i);
        }
    }

    void QCResultAnalysis::AddFlankQCResult(const gamtools::QCStatResult &qc_flank) {
        if (qc_flank.tid >= 0) {
            flank_results_[qc_flank.tid].Add(qc_flank);
        }
    }

    void QCResultAnalysis::AddTargetQCResult(const gamtools::QCStatResult &qc_target) {
        if (qc_target.tid >= 0) {
            target_results_[qc_target.tid].Add(qc_target);
        }
    }

    void QCResultAnalysis::AddMappingResult(const gamtools::QCMappingResult &map_result) {
        if (map_result.tid >= 0) {
            mapping_results_[map_result.tid].Add(map_result);
        } else {
            unmapping_result_.Add(map_result);
        }
    }

    std::string QCResultAnalysis::Report() {
        Analysis();
        std::string result;
        result += MappingResultReport();
        result += TargetResultReport();
        if (target_) {
            result += FlankResultReport();
        }
        result += ChromosomeResultReport();
        result += DepthResultReport();
        return result;
    }

    void QCResultAnalysis::Analysis() {
        AnalysisMappingResult();
        AnalysisTargetResult();
        if (target_) {
            AnalysisFlankResult();
        }
        AnalysisChromesomeResult();
        AnalysisDepthResult();
    }

    void QCResultAnalysis::AnalysisMappingResult() {
        for (auto &result : mapping_results_) {
            mapping_result_.Add(result);
        }
    }

    void QCResultAnalysis::AnalysisFlankResult() {
        for (auto &result : flank_results_) {
            flank_result_.Add(result);
        }
    }

    void QCResultAnalysis::AnalysisTargetResult() {
        for (auto &result : target_results_) {
            target_result_.Add(result);
        }
    }

    void QCResultAnalysis::AnalysisChromesomeResult() {
        for (int chr_idx = 0; chr_idx < qc_base_data_.refer_dict.size(); ++chr_idx) {
            ChromosomeStatResult chr_stat_result;
            chr_stat_result.chr_name = qc_base_data_.refer_names[chr_idx];
            if (qc_base_data_.target_chr_lens[chr_idx] > 0 ) {
                chr_stat_result.lens = qc_base_data_.target_chr_lens[chr_idx];
                chr_stat_result.bases = target_results_[chr_idx].bases_num;
                chr_stat_result.cover_pos = target_results_[chr_idx].coverage_pos;
                chr_stat_result.cover_precent = (double) chr_stat_result.cover_pos / chr_stat_result.lens;
                chr_stat_result.mean_depth = (double) target_results_[chr_idx].depth / chr_stat_result.lens;
                chr_stat_result.relative_depth = (double)  chr_stat_result.mean_depth / (target_result_.depth / qc_base_data_.target_total_lens);
                int64_t cnt = 0;

                for (int idx = 1; idx < kMaxDepth; ++idx) {
                    cnt += target_results_[chr_idx].depth_dist[idx];
                }
                int64_t depth0_cnt = qc_base_data_.target_chr_lens[chr_idx] - cnt;
                int median = (cnt + depth0_cnt) >> 1;
                cnt = 0;
                int64_t pre_cnt = depth0_cnt;
                for (int idx = 1; idx < kMaxDepth; ++idx) {
                    cnt += target_results_[chr_idx].depth_dist[idx];
                    if (pre_cnt <= median && median < cnt) {
                        chr_stat_result.median_depth = idx;
                        break;
                    }
                    pre_cnt = cnt;
                }
                chr_stat_results_.push_back(chr_stat_result);
            }
        }
    }

    void QCResultAnalysis::AnalysisDepthResult() {

        std::vector<int64_t> target_depth;
        std::vector<int64_t> flank_depth;
        target_depth.resize(kMaxDepth, 0);
        flank_depth.resize(kMaxDepth, 0);
        for (int chr_idx = 0; chr_idx< qc_base_data_.refer_dict.size(); ++chr_idx) {
            int64_t temp = 0;
            int64_t flank_temp = 0;
            for (int idx = kMaxDepth - 1; idx >= 0; --idx) {
                temp += target_results_[chr_idx].depth_dist[idx];
                target_depth[idx] += temp;
                if (target_) {
                    flank_temp += flank_results_[chr_idx].depth_dist[idx];
                    flank_depth[idx] += flank_temp;
                }
            }
        }
        target_depth_radio.resize(kMaxDepth);
        if (target_) {
            flank_depth_radio.resize(kMaxDepth);
            total_depth_radio.resize(kMaxDepth);
        }
        for (int idx = 1; idx < kMaxDepth; ++idx) {
            target_depth_radio[idx] = (double) target_depth[idx] / qc_base_data_.target_total_lens;
            if (target_) {
                flank_depth_radio[idx] = (double) flank_depth[idx] / qc_base_data_.flank_total_lens;
                total_depth_radio[idx] = (double) (target_depth[idx] + flank_depth[idx]) /
                        (qc_base_data_.target_total_lens + qc_base_data_.flank_total_lens);
            }
        }


    }


    std::string QCResultAnalysis::MappingResultReport() {
        std::ostringstream oss;
        oss << "\t\t\t\t\tMapping Statistics" << std::endl;
        oss << "Total Reads\t" << mapping_result_.total_reads_num << std::endl;
        oss << "Total Bases\t" << mapping_result_.total_bases_num << std::endl;
        oss << "Reads mapped to genome\t" << mapping_result_.map_reads_num << std::endl;
        oss << "Bases mapped to genome\t" << mapping_result_.map_bases_num << std::endl;
        oss << "Reads mapped (mapq >= 10) to genome\t" << mapping_result_.mapq10_reads_num <<  std::endl;
        oss << "Bases mapped (mapq >= 10) to genome\t" << mapping_result_.mapq10_bases_num <<  std::endl;
        oss << "mapq >= 10 rate(%)\t" << (double ) 100 * mapping_result_.mapq10_reads_num / mapping_result_.map_reads_num << std::endl;
        std::string chrx_name = "chrX";
        std::string chry_name = "chrY";
        if (qc_base_data_.refer_dict.find(chrx_name) != qc_base_data_.refer_dict.end()
                && qc_base_data_.refer_dict.find(chry_name) != qc_base_data_.refer_dict.end()) {
            int chrx_idx = qc_base_data_.refer_dict.at(chrx_name);
            int chry_idx = qc_base_data_.refer_dict.at(chry_name);
            if (qc_base_data_.target_chr_lens[chrx_idx] > 0
                    && qc_base_data_.target_chr_lens[chry_idx] > 0) {
                double chrx_median = target_results_[chrx_idx].depth / qc_base_data_.target_chr_lens[chrx_idx];
                double chry_median = target_results_[chry_idx].depth / qc_base_data_.target_chr_lens[chry_idx];
                oss << "Mean depth of chrX(X)\t" << chrx_median << std::endl;
                oss << "Mean depth of chrY(X)\t" << chry_median << std::endl;
                oss << "Gender\t" << (target_results_[chry_idx].coverage_pos / qc_base_data_.target_chr_lens[chry_idx] >= 0.5 ? 'M' : 'W') << std::endl;
            } else {
                oss << "Mean depth of chrX(X)\t" << 0.0 << std::endl;
                oss << "Mean depth of chrY(X)\t" << 0.0 << std::endl;
                oss << "Gender\t - " <<  std::endl;
            }
        } else {
            oss << "Mean depth of chrX(X)\t" << 0.0 << std::endl;
            oss << "Mean depth of chrY(X)\t" << 0.0 << std::endl;
            oss << "Gender\t - " <<  std::endl;
        }
        oss << "Duplicate rate(%)\t" << std::setprecision(4) << (double) 100 * mapping_result_.dup_reads_num / mapping_result_.total_reads_num << std::endl;
        return  oss.str();
    }

    std::string QCResultAnalysis::TargetResultReport() {
        std::ostringstream oss;
        oss << "\t\t\t\t\tTarget" << std::endl;
        oss << "Bases in target region(bp)\t " << qc_base_data_.target_total_lens << std::endl;
        oss << "Reads mapped to target region\t" << target_result_.reads_num << std::endl;
        oss << "Bases mapped to target region\t" << target_result_.bases_num << std::endl;
        oss << "Reads mapped (mapq >= 10) to target region\t" << target_result_.mapq10_reads_num <<std::endl;
        oss << "Bases mapped (mapq >= 10) to target region\t" << target_result_.mapq10_bases_num <<std::endl;
        oss << "Mean depth of target region(X)\t" << std::setprecision(5) << (double) target_result_.depth / qc_base_data_.target_total_lens  <<std::endl;
        oss << "Coverage of target region(%)\t" << std::setprecision(4) << (double) 100 * target_result_.coverage_pos  / qc_base_data_.target_total_lens << std::endl;
        std::vector<int> depth_stat = {4, 10, 20, 30, 40, 50, 60, 70, 80, 90, 100};
        for (auto depth :depth_stat ) {
            oss << "Fraction of target region covered >=" << depth << "X(%)\t" << 100 * target_depth_radio[depth] << std::endl;
        }

        return oss.str();
    }

    std::string QCResultAnalysis::FlankResultReport() {
        std::ostringstream oss;
        oss << "\t\t\t\t\tFlank" << std::endl;
        oss << "Bases in flanking region(bp)\t " << qc_base_data_.flank_total_lens << std::endl;
        oss << "Reads mapped to flanking region\t" << flank_result_.reads_num << std::endl;
        oss << "Bases mapped to flanking region\t" << flank_result_.bases_num  << std::endl;
        oss << "Reads mapped (Mapq >= 10) to flanking region\t" << flank_result_.mapq10_reads_num <<std::endl;
        oss << "Bases mapped (Mapq >= 10) to flanking region\t" << flank_result_.mapq10_bases_num <<std::endl;
        oss << "Mean depth of flanking region(X)\t" << std::setprecision(5) << (double) flank_result_.depth / qc_base_data_.flank_total_lens <<std::endl;
        oss << "Coverage of flanking region(%)\t" << std::setprecision(4) <<  (double) 100 * flank_result_.coverage_pos/ qc_base_data_.flank_total_lens<< std::endl;
        std::vector<int> depth_stat = {4, 10, 20, 30, 40, 50, 60, 70, 80, 90, 100};
        for (auto depth : depth_stat ) {
            oss << "Fraction of flanking region covered >=" << depth << "X(%)\t" << 100 * flank_depth_radio[depth] << std::endl;
        }
        return oss.str();
    }

    std::string QCResultAnalysis::ChromosomeResultReport() {
        std::ostringstream oss;
        oss << "\t\t\t\t\tChromosome Depth" << std::endl;
        oss << "Chromosome\tSize\tTotalBase\tCoverPosition\tCoveragePercent\tMeanDepth\tRelativeDepth\tMedianDepth" << std::endl;
        for (int chr_idx = 0; chr_idx < qc_base_data_.refer_dict.size(); ++chr_idx) {
            if (chr_stat_results_[chr_idx].lens > 0) {
                auto &stat_data = chr_stat_results_[chr_idx];
                oss << stat_data.chr_name << "\t" << stat_data.lens << "\t" << stat_data.bases << "\t" << stat_data.cover_pos << "\t";
                oss << std::setprecision(4) << 100 * stat_data.cover_precent << "\t" << stat_data.mean_depth << "\t" <<
                    stat_data.relative_depth << "\t" << stat_data.median_depth << std::endl;
            }
        }
        oss << std::endl;
        return oss.str();
    }

    std::string QCResultAnalysis::DepthResultReport() {
        std::ostringstream oss;
        oss << "\t\t\t\t\tDepth Staticstic" << std::endl;
        if (target_) {
            oss << "Depth\t TRPercent\t FlankPercent\t TotalPercent" << std::endl;
            oss << "0\t 100\t 100\t 100" << std::endl;
            for (int idx = 1; idx < kMaxDepth; ++idx) {
                if (target_depth_radio[idx] > 0.1 || idx <= 100) {
                    oss << idx << "\t" << std::setprecision(4) << 100 * target_depth_radio[idx] << "\t" <<
                        100 * flank_depth_radio[idx] << "\t" << 100 * total_depth_radio[idx] << std::endl;
                }
            }

        } else {
            oss << "Depth\t TotalPercent(%)" << std::endl;
            oss << "0\t 100" << std::endl;
            for (int idx = 1; idx < kMaxDepth; ++idx) {
                if (target_depth_radio[idx] >= 0.1 || idx <= 100) {
                    oss << idx << "\t" << std::setprecision(4) << 100 * target_depth_radio[idx] << std::endl;
                }
            }
        }
        return oss.str();
    }


}