//
// Created by ruoshui on 6/24/18.
//

#include <util/glogger.h>
#include "qc_stat_impl.h"
#include "qc_compute.h"
#include <fstream>


namespace gamtools {
    void QCStatImpl::InitReferenceIndex() {
        char data[1024];
        char chr_name[64];
        int chr_len;
        int chr_idx = 0;
        std::ifstream ref_ifs(ref_idx_file_);
        if (ref_ifs.is_open()){
            while (ref_ifs.getline(data, 1024) ) {
                sscanf(data, "%s %d", chr_name, &chr_len);
                refer_dict_[chr_name] = chr_idx;
                refer_lens_.push_back(chr_len);
                chr_idx++;
            }
            ref_ifs.close();
        } else {
            GLOG_ERROR << "Load Reference file index *.fai Error";
        }
    }


    void QCStatImpl::InitBedRegion() {
        std::ifstream bed_ifs(bed_file_);
        char chr_name[64];
        target_region_.resize(refer_dict_.size());
        flank_region_.resize(refer_dict_.size());
        target_chr_lens_.resize(refer_dict_.size(), 0);
        flank_chr_lens_.resize(refer_dict_.size(), 0);
//        target_total_len_ = 0;
//        flank_total_len_ = 0;
        const int flank_extend = 200;
        if (bed_ifs.is_open()) {
            char *data = new char[1024];
            while(bed_ifs.getline(data, 1024)) {
                int target_start, target_stop;
                sscanf(data, "%s %d %d", chr_name, &target_start, &target_stop);
                int chr_idx = refer_dict_[chr_name];
                int target_len = target_stop - target_start;
                auto  bed_region = std::make_pair(target_start, target_stop);
                target_region_[chr_idx].push_back(bed_region);
                target_chr_lens_[chr_idx] += target_len;
//                target_total_len_ += target_len;
                int flank_start = target_start - flank_extend >=  0 ? target_start - flank_extend : 0;
                int flank_stop = target_stop + flank_extend <  refer_lens_[chr_idx]? target_stop + flank_extend : refer_lens_[chr_idx];
                auto flank_region = std::make_pair(flank_start, flank_stop);
                int flank_len = flank_stop - flank_start;
                flank_region_[chr_idx].push_back(flank_region);
                flank_chr_lens_[chr_idx] += flank_len;
//                flank_total_len_ += flank_len;
            }
            bed_ifs.close();
            delete [] data;
        } else { // WGS
            for ( int chr_idx = 0; chr_idx <refer_dict_.size(); ++chr_idx) {
                int target_start = 0;
                int target_end = refer_lens_[chr_idx];
                target_region_[chr_idx].push_back(std::make_pair(target_start, target_end));
                target_chr_lens_[chr_idx] = target_end;
            }

        }

    }

    void QCStatImpl::DoStatics() {
        std::unique_ptr<QCShardingData> qc_data_ptr;
        std::unique_ptr<QCCompute> qc_compute;
        while (qc_sharind_queue_.read(qc_data_ptr)) {
            qc_compute.Init();
            auto qc_result = qc_compute.Compute(qc_data_ptr);
            {
                std::lock_guard<std::mutex> lck(mtx_);
            }
        }
    }
}