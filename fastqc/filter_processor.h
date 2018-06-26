//
// Created by ruoshui on 5/4/18.
//

#ifndef GAMTOOLS_SM_FILTER_PROCESSOR_H
#define GAMTOOLS_SM_FILTER_PROCESSOR_H

#include <string>
#include <set>
#include <strings.h>
#include <nbsm/options.h>
#include <util/array_block_queue.h>

#include "base_fastq_info.h"
#include "util/gam_read_buffer.h"
#include "bwa_mem/gam_read.h"

namespace gamtools {
    enum Base  {
        A_ = 0, C_, G_, T_, N_,
    };
    struct StatisResult  {
        bool hasAdpt;
        bool isLowQual;
        bool nExceed;
//        bool isPolyA;
//        bool isPolyX;
        //由于pe时，需要read1和read2的平均质量值
//        int  sumQuality;

        StatisResult()
        {
            hasAdpt = false;
            isLowQual = false;
            nExceed = false;
//            isPolyA = false;
//            isPolyX = false;
//            sumQuality = 0;
        }
    };

    struct StatisInfo  {
        int q20, q30;
//        int a, c, g, t, n;
        int ACGTN[5];
        int lowQual;
        int readLen;
        StatisInfo()
        {
            bzero(this, sizeof(StatisInfo));
        }
    };


    class GAMReadBuffer;
    class FastqInfoStatistics;
    class FilterProcessor {
    public:
        explicit FilterProcessor(const FilterOptions &filter_options,
                                 BlockQueue <std::unique_ptr<GAMReadBuffer>> &fastq_channel,
                                 BlockQueue <std::unique_ptr<BWAReadBuffer>> &bwamem_channel,
                                 const int bwamem_batch_size,
                                 std::unique_ptr<FastqInfoStatistics> &&fastq_info_stats);

        std::thread spawn();
        void Filter();

    private:
//        void calculateBaseDistribute(GAMRead* read, FqInfo &info, int readLen);
        void BatchFilter(std::unique_ptr<GAMReadBuffer> &read_buffer, int start, int end);
        bool statisticsPE(GAMRead *read1, GAMRead *read2, BaseFqInfo &info1, BaseFqInfo &info2);

        int adaptorIndex(GAMRead *read, std::string &adapter, int adpLen, std::set<std::string> &readsName,
                         StatisResult &sr);


        StatisInfo auxStatistics(GAMRead *read, std::string &adapter, int adptLen, BaseFqInfo &info,
                                         gamtools::StatisResult &sr);


        int hasAdapter(const char *sequence, int read_len, const char *adapter, int adpt_len);
        bool hasAdapter(std::set<std::string> &readsName, const char *seqName);
        int thread_num_;
        bool filterAdapter_;
        bool isAdptList_;
        std::set<std::string> readsName1_;   //store the read's name which in adapter list file
        std::set<std::string> readsName2_;
        std::string adapter1_; // store the adapter seq
        std::string adapter2_;
        int adapterLen1_;
        int adapterLen2_;
        int lowQual_;    //low quality
        float qualRate_;
        float nRate_;
        int misMatch_;
        float matchRatio_;
        int qualSys_ ;
        std::mutex mtx_;
        int bwamem_batch_size_;
        BlockQueue<std::unique_ptr<GAMReadBuffer>> &fastq_channel_;
        BlockQueue <std::unique_ptr<BWAReadBuffer>> &bwamem_channel_;
        std::unique_ptr<FastqInfoStatistics> fastq_info_stats_;
    };

}


#endif //GAMTOOLS_SM_FILTER_PROCESSOR_H
