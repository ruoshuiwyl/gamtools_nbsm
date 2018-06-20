//
// Created by ruoshui on 6/13/18.
//

#ifndef GAMTOOLS_SM_QUALITY_CONTROL_H
#define GAMTOOLS_SM_QUALITY_CONTROL_H


#include <string>
#include <memory>
#include <map>
#include <vector>
#include <util/slice.h>
#include <deque>
#include "qc_stat.h"

namespace gamtools {
    typedef std::tuple<int, int, int> bed_t;


    class QualityControl {
    public:
        QualityControl(const std::string &ref_file, const std::string &bed_file, const std::string &report_file);
        QualityControl(const std::string &ref_file, const std::string &report_file);
        void Init();
        void Statistics(const StatisticsSlice &stat);
        void Report();
    private:

//        void StatisticsRead(const StatisticsSlice &stat);
//        void StatisticsDepth(const StatisticsSlice &stat );


        std::deque<std::pair<int,int>> stat_list_;
        int curr_idx_;
        int curr_pos_;
        int curr_end_;
        std::string ref_filename_;
        std::string report_filename_;
        std::unique_ptr<BaseStat> base_stat_;
    };
}


#endif //GAMTOOLS_SM_QUALITY_CONTROL_H
