//
// Created by ruoshui on 6/25/18.
//

#include "qc_result_data.h"


namespace gamtools {

    void QCStatResult::Add(const gamtools::QCStatResult &lhs) {
         reads_num += lhs.reads_num;
         bases_num += lhs.bases_num;
         mapq10_reads_num += lhs.mapq10_reads_num;
         mapq10_bases_num += lhs.mapq10_bases_num;
         coverage_pos += lhs.coverage_pos;
         depth += lhs.depth;
         for (int i = 0; i < kMaxDepth; ++i) {
             depth_dist[i] += lhs.depth_dist[i];
         }
    }

    void QCMappingResult::Add(const QCMappingResult &lhs) {
         dup_reads_num += lhs.dup_reads_num;
         total_reads_num += lhs.total_reads_num;
         total_bases_num += lhs.total_bases_num;
         map_reads_num += lhs.map_reads_num;
         map_bases_num += lhs.map_bases_num;
         mapq10_reads_num += lhs.mapq10_reads_num;
         mapq10_bases_num += lhs.mapq10_bases_num;
    }
}