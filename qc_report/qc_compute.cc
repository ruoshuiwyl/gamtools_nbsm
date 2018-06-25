//
// Created by ruoshui on 6/24/18.
//

#include <assert.h>
#include "qc_compute.h"


namespace gamtools {

    void QCCompute::init(int tid) {
        target_result_.reset(tid);
//        target_result_.tid = tid;
        curr_pos_ = 0;
        curr_end_ = 0;
        .
        if (target_) {
            flank_result_.reset(tid);
            target_region_ = total_target_region_[tid];
            flank_region_ = total_flank_region_[tid];
            target_depth_reg_ = 0;
            flank_depth_reg_ = 0;
        }

    }


    void  QCCompute::Compute(std::unique_ptr<gamtools::QCShardingData> qc_data_ptr) {
        auto &stat_datas = qc_data_ptr->stat_datas;
        for (auto &stat_data : stat_datas) {
            Statistics(stat_data);
        }
    }


    void QCCompute::Statistics(const gamtools::StatisticsSlice &stat) {
        StatisticsRead(stat);
        if (stat.rlen > 0) {
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
            if (target_region_[target_depth_reg_].second <= pos) {
                while ((target_region_.size() > target_depth_reg_)
                       && target_region_[target_depth_reg_].second <= pos) {
                    target_depth_reg_++;
                }
            }

            if (target_region_[target_depth_reg_].first <= pos
                && target_region_[target_depth_reg_].second > pos) {
                target_result_.depth_dist[depth < kMaxDepth?depth :  kMaxDepth - 1] ++;
                target_result_.coverage_pos++;
                target_result_.depth += depth;
                flank_result_.depth_dist[depth < kMaxDepth? depth : kMaxDepth - 1]++;
                flank_result_.coverage_pos++;
                flank_result_.depth += depth;
                return;
            }

            if (flank_region_[flank_depth_reg_].second <= pos) {
                while ((flank_region_.size() > flank_depth_reg_) &&
                       flank_region_[flank_depth_reg_].second <= pos) {
                    target_depth_reg_++;
                }
            }
            if (flank_region_[flank_depth_reg_].first <= pos && flank_region_[flank_depth_reg_].second < pos) {
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
        if (stat.rlen == 0) {
            return;
        }
        if (target_ && !target_region_.empty()) {
            if (stat.tid > target_read_idx_) {
                target_read_idx_ = stat.tid;
                target_read_reg_ = 0;
                target_read_bed_ = target_region_[target_read_reg_];
                flank_read_idx_ = stat.tid;
                flank_read_reg_ = 0;
                flank_read_bed_ = flank_region_[flank_read_idx_][flank_read_reg_];
            }
            int overlap_end = stat.pos + stat.rlen;
            if (std::get<1>(target_read_bed_) < stat.pos) {
                while (std::get<1>(target_region_[target_read_idx_][target_read_reg_]) <= stat.pos
                       && (target_region_[target_read_idx_].size() > target_read_reg_)) {
                    target_read_reg_++;
                }
                target_read_bed_ = target_region_[target_read_idx_][target_read_reg_];
            }

            if (!(std::get<0>(target_read_bed_) >= overlap_end || std::get<1>(target_read_bed_) <= stat.pos)) {
                target_reads_[target_read_idx_]++;
                target_bases_[target_read_idx_] += stat.qlen;
                target_total_reads_ ++;
                target_total_bases_ += stat.qlen;
                if (stat.mapq >= kMapq ) {
                    mapq10_target_reads_[target_read_idx_]++;
                    mapq10_target_bases_[target_read_idx_] +=  stat.qlen;
                    mapq10_target_total_reads_ ++;
                    mapq10_target_total_bases_ += stat.qlen;
                }
            }

            if (std::get<1>(flank_read_bed_)) {
                while (std::get<1>(flank_region_[flank_read_idx_][flank_read_reg_]) <= stat.pos
                       && (flank_region_[flank_read_idx_].size() > target_read_reg_)) {
                    flank_read_reg_++;
                }
                flank_read_bed_ = flank_region_[flank_read_idx_][flank_read_reg_];
            }

            if (!(std::get<0>(flank_read_bed_) >= overlap_end || std::get<1>(flank_read_bed_) <= stat.pos)) {
                flank_reads_[flank_read_idx_]++;
                flank_bases_[flank_read_idx_] += stat.qlen;
                flank_total_reads_++;
                flank_total_bases_ += stat.qlen;
                if (stat.mapq >= kMapq) {
                    mapq10_flank_reads_[flank_read_idx_]++;
                    mapq10_flank_bases_[flank_read_idx_] +=  stat.qlen;
                    mapq10_flank_total_reads_++;
                    mapq10_flank_total_bases_ += stat.qlen;
                }
            }


        } else {

                mapping_result_.map_reads_num++;
                mapping_result_.map_bases_num += stat.qlen;
                if (stat.mapq >= 10) {
                    mapping_result_.mapq10_reads_num++;
                    mapping_result_.mapq10_bases_num += stat.qlen;
                }

        }
    }


}