//
// Created by ruoshui on 5/7/18.
//

#ifndef GAMTOOLS_SM_BASE_FASTQ_INFO_H
#define GAMTOOLS_SM_BASE_FASTQ_INFO_H


#include <strings.h>
#include <cstdint>
#include <string>

namespace gamtools {
    struct BaseFqInfo {
        BaseFqInfo() {
            bzero(this, sizeof(BaseFqInfo));
//         maxQualityValue = 41;
        }
        uint64_t  raw_read_num;
        uint64_t  raw_base_num;
        uint64_t  clean_read_num;
        uint64_t  clean_base_num;

        uint64_t  raw_base_n;
        uint64_t  clean_base_n;

        uint64_t  raw_base_gc;
        uint64_t  clean_base_gc;

        uint64_t  raw_q20;
        uint64_t  clean_q20;

        uint64_t  raw_q30;
        uint64_t  clean_q30;

        uint64_t  adapter_num;
        uint64_t  n_excess_num;
        uint64_t  low_qual_num;

        uint64_t  total_adapter_num;
        uint64_t  total_n_excess_num;
        uint64_t  total_low_qual_num;
        void Add(const BaseFqInfo &base_fq_info);

    };

    std::string PrintBaseFqInfo(const std::string &head, const BaseFqInfo *fq_info1, const BaseFqInfo *fq_info2);
}


#endif //GAMTOOLS_SM_BASE_FASTQ_INFO_H
