//
// Created by ruoshui on 6/25/18.
//

#ifndef GAMTOOLS_SM_QC_RESULT_H
#define GAMTOOLS_SM_QC_RESULT_H


#include <vector>
#include <string>
#include "qc_result_data.h"

namespace gamtools {
    class QCResultAnalysis {
    public:
        QCResultAnalysis(const QCBaseData &qc_base_data);
        void AddTargetQCResult(const QCStatResult &qc_target);
        void AddFlankQCResult(const QCStatResult &qc_flank);
        void AddMappingResult(const QCMappingResult &map_result);
        std::string  Report();
    private:
        struct  ChromosomeStatResult {
            std::string chr_name;
            int tid;
            int lens;
            int64_t bases;
            int cover_pos;
            double cover_precent;
            double mean_depth;
            double relative_depth;
            int median_depth;
        };
        void Analysis();
        void AnalysisMappingResult();
        void AnalysisTargetResult();
        void AnalysisFlankResult();
        void AnalysisChromesomeResult();
        void AnalysisDepthResult();


        std::string MappingResultReport();
        std::string TargetResultReport();
        std::string FlankResultReport();
        std::string ChromosomeResultReport();
        std::string DepthResultReport();
        const bool target_;
        std::vector<QCStatResult> target_results_;
        std::vector<QCStatResult> flank_results_;
        std::vector<QCMappingResult> mapping_results_;
        QCMappingResult mapping_result_;
        QCMappingResult unmapping_result_;
        QCStatResult target_result_;
        QCStatResult flank_result_;
        std::vector<double> target_depth_radio;
        std::vector<double> flank_depth_radio;
        std::vector<double> total_depth_radio;
        std::vector<ChromosomeStatResult> chr_stat_results_;
        const QCBaseData &qc_base_data_;
    };
}


#endif //GAMTOOLS_SM_QC_RESULT_H
