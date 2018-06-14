//
// Created by ruoshui on 6/13/18.
//

#include <fstream>
#include <util/glogger.h>
#include "quality_control.h"
#include "qc_stat.h"


namespace gamtools {


    void QualityControl::Init() {
        base_stat_->Init();
    }



    void QualityControl::Statistics( const StatisticsSlice &stat) {
        base_stat_->StatisticsRead(stat);
        if (base_stat_->HasStat(stat.tid)) {
            assert(stat.tid  >= curr_idx_);
            if (stat.tid > curr_idx_) {
                while(!stat_list_.empty()){
                    base_stat_->StatisticsDepth(curr_idx_, stat_list_.front().first, stat_list_.front().second);
                    stat_list_.pop_front();
                }
                curr_idx_ = stat.tid;
                curr_pos_ = 0;
                curr_end_ = 0;
            }
            int end = stat.pos + stat.rlen;
            if (end > curr_end_) {
                for ( int idx = curr_end_; idx < end; ++idx) {
                    stat_list_.push_back(std::make_pair(idx, 0));
                }
                curr_end_  = end;
            }
            assert(stat.pos >= curr_pos_);
            if (stat.pos > curr_pos_) {
                while(stat_list_.front().first < stat.pos) {
                    base_stat_->StatisticsDepth(curr_idx_, stat_list_.front().first, stat_list_.front().second);
                    stat_list_.pop_front();
                }
                assert(curr_pos_ == stat_list_.front().first);
            }
            for (int idx = 0; idx < stat.rlen ; ++idx) {
                stat_list_[idx].second++;
            }
        }
    }


    void QualityControl::Report() {
        std::ofstream report_file(report_filename_);
        if (report_file.is_open()) {
            report_file << base_stat_->Report();
            report_file.close();
        } else {
            GLOG_ERROR << "Load QC Report Filename" << report_filename_;
        }

    }
}