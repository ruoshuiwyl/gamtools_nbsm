//
// Created by ruoshui on 6/13/18.
//

#include <fstream>
#include <cassert>

//#include <util/glogger.h>
#include "quality_control.h"
#include "qc_stat.h"


namespace gamtools {

    QualityControl::QualityControl(const std::string &ref_file, const std::string &report_file)
            : report_filename_(report_file),
              curr_idx_(-1),
              curr_pos_(-1),
              curr_end_(-1) {
        base_stat_ =  std::unique_ptr<WGSStat>( new WGSStat(ref_file));
    }
    QualityControl::QualityControl(const std::string &ref_file, const std::string &bed_file,
                                   const std::string &report_file)
        : report_filename_(report_file),
          curr_idx_(-1),
          curr_pos_(-1),
          curr_end_(-1) {
        base_stat_ = std::unique_ptr<TargetStat> (new TargetStat(ref_file, bed_file));
    }



    void QualityControl::Init() {
        base_stat_->Init();
    }


    void QualityControl::Statistics( const StatisticsSlice &stat) {
        base_stat_->StatisticsRead(stat);
        if (base_stat_->HasStat(stat.tid) && stat.rlen > 0) {
            assert(stat.tid  >= curr_idx_);
            if (stat.tid > curr_idx_) {
                while(!stat_list_.empty()){
                    if (stat_list_.front().second > 0) {
                        base_stat_->StatisticsDepth(curr_idx_, stat_list_.front().first, stat_list_.front().second);
                    }
                    stat_list_.pop_front();
                }
                curr_idx_ = stat.tid;
                curr_pos_ = 0;
                curr_end_ = 0;
            }
            int end = stat.pos + stat.rlen;
            if (end > curr_end_) {
                if (stat.pos > curr_end_ + 100) {
                    while (!stat_list_.empty()) {
                        if (stat_list_.front().second > 0) {
                            base_stat_->StatisticsDepth(curr_idx_, stat_list_.front().first, stat_list_.front().second);
                        }
                        stat_list_.pop_front();
                    }
                    curr_pos_ = stat.pos;
                    curr_end_ = stat.pos;
                }
                for (int idx = curr_end_; idx < end; ++idx) {
                    stat_list_.push_back(std::make_pair(idx, 0));
                }
                curr_end_ = end;
            }
            assert(stat.pos >= curr_pos_);
            if (stat.pos > curr_pos_) {
                while (stat_list_.front().first < stat.pos) {
                    if (stat_list_.front().second > 0) {
                        base_stat_->StatisticsDepth(curr_idx_, stat_list_.front().first, stat_list_.front().second);
                    }
                    stat_list_.pop_front();
                }
                assert(stat.pos == stat_list_.front().first);
                curr_pos_ = stat_list_.front().first;
            }
            for (int idx = 0; idx < stat.rlen ; ++idx) {
                stat_list_[idx].second++;
            }
        }
    }


    void QualityControl::Report() {

        while(!stat_list_.empty()){
            if (stat_list_.front().second > 0) {
                base_stat_->StatisticsDepth(curr_idx_, stat_list_.front().first, stat_list_.front().second);
            }
            stat_list_.pop_front();
        }

        std::ofstream report_file(report_filename_, std::ofstream::app);
        if (report_file.is_open()) {
            report_file << base_stat_->Report();
            report_file.close();
        } else {
//            GLOG_ERROR << "Load QC Report Filename" << report_filename_;
        }

    }
}