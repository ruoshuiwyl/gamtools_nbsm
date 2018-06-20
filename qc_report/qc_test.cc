//
// Created by ruoshui on 6/14/18.
//



#include <memory>
#include "quality_control.h"
using namespace gamtools;

std::vector<int> tids = {1,2,3,4,5,6,7,8,  9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24};
std::vector<int> poss = {30, 40, 50, 70, 100, 30000, 30200, 30330, 40000, 50000, 60000};


void wes_stat() {
    std::string ref_file = "/home/ruoshui/data/ucsc.hg19/ucsc.hg19/ucsc.hg19.fasta.fai";
    std::string bed_file = "/home/ruoshui/data/ucsc.hg19/ex_region.sort.bed";
    std::string report_file = "test_wes.report";
    std::unique_ptr<QualityControl> qc_report(new QualityControl(ref_file, report_file));
    qc_report->Init();
    {
        int cnt = 0;
        StatisticsSlice slice_stat;
        for (auto tid : tids) {
            slice_stat.tid = tid;
            for (auto pos : poss) {
                cnt++;
                slice_stat.pos = pos;
                slice_stat.qlen = 100;
                slice_stat.rlen = 100;
                slice_stat.mapq = cnt % 20 == 19 ? 10 : 60;
                slice_stat.is_dup = cnt % 60 == 59 ;
                qc_report->Statistics(slice_stat);
            }
        }
    }
    qc_report->Report();
}

void wgs_stat() {
    std::string ref_file = "/home/ruoshui/data/ucsc.hg19/ucsc.hg19/ucsc.hg19.fasta.fai";
    std::string report_file = "test_wgs.qc";
    std::unique_ptr<QualityControl> qc_report(new QualityControl(ref_file, report_file));
    qc_report->Init();
    {
        int cnt = 0;
        StatisticsSlice slice_stat;
        for (auto tid : tids) {
            slice_stat.tid = tid;
            for (auto pos : poss) {
                cnt++;
                slice_stat.pos = pos;
                slice_stat.qlen = 100;
                slice_stat.rlen = 100;
                slice_stat.mapq = cnt % 20 == 19 ? 10 : 60;
                slice_stat.is_dup = cnt % 60 == 59 ;
                qc_report->Statistics(slice_stat);
            }
        }
    }
    qc_report->Report();
}

int main() {
    wgs_stat();


    return 0;
}