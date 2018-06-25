//
// Created by ruoshui on 6/24/18.
//

#ifndef GAMTOOLS_SM_QC_COMPUTE_H
#define GAMTOOLS_SM_QC_COMPUTE_H


#include "quality_control.h"

namespace gamtools {
    const int kMaxDepth = 8192;
    struct QCStatResult {
        void reset(int chr_idx) {
            tid = chr_idx;
            reads_num = 0;
            bases_num = 0;
            mapq10_reads_num = 0;
            mapq10_bases_num = 0;
            coverage_pos = 0;
            depth = 0;
            depth_dist.resize(kMaxDepth, 0);
        }
        int tid;
        std::string chr_name_;
        int64_t reads_num;
        int64_t bases_num;
        int64_t mapq10_reads_num;
        int64_t mapq10_bases_num;
        int64_t coverage_pos;
        int64_t depth;
        std::vector<int64_t> depth_dist;
    };

    struct QCMappingResult {
        void reset(int chr_idx) {
            tid = chr_idx;
            dup_reads_num = 0;
            total_reads_num = 0;
            total_bases_num = 0;
            map_reads_num = 0;
            map_bases_num = 0;
            mapq10_reads_num = 0;
            mapq10_bases_num = 0;
        }
        int tid;
        int64_t dup_reads_num;
        int64_t total_reads_num;
        int64_t total_bases_num;
        int64_t map_reads_num;
        int64_t map_bases_num;
        int64_t mapq10_reads_num;
        int64_t mapq10_bases_num;
    };


    class QCCompute {
    public:
        QCCompute(bool target, const std::vector<std::vector<std::pair<int, int>>> &total_region_);
        void init(int tid);
        void Compute(std::unique_ptr<QCShardingData> qc_data_ptr);

    private:
        void StatisticsRead(const StatisticsSlice &stat);
        void StatisticsDepth(int pos, int depth);
        void Statistics(const StatisticsSlice &stat);

        bool target_;
        int curr_pos_;
        int curr_end_;
        int target_depth_reg_;
        int flank_depth_reg_;
        std::deque<std::pair<int,int>> stat_list_;
        std::vector<std::pair<int, int>> target_region_;
        std::vector<std::pair<int, int>> flank_region_;


        QCStatResult target_result_;
        QCStatResult flank_result_;
        QCMappingResult mapping_result_;

        const std::vector<std::vector<std::pair<int, int>>> &total_target_region_;
        const std::vector<std::vector<std::pair<int, int>>> &total_flank_region_;

    };
}


#endif //GAMTOOLS_SM_QC_COMPUTE_H
