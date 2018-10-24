//
// Created by ruoshui on 5/4/18.
//

#include <cmath>
#include <iostream>
#include <cstring>
#include <thread>

#include "util/boost/threadpool.hpp"

#include <nbsm/options.h>
#include <util/glogger.h>

#include "filter_processor.h"
#include "base_fastq_info.h"
#include "bwa_mem/gam_read.h"
#include "util/gam_read_buffer.h"

#include "fastqc/fastq_info_statistics.h"
#include "fastqc/gam_fastq_file.h"




using namespace std;
namespace gamtools {


    static unsigned char nst_nt4_table[256] = {
            4, 4, 4, 4,  4, 4, 4, 4,  4, 4, 4, 4,  4, 4, 4, 4,
            4, 4, 4, 4,  4, 4, 4, 4,  4, 4, 4, 4,  4, 4, 4, 4,
            4, 4, 4, 4,  4, 4, 4, 4,  4, 4, 4, 4,  4, 5 /*'-'*/, 4, 4,
            4, 4, 4, 4,  4, 4, 4, 4,  4, 4, 4, 4,  4, 4, 4, 4,
            4, 0, 4, 1,  4, 4, 4, 2,  4, 4, 4, 4,  4, 4, 4, 4,
            4, 4, 4, 4,  3, 4, 4, 4,  4, 4, 4, 4,  4, 4, 4, 4,
            4, 0, 4, 1,  4, 4, 4, 2,  4, 4, 4, 4,  4, 4, 4, 4,
            4, 4, 4, 4,  3, 4, 4, 4,  4, 4, 4, 4,  4, 4, 4, 4,
            4, 4, 4, 4,  4, 4, 4, 4,  4, 4, 4, 4,  4, 4, 4, 4,
            4, 4, 4, 4,  4, 4, 4, 4,  4, 4, 4, 4,  4, 4, 4, 4,
            4, 4, 4, 4,  4, 4, 4, 4,  4, 4, 4, 4,  4, 4, 4, 4,
            4, 4, 4, 4,  4, 4, 4, 4,  4, 4, 4, 4,  4, 4, 4, 4,
            4, 4, 4, 4,  4, 4, 4, 4,  4, 4, 4, 4,  4, 4, 4, 4,
            4, 4, 4, 4,  4, 4, 4, 4,  4, 4, 4, 4,  4, 4, 4, 4,
            4, 4, 4, 4,  4, 4, 4, 4,  4, 4, 4, 4,  4, 4, 4, 4,
            4, 4, 4, 4,  4, 4, 4, 4,  4, 4, 4, 4,  4, 4, 4, 4
    };

    FilterProcessor::FilterProcessor(const FilterOptions &filter_options,
                                     BlockQueue <unique_ptr<GAMReadBuffer>> &fastq_channel,
                                     BlockQueue <unique_ptr<BWAReadBuffer>> &bwamem_channel,
                                     const int bwamem_batch_size,
                                     std::unique_ptr<FastqInfoStatistics> &&fastq_info_stats)
            : fastq_channel_(fastq_channel),
              bwamem_channel_(bwamem_channel),
              bwamem_batch_size_(bwamem_batch_size),
              fastq_info_stats_(std::move(fastq_info_stats)){
        thread_num_ = filter_options.filter_thread_num;
        filterAdapter_ = filter_options.filter_adapter;
        isAdptList_ = filter_options.is_adpt_list;

        if (!isAdptList_) {
            adapter1_ = filter_options.adapter1;
            adapter2_ = filter_options.adapter2;
            adapterLen1_ = adapter1_.size();
            adapterLen2_ = adapter2_.size();
        }

        lowQual_ = filter_options.low_qual;    //low quality
        qualRate_ = filter_options.qual_rate;
        nRate_ = filter_options.n_base_rate;
        qualSys_ = filter_options.qual_sys;
        misMatch_= filter_options.adapter_mis_match;
        matchRatio_ = filter_options.adapter_match_ratio;


    }

    std::thread FilterProcessor::spawn() {
        return std::thread(&FilterProcessor::Filter, this);
    }

