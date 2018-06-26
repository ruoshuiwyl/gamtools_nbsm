//
// Created by ruoshui on 6/25/18.
//

#ifndef GAMTOOLS_SM_QC_RESULT_DATA_H
#define GAMTOOLS_SM_QC_RESULT_DATA_H


#include <string>
#include <vector>
#include <map>

namespace gamtools {
    const int kMaxDepth = 8192;
    const int kMapThreshold = 10;
    struct QCStatResult {
        QCStatResult() {
            tid = 0;
            reads_num = 0;
            bases_num = 0;
            mapq10_reads_num = 0;
            mapq10_bases_num = 0;
            coverage_pos = 0;
            depth = 0;
            depth_dist.resize(kMaxDepth, 0);
        }
        void reset(int chr_idx) {
            tid = chr_idx;
            reads_num = 0;
            bases_num = 0;
            mapq10_reads_num = 0;
            mapq10_bases_num = 0;
            coverage_pos = 0;
            depth = 0;
            depth_dist.resize(kMaxDepth, 0);
        }
        void Add(const QCStatResult &lhs);
        int tid;
        std::string chr_name_;
        int64_t reads_num;
        int64_t bases_num;
        int64_t mapq10_reads_num;
        int64_t mapq10_bases_num;
        int64_t coverage_pos;
        int64_t depth;
        std::vector<int64_t> depth_dist;
    };

    struct QCMappingResult {
        QCMappingResult() {
            tid = 0;
            dup_reads_num = 0;
            total_reads_num = 0;
            total_bases_num = 0;
            map_reads_num = 0;
            map_bases_num = 0;
            mapq10_reads_num = 0;
            mapq10_bases_num = 0;
        }
        void reset(int chr_idx) {
            tid = chr_idx;
            dup_reads_num = 0;
            total_reads_num = 0;
            total_bases_num = 0;
            map_reads_num = 0;
            map_bases_num = 0;
            mapq10_reads_num = 0;
            mapq10_bases_num = 0;
        }
        void Add(const QCMappingResult &lhs);
        int tid;
        int64_t dup_reads_num;
        int64_t total_reads_num;
        int64_t total_bases_num;
        int64_t map_reads_num;
        int64_t map_bases_num;
        int64_t mapq10_reads_num;
        int64_t mapq10_bases_num;
    };

    struct QCBaseData {
        QCBaseData(): target(false),
                      target_total_lens(0),
                      flank_total_lens(0) {}
        bool target;
        int64_t target_total_lens;
        int64_t flank_total_lens;
        std::map<std::string, int> refer_dict;
        std::vector<int> target_chr_lens;
        std::vector<int> flank_chr_lens;
        std::vector<std::vector<std::pair<int, int>>> target_region;
        std::vector<std::vector<std::pair<int, int>>> flank_region;
        std::vector<int> refer_lens;
        std::vector<std::string> refer_names;
    };
}


#endif //GAMTOOLS_SM_QC_RESULT_DATA_H
