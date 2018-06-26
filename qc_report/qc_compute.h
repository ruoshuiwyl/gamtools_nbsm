//
// Created by ruoshui on 6/24/18.
//

#ifndef GAMTOOLS_SM_QC_COMPUTE_H
#define GAMTOOLS_SM_QC_COMPUTE_H


#include "quality_control.h"
#include "qc_result_data.h"
#include "qc_stat_impl.h"

namespace gamtools {

    class QCCompute {
    public:
        QCCompute(bool target, const QCBaseData &qc_base_data);
        void Init(int tid);
        void Compute(const std::vector<StatisticsSlice> &stat_datas);
        void Clear();
        const QCStatResult& TargetResult(){
            return target_result_;
        }
        const QCStatResult& FlankResult() {
            return flank_result_;
        }
        const QCMappingResult& MappingResult() {
            return mapping_result_;
        }
    private:
        void StatisticsRead(const StatisticsSlice &stat);
        void StatisticsDepth(int pos, int depth);
        void Statistics(const StatisticsSlice &stat);
        std::set<int> poss;
        const bool target_;
        int curr_pos_;
        int curr_end_;
        int target_depth_reg_;
        int flank_depth_reg_;
        int target_read_reg_;
        int flank_read_reg_;
        std::deque<std::pair<int,int>> stat_list_;
        std::vector<std::pair<int, int>> target_region_;
        std::vector<std::pair<int, int>> flank_region_;
        QCStatResult target_result_;
        QCStatResult flank_result_;
        QCMappingResult mapping_result_;
//        const std::vector<std::vector<std::pair<int, int>>> &total_target_region_;
//        const std::vector<std::vector<std::pair<int, int>>> &total_flank_region_;
        const QCBaseData &qc_base_data_;

    };
}


#endif //GAMTOOLS_SM_QC_COMPUTE_H