    void FilterProcessor::Filter() {
        std::unique_ptr<GAMReadBuffer> read_buffer;
        std::unique_ptr<BWAReadBuffer> bwa_read_buffer(new BWAReadBuffer(bwamem_batch_size_));
        boost::threadpool::pool pl(thread_num_);
        while (fastq_channel_.read(read_buffer)) {
            int batch_size = read_buffer->size() / thread_num_;
            int index = 0;
            for (int i = 0; i < thread_num_; ++i) {
                int start = index;
                int end = index + batch_size > read_buffer->size()? read_buffer->size() : index + batch_size;
                index = end;
                pl.schedule([&, start, end]{
                    BatchFilter(read_buffer, start, end);
                });

            }
            pl.wait();
            if (!read_buffer->check()) {
                GLOG_ERROR << "Read Staticics Error"  ;
            }
            fastq_info_stats_->AddBatchFastqInfo(read_buffer->lane_id(), read_buffer->fq_info1(),read_buffer->fq_info2());
            for (int i = 0; i < read_buffer->size(); ++i) {
                auto read1 = read_buffer->read1(i);
                auto read2 = read_buffer->read2(i);
                if (read1->is_clean && read2->is_clean) {
                    if (!bwa_read_buffer->AddPairEndRead(read1, read2)) {
                        bwamem_channel_.write(std::move(bwa_read_buffer));
                        GLOG_TRACE << "Input BWA MEM Read Queue size: " << bwamem_channel_.size();
                        bwa_read_buffer = std::unique_ptr<BWAReadBuffer>(new BWAReadBuffer(bwamem_batch_size_));
                        bwa_read_buffer->AddPairEndRead(read1, read2);
                    }
                    free(read1);
                    free(read2);
                } else { // free not clean
                    gam_read_destory(read1);
                    gam_read_destory(read2);
                }
            }

        }
        if (fastq_channel_.eof()) {
            bwamem_channel_.write(std::move(bwa_read_buffer));
            bwamem_channel_.SendEof();
            fastq_info_stats_->OutputFastqInfo();
        }
        pl.clear();
    }



    void FilterProcessor::BatchFilter(std::unique_ptr<GAMReadBuffer> &read_buffer, int start, int end) {
        GLOG_DEBUG << "Start batch filter " << start << ":" << end;
        BaseFqInfo info1, info2;
        for (int idx = start; idx < end; ++idx) {
            auto read1 = read_buffer->read1(idx);
            auto read2 = read_buffer->read2(idx);
            statisticsPE(read1, read2, info1, info2);
        }
        {
            std::lock_guard<std::mutex> lock(mtx_);
            read_buffer->AddFastqInfo(info1, info2);
        }

    }



    bool FilterProcessor::statisticsPE(GAMRead *read1, GAMRead *read2, BaseFqInfo &info1, BaseFqInfo &info2) {
        StatisResult sr1, sr2;
        adaptorIndex(read1, adapter1_, adapterLen1_, readsName1_, sr1);
        adaptorIndex(read2, adapter2_, adapterLen2_, readsName2_, sr2);
        StatisInfo si1 = auxStatistics(read1, adapter1_, adapterLen1_, info1, sr1);
        StatisInfo si2 = auxStatistics(read2, adapter2_, adapterLen2_, info2, sr2);
        bool clean = true;
        if (sr1.hasAdpt || sr2.hasAdpt) {
            clean = false;
            info1.total_adapter_num++;
            if (sr1.hasAdpt) {
                info1.adapter_num++;
            }
            if (sr2.hasAdpt) {
                info2.adapter_num++;
            }
        }

        if ( clean && (sr1.nExceed || sr2.nExceed)) {
            clean = false;
            info1.total_n_excess_num++;
            if (sr1.nExceed) {
                info1.n_excess_num++;
            }
            if (sr2.nExceed) {
                info2.n_excess_num++;
            }
        }

        if (clean && (sr1.isLowQual || sr2.isLowQual)) {
            clean = false;
            info1.total_low_qual_num++;
            if (sr1.isLowQual) {
                info1.low_qual_num++;
            }
            if (sr2.isLowQual) {
                info2.low_qual_num++;
            }
        }
        if (clean) {
            info1.clean_base_gc += si1.ACGTN[C_] + si1.ACGTN[G_];
            info1.clean_base_n += si1.ACGTN[N_];
            info1.clean_q20 += si1.q20;
            info1.clean_q30 += si1.q30;
            info1.clean_read_num++;
            info1.clean_base_num += si1.readLen;

            info2.clean_base_gc += si2.ACGTN[C_] + si2.ACGTN[G_];
            info2.clean_base_n += si2.ACGTN[N_];
            info2.clean_q20 += si2.q20;
            info2.clean_q30 += si2.q30;
            info2.clean_read_num++;
            info2.clean_base_num += si2.readLen;
        }
        read1->is_clean = clean;
        read2->is_clean = clean;
        return clean;

    }



