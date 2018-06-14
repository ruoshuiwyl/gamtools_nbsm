//
// Created by ruoshui on 6/14/18.
//

#ifndef GAMTOOLS_SM_TARGET_STAT_H
#define GAMTOOLS_SM_TARGET_STAT_H


#include <util/slice.h>
#include <vector>

namespace gamtools {

    typedef std::tuple<int, int, int> bed_t;
    struct StatisticsSlice {
        StatisticsSlice(const Slice &s) {
            tid = s.data()[0];
            pos = s.data()[1];
            rlen = 0;
            qlen = 0;
            mapq = 0;

        }
        int tid;
        int pos;
        int rlen;
        int qlen;
        int mapq; //
    };
    class BaseStat {
    public:
        BaseStat(const std::string &ref_idx_file): ref_filename_(ref_idx_file){}
        virtual void StatisticsRead(const StatisticsSlice &stat)  = 0;
        virtual void StatisticsDepth(int tid, int pos, int depth) = 0;
        virtual void Init() = 0;
        virtual std::string Report() = 0;
        virtual ~BaseStat();
        inline bool HasStat(int tid) {
            return stat_chr_.count(tid) != 0;
        }
    protected:
        void ReadReferIndex();
        std::string ref_filename_;
        std::set<int> stat_chr_;
        std::map<std::string, int> refer_dict_;
        std::vector<int> refer_lens_;
    };

    class TargetStat : public BaseStat {
    public:
        TargetStat(const std::string &bed_file, const std::string &ref_idx_file);
        void Init();
        void StatisticsRead(const StatisticsSlice &stat);
        void StatisticsDepth(int tid, int pos, int depth);
        std::string Report();
    private:
        std::string bed_filename_;
        int target_read_idx_, flank_read_idx_;
        int target_read_reg_, flank_read_reg_;
        int target_depth_idx_, flank_depth_idx_;
        int target_depth_reg_, flank_depth_reg_;
        std::vector<std::vector<bed_t>> target_region_, flank_region_;
        std::vector<int> target_chr_lens_, flank_chr_lens_;
        std::vector<std::vector<int64_t>> target_depth_, flank_depth_; // static depth
        std::vector<int64_t> target_coverage_, flank_coverage_;
        std::vector<int64_t> target_depth_stat_, flank_depth_stat_;
        std::vector<int64_t> target_reads_, target_bases_;
        std::vector<int64_t> flank_reads_, flank_bases_;
        std::vector<int64_t> mapq10_target_reads_, mapq10_target_bases_;
        std::vector<int64_t> mapq10_flank_reads_,  mapq10_flank_bases_;
        int flank_extend = 200;
        int kMapq = 10;
        int kMaxDepth = 8192;

    };

    class WGSStat : public BaseStat {
    public :
        WGSStat(const std::string &ref_idx_file);
        void Init();
        void StatisticsRead(const StatisticsSlice &stat);
        void StatisticsDepth(int tid, int pos, int depth);
        std::string Report();
    private:
        std::vector<int64_t> target_coverage_;
        std::vector<int64_t> depth_stat_;
        std::vector<int64_t> mapq10_target_reads_, target_reads_;
        std::vector<int64_t> mapq10_target_bases_, target_bases_;
        std::vector<std::vector<int64_t>> target_depth_;

        int kMaxDepth = 8192;
        int kMapq = 10;
    };
}


#endif //GAMTOOLS_SM_TARGET_STAT_H
