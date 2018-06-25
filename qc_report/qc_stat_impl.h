//
// Created by ruoshui on 6/24/18.
//

#ifndef GAMTOOLS_SM_QC_STAT_IMPL_H
#define GAMTOOLS_SM_QC_STAT_IMPL_H


#include <string>
#include <mutex>
#include <util/array_block_queue.h>
#include "qc_result_data.h"
#include "qc_result_analysis.h"
#include <thread>
#include <util/bounded_queue.h>

namespace  gamtools {

    struct StatisticsSlice {
        StatisticsSlice() = default;
        bool is_dup;
        int tid;
        int pos;
        int rlen;
        int qlen;
        int mapq; //
    };
    struct QCShardingData {
        explicit QCShardingData(int chr_idx, int idx): tid(chr_idx), order_(idx){}
        int order() {
            return order_;
        }
        void InsertStatData(StatisticsSlice &stat_data) {
            stat_datas.push_back(stat_data);
        }
        int tid;
        int order_;
        std::vector<StatisticsSlice> stat_datas;
    };


    class QCStatImpl {
    public:
        QCStatImpl(BoundedQueue<std::unique_ptr<QCShardingData>> &qc_queue,
                   const std::string &ref_file,
                   const std::string &bed_file,
                   const std::string &report_file);
        std::thread StaticsSpawn();
        void DoStatics();
        void Report();
    private:
        void InitReferenceIndex();
        void InitBedRegion();
        bool target_;
        std::string ref_idx_file_;
        std::string bed_file_;
        std::string report_file_;
        QCBaseData base_data_;
        std::mutex mtx_;
        std::unique_ptr<QCResultAnalysis> qc_result_analysis_;
        BoundedQueue<std::unique_ptr<QCShardingData>> &qc_sharind_queue_;
    };
}


#endif //GAMTOOLS_SM_QC_STAT_IMPL_H