    bool FilterProcessor::hasAdapter(std::set<std::string> &readsName, const char *seqName)  {
        int i = 0;
        while (seqName[i++] != '/')
            ;
        return readsName.count(string(seqName + 1, i-2));
    }


    int FilterProcessor::adaptorIndex(GAMRead *read, std::string &adapter, int adpLen,
                                      std::set<std::string> &readsName, gamtools::StatisResult &sr) {
        int readLen = read->l_seq;
        int index = -1;
        if(filterAdapter_) {
            if(isAdptList_)  {
                sr.hasAdpt = hasAdapter(readsName,read->name);
            } else {
                index = hasAdapter(read->seq, readLen, adapter.c_str(), adpLen);
                if(index != -1) {
                    sr.hasAdpt = true;
                }
            }
        }
        return index;
    }

    StatisInfo FilterProcessor::auxStatistics(GAMRead *read, std::string &adapter, int adptLen, BaseFqInfo &info,
                                                  gamtools::StatisResult &sr) {
        int qual;
        int readLen = read->l_seq;
        info.raw_base_num += readLen;
        info.raw_read_num++;
        StatisInfo si;
//        int a = 0, g = 0, c = 0, t = 0, n = 0;
        int q20 = 0, q30 = 0;
//        int right = readLen;
//        int sumQual = 0;
//        map<char, int> longX;
//        longX['A'] = longX['C'] = longX['G'] = longX['T'] = longX['N'] = 0;
//        map<char, int> tmplongX;
//        tmplongX['A'] = tmplongX['C'] = tmplongX['G'] = tmplongX['T'] = tmplongX['N'] = 0;
//        char lastBase = read->baseSequence[0];

        for (int i=0; i<readLen; ++i) {
            char base = nst_nt4_table[read->seq[i]]; //convert to 2-bit encoding
            si.ACGTN[base]++;
            qual = read->qual[i] - qualSys_;
            read->qual[i] = qual;
//            info.qual[i][qual]++;
            if (qual <= lowQual_) {
                si.lowQual++;
            } else if (qual >= 20) {
                q20++;
                si.q20++;
//                info.q20q30[i][0]++;
                if (qual >= 30) {
                    q30++;
                    si.q30++;
//                    info.q20q30[i][1]++;
                }
            }

        }

//        int polyXnum = max(max(longX['A'],longX['C']),max(longX['G'],longX['T']));
//        sr.isPolyX = polyX_ > 0 && polyXnum >= polyX_;

        info.raw_base_gc += si.ACGTN[C_] + si.ACGTN[G_];
        info.raw_base_n += si.ACGTN[N_];
        info.raw_q20 += q20;
        info.raw_q30 += q30;


        si.readLen = readLen;
        sr.nExceed = (si.ACGTN[N_] >= readLen * nRate_);
        sr.isLowQual = (si.lowQual >= readLen * qualRate_);
        return si;
    }

    int FilterProcessor::hasAdapter(const char *sequence, int read_len, const char *adapter, int adpt_len) {
        int r1, mis, misMatchTemp;
        int left = read_len - adpt_len;

        for (r1 = 0; r1 <= left; ++r1) {
            int mis = 0;
            for (int c = 0; c < adpt_len; ++c) {
                if (adapter[c] != sequence[r1 + c]) {
                    mis++;
                    if (mis > misMatch_) {
                        break;
                    }
                }
            }
            if (mis <= misMatch_)
                return r1;
        }

        int minMatchLen = (int) ceil(adpt_len * matchRatio_);

        for (r1 = 1; r1 <= adpt_len - minMatchLen + 1; ++r1) {
            mis = 0;
            misMatchTemp = misMatch_ - (r1 * (misMatch_ + 1) - 1) / (adpt_len - minMatchLen);
            for (int c = 0; c <= adpt_len - r1; ++c) {
                if (adapter[c] != sequence[left + r1 + c])
                    mis++;
            }

            if (mis <= misMatchTemp)
                return left + r1;
        }

        return -1;
    }

}