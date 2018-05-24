//
// Created by ruoshui on 5/24/18.
//

#ifndef GAMTOOLS_SM_FASTQ_INFO_STATISTICS_H
#define GAMTOOLS_SM_FASTQ_INFO_STATISTICS_H

#include <vector>
#include <string>

namespace gamtools {
    struct BaseFqInfo;
    class GAMFastqFileInfo;
    class FastqInfoStatistics {
    public:
        FastqInfoStatistics(int lane_cnt_, const std::string &filename, std::vector<GAMFastqFileInfo> &fastq_file_lists);
        void AddBatchFastqInfo(int lane_id, const BaseFqInfo &baseFqInfo1, const BaseFqInfo &baseFqInfo2);
        void OutputFastqInfo();

    private:
        std::vector<std::pair<BaseFqInfo, BaseFqInfo> > fastq_infos_;
        std::vector<GAMFastqFileInfo> &fastq_file_lists_;
        std::string file_name_;
        int lane_cnt_;
    };
}


#endif //GAMTOOLS_SM_FASTQ_INFO_STATISTICS_H
