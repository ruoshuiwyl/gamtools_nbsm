//
// Created by ruoshui on 6/13/18.
//

#ifndef GAMTOOLS_SM_QUALITY_CONTROL_H
#define GAMTOOLS_SM_QUALITY_CONTROL_H


#include <string>
#include <map>
#include <vector>
#include <util/slice.h>

namespace gamtools {
    typedef std::tuple<int, int, int> bed_t;


    struct StatisticsSlice {
        StatisticsSlice(const Slice &s):slice(s) {

        }

        int tid;
        int pos;
        int rlen;
        int qlen;
        int mapq;
        const Slice &slice;
    };


    class QualityControl {
    public:
        void Init();
        void Statistics(const Slice &slice);
        void Report();
    private:


        std::list<int> stat_list_;
        int curr_idx_;
        int curr_pos_;
        bool target_;
        std::set<int> stat_chr_;
        std::map<std::string, int> refer_dict_;
        std::vector<int> refer_lens_;
        std::vector<std::vector<bed_t>> target_region_, flank_region_;
        std::vector<int> target_chr_lens_, flank_chr_lens_;
        std::vector<std::vector<int64_t>> target_depth_, flank_depth_;
        int flank_extend = 200;
        std::string bed_filename_;
        std::string ref_filename_;
    };
}


#endif //GAMTOOLS_SM_QUALITY_CONTROL_H
