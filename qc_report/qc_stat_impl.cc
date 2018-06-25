//
// Created by ruoshui on 6/24/18.
//

#include <util/glogger.h>
#include "qc_stat_impl.h"
#include "qc_compute.h"
#include <fstream>


namespace gamtools {

    QCStatImpl::QCStatImpl(BoundedQueue<std::unique_ptr<QCShardingData>> &qc_queue,
                           const std::string &ref_file,
                           const std::string &bed_file,
                           const std::string &report_file)
            :qc_sharind_queue_(qc_queue),
             ref_idx_file_(ref_file),
             bed_file_(bed_file),
             report_file_(report_file)  {
        InitReferenceIndex();
        if (!bed_file_.empty()) {
            target_ = true;
            base_data_.target = true;
        } else {
            target_ = false;
            base_data_.target = false;
        }
        InitBedRegion();

    }
    void QCStatImpl::InitReferenceIndex() {
        char data[1024];
        char chr_name[64];
        int chr_len;
        int chr_idx = 0;
        std::ifstream ref_ifs(ref_idx_file_);
        if (ref_ifs.is_open()){
            while (ref_ifs.getline(data, 1024) ) {
                sscanf(data, "%s %d", chr_name, &chr_len);
                base_data_.refer_dict[chr_name] = chr_idx;
                base_data_.refer_lens.push_back(chr_len);
                base_data_.refer_names.push_back(chr_name);
                chr_idx++;
            }
            ref_ifs.close();
        } else {
            GLOG_ERROR << "Load Reference file index *.fai Error";
        }
    }


    void QCStatImpl::InitBedRegion() {

        char chr_name[64];
        int total_chr_size = base_data_.refer_dict.size();
        base_data_.target_region.resize(total_chr_size);
        base_data_.flank_region.resize(total_chr_size);
        base_data_.target_chr_lens.resize(total_chr_size, 0);
        base_data_.flank_chr_lens.resize(total_chr_size, 0);
        const int flank_extend = 200;
        if (target_) {
            std::ifstream bed_ifs(bed_file_);
            if (bed_ifs.is_open()) {
                char *data = new char[1024];
                while (bed_ifs.getline(data, 1024)) {
                    int target_start, target_stop;
                    sscanf(data, "%s %d %d", chr_name, &target_start, &target_stop);
                    int chr_idx = base_data_.refer_dict[chr_name];
                    int target_len = target_stop - target_start;
                    auto bed_region = std::make_pair(target_start, target_stop);
                    base_data_.target_region[chr_idx].push_back(bed_region);
                    base_data_.target_chr_lens[chr_idx] += target_len;
                    base_data_.target_total_lens += target_len;
                    int flank_start = target_start - flank_extend >= 0 ? target_start - flank_extend : 0;
                    int flank_stop = target_stop + flank_extend < base_data_.refer_lens[chr_idx] ?
                                     target_stop + flank_extend : base_data_.refer_lens[chr_idx];
                    auto flank_region = std::make_pair(flank_start, flank_stop);
                    int flank_len = flank_stop - flank_start;
                    base_data_.flank_region[chr_idx].push_back(flank_region);
                    base_data_.flank_chr_lens[chr_idx] += flank_len;
                    base_data_.flank_total_lens += flank_len;
//                flank_total_len_ += flank_len;
                }
                bed_ifs.close();
                delete[] data;
            } else {
                GLOG_ERROR << "Can not open bed file " << bed_file_;
            }
        } else { // WGS
            for ( int chr_idx = 0; chr_idx <total_chr_size; ++chr_idx) {
                int target_start = 0;
                int target_end = base_data_.refer_lens[chr_idx];
                base_data_.target_region[chr_idx].push_back(std::make_pair(target_start, target_end));
                base_data_.target_chr_lens[chr_idx] = target_end;
                base_data_.target_total_lens += target_end;
            }

        }

    }

    std::thread QCStatImpl::StaticsSpawn() {
        std::thread statics_thread(&QCStatImpl::DoStatics, this);
        return statics_thread;
    }

    void QCStatImpl::DoStatics() {
        std::unique_ptr<QCShardingData> qc_data_ptr;
        std::unique_ptr<QCCompute> qc_compute;
        while (qc_sharind_queue_.read(qc_data_ptr)) {
            qc_compute->Init(qc_data_ptr->tid);
            qc_compute->Compute(qc_data_ptr->stat_datas);
            {
                std::lock_guard<std::mutex> lck(mtx_);
                qc_result_analysis_->AddMappingResult(qc_compute->MappingResult());
                qc_result_analysis_->AddTargetQCResult(qc_compute->TargetResult());
                if (target_) {
                    qc_result_analysis_->AddFlankQCResult(qc_compute->FlankResult());
                }
            }
            qc_compute->Clear();
        }
    }

    void QCStatImpl::Report() {
        std::ofstream ofs(report_file_);
        if (ofs.is_open()) {
            ofs << qc_result_analysis_->Report();
            ofs.close();
        } else {
            GLOG_ERROR << "Can not open eport file " << report_file_;
        }
    }
}