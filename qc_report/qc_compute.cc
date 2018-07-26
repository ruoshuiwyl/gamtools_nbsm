//
// Created by ruoshui on 6/24/18.
//

#include <assert.h>
#include <iostream>
#include <util/glogger.h>
#include "qc_compute.h"


namespace gamtools {


    QCCompute::QCCompute(bool target, const gamtools::QCBaseData &qc_base_data)
            :target_(target), qc_base_data_(qc_base_data) {

    }
    void QCCompute::Init(int tid) {
        target_result_.reset(tid);
        mapping_result_.reset(tid);
//        target_results_.tid = tid;
        curr_pos_ = 0;
        curr_end_ = 0;
        if (target_) {
            flank_result_.reset(tid);
            if (tid >= 0) {
                target_region_ = qc_base_data_.target_region[tid];
                flank_region_ = qc_base_data_.flank_region[tid];
            }
            target_depth_reg_ = 0;
            flank_depth_reg_ = 0;
            target_read_reg_ = 0;
            flank_read_reg_ = 0;
        }

    }


    void QCCompute::Compute(const std::vector<StatisticsSlice> &stat_datas) {
        for (auto &stat_data : stat_datas) {
            Statistics(stat_data);
        }
    }

    void QCCompute::Clear() {
        while (!stat_list_.empty()) {
            if (stat_list_.front().second > 0) {
                StatisticsDepth(stat_list_.front().first, stat_list_.front().second);
            }
            stat_list_.pop_front();
        }
    }

    void QCCompute::Statistics(const gamtools::StatisticsSlice &stat) {
        StatisticsRead(stat);
        if (stat.rlen > 1000) {
            GLOG_ERROR << "Error reference length too long";
        }
        if (stat.rlen > 0 && !stat.is_dup) {
//            if (stat.is_dup) {
//                std::cerr << "Error" << std::endl;
//            }
            int end = stat.pos + stat.rlen;
            if (end > curr_end_) {
                if (stat.pos > curr_end_ + 100) { // skip larger no data region
                    while (!stat_list_.empty()) {
                        if (stat_list_.front().second > 0) {
                            StatisticsDepth(stat_list_.front().first, stat_list_.front().second);
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
                        StatisticsDepth(stat_list_.front().first, stat_list_.front().second);
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

    void QCCompute::StatisticsDepth( int pos, int depth) {
        if (target_) {
            if (target_region_[target_depth_reg_].second <= pos
                    && target_depth_reg_  + 1 < target_region_.size()) {
                while ((target_region_.size() > target_depth_reg_)
                       && target_region_[target_depth_reg_].second <= pos) {
                    target_depth_reg_++;
                }
            }
            if (target_region_[target_depth_reg_].first <= pos
                && target_region_[target_depth_reg_].second > pos) {
                target_result_.depth_dist[depth < kMaxDepth ? depth :  kMaxDepth - 1] ++;
                target_result_.coverage_pos++;
                target_result_.depth += depth;
                flank_result_.depth_dist[depth < kMaxDepth ? depth : kMaxDepth - 1]++;
                flank_result_.coverage_pos++;
                flank_result_.depth += depth;
                return;
            }

            if (flank_region_[flank_depth_reg_].second <= pos
                    && flank_depth_reg_ + 1 < flank_region_.size()) {
                while ((flank_region_.size() > flank_depth_reg_) &&
                       flank_region_[flank_depth_reg_].second <= pos) {
                    flank_depth_reg_++;
                }
            }
            if (flank_region_[flank_depth_reg_].first <= pos && flank_region_[flank_depth_reg_].second > pos) {
                flank_result_.depth_dist[depth < kMaxDepth ? depth : kMaxDepth - 1]++;
                flank_result_.depth += depth;
                flank_result_.coverage_pos++;
            }
        } else {
            target_result_.coverage_pos++;
            target_result_.depth += depth;
            target_result_.depth_dist[depth < kMaxDepth ? depth : kMaxDepth - 1]++;
        }
    }

    void QCCompute::StatisticsRead(const gamtools::StatisticsSlice &stat) {
        mapping_result_.total_reads_num++;
        mapping_result_.total_bases_num += stat.qlen;
        mapping_result_.dup_reads_num += stat.is_dup? 1 : 0;
        if (stat.rlen == 0) {
            return;
        }
        mapping_result_.map_reads_num++;
        mapping_result_.map_bases_num += stat.qlen;
        if (stat.mapq >= 10) {
            mapping_result_.mapq10_reads_num++;
            mapping_result_.mapq10_bases_num += stat.qlen;
        }
        if (target_ ) {
            if (target_region_.empty()) {
                return;
            }
            int overlap_end = stat.pos + stat.rlen;
            if (target_region_[target_read_reg_].second < stat.pos) {
                while (target_region_[target_read_reg_].second <= stat.pos
                       && (target_region_.size() > target_read_reg_)) {
                    target_read_reg_++;
                }
            }
            if (!(target_region_[target_read_reg_].first >= overlap_end
                  || target_region_[target_read_reg_].second <= stat.pos)) {
                target_result_.reads_num++;
                target_result_.bases_num += stat.qlen;
                flank_result_.reads_num++;
                flank_result_.bases_num += stat.qlen;
                if (stat.mapq >= kMapThreshold ) {
                    target_result_.mapq10_reads_num++;
                    target_result_.mapq10_bases_num += stat.qlen;
                    flank_result_.mapq10_reads_num++;
                    flank_result_.mapq10_bases_num += stat.qlen;
                }
                return ;
            }
            if (flank_region_[flank_read_reg_].second < stat.pos) {
                while (flank_region_[flank_read_reg_].second <= stat.pos
                       && (flank_region_.size() > target_read_reg_)) {
                    flank_read_reg_++;
                }
            }
            if (!(flank_region_[flank_read_reg_].first >= overlap_end
                  || flank_region_[flank_read_reg_].second <= stat.pos)) {
                flank_result_.reads_num++;
                flank_result_.bases_num += stat.qlen;
                if (stat.mapq >= 10) {
                    flank_result_.mapq10_reads_num++;
                    flank_result_.mapq10_bases_num += stat.qlen;
                }
            }
        } else {
            target_result_.reads_num++;
            target_result_.bases_num += stat.qlen;
            if (stat.mapq >= kMapThreshold) {
                target_result_.mapq10_reads_num++;
                target_result_.mapq10_bases_num += stat.qlen;
            }

        }
    }


}