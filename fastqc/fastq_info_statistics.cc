//
// Created by ruoshui on 5/24/18.
//

#include <util/glogger.h>
#include "fastq_info_statistics.h"
#include "base_fastq_info.h"
#include "fastqc/gam_fastq_file.h"


namespace gamtools {
    FastqInfoStatistics::FastqInfoStatistics(int lane_cnt, const std::string &filename, std::vector<GAMFastqFileInfo> &fastq_file_lists)
            : lane_cnt_(lane_cnt),
              file_name_(filename),
              fastq_file_lists_(fastq_file_lists){
        fastq_infos_.resize(lane_cnt_);
    }

    void FastqInfoStatistics::AddBatchFastqInfo(int lane_id, const BaseFqInfo &baseFqInfo1,
                                                const BaseFqInfo &baseFqInfo2) {
        if (lane_id < lane_cnt_) {
            fastq_infos_[lane_id].first.Add(baseFqInfo1);
            fastq_infos_[lane_id].second.Add(baseFqInfo2);
        } else {
            GLOG_ERROR << "lane number exceed set lane count number ";
        }
    }

    void FastqInfoStatistics::OutputFastqInfo() {

        std::ofstream ofs(file_name_);
        if (!ofs.is_open()) {
            GLOG_ERROR << "Output QC infomation file name " << file_name_ << " failed";
        }
        std::stringstream oss;
        if (fastq_infos_.size() == 1) {
            oss << "Total information of sample " << std::endl;
            oss << "Sample have one lane data" << std::endl;
            oss << "\tType\t\tRaw Data\t\tClean" << std::endl;
            oss << PrintBaseFqInfo(&fastq_infos_[0].first, &fastq_infos_[0].second) << std::endl;
        } else {
            std::pair<BaseFqInfo, BaseFqInfo> sample_fastq_info;
            for (auto &fq_infos : fastq_infos_){
                sample_fastq_info.first.Add(fq_infos.first);
                sample_fastq_info.second.Add(fq_infos.second);
            }
            oss << "Total information of sample " << std::endl;
            oss << "Total have " << fastq_infos_.size() << "lanes" << std::endl;
            oss << "\tType\t\tRaw Data\t\tClean" << std::endl;
            int lane_id = 0;
            uint64_t sample_raw_base_num = sample_fastq_info.first.raw_base_num + sample_fastq_info.second.raw_base_num ;
            uint64_t sample_clean_base_num = sample_fastq_info.first.clean_base_num + sample_fastq_info.second.clean_base_num ;
            for (auto &fq_info :fastq_infos_) {
                uint64_t raw_base_num = fq_info.first.raw_base_num + fq_info.second.raw_base_num;
                uint64_t clean_base_num = fq_info.first.clean_base_num + fq_info.second.clean_base_num;
                oss << "Propotions of lane " << lane_id++ << "\t " << std::setprecision(4)
                    << (float)(100 * raw_base_num/sample_raw_base_num) << "\t " << (float)(100 * clean_base_num/sample_clean_base_num) << std::endl;
            }
            std::string head;
            oss << PrintBaseFqInfo(&sample_fastq_info.first, &sample_fastq_info.second) << std::endl;
            lane_id = 0;
            for (auto &fq_infos : fastq_infos_) {
                oss << "Lane ID" << lane_id << std::endl;
                oss << "Fastq filename :" << fastq_file_lists_[lane_id].fastq1_filename << "\t " << fastq_file_lists_[lane_id].fastq2_filename << std::endl;
                oss << "\tType\t\tRaw Data\t\tClean" << std::endl;
                oss << PrintBaseFqInfo(&sample_fastq_info.first, &sample_fastq_info.second) << std::endl;
                lane_id++;
            }
        }

        ofs << oss.str();
        ofs.close();
    }
}