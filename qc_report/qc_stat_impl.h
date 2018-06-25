//
// Created by ruoshui on 6/24/18.
//

#ifndef GAMTOOLS_SM_QC_STAT_IMPL_H
#define GAMTOOLS_SM_QC_STAT_IMPL_H


#include <string>
#include <mutex>

namespace  gamtools {



    class QCStatImpl {
    public:
        QCStatImpl(ArrayBlockQueue<std::unique_ptr<QCShardingData>> &qc_queue,
                   const std::string &ref_file,
                   const std::string &bed_file,
                   const std::string &report_file);

        void DoStatics();

        void Report();

    private:


        void InitReferenceIndex();
        void InitBedRegion();
        void StaticesReadAndDepth();

        bool target_;
        std::string ref_idx_file_;
        std::string bed_file_;
        std::string report_file_;
        std::map<std::string, int> refer_dict_;
        std::vector<int> target_chr_lens_, flank_chr_lens_;
        std::vector<std::vector<std::pair<int, int>>> target_region_, flank_region_;

        std::vector<int> refer_lens_;
        std::mutex mtx_;
//        std::unique_ptr<BaseStat> fastaln_stat_;
        ArrayBlockQueue<std::unique_ptr<QCShardingData>> &qc_sharind_queue_;
    };
}


#endif //GAMTOOLS_SM_QC_STAT_IMPL_H
