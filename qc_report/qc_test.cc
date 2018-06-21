//
// Created by ruoshui on 6/14/18.
//



#include <memory>
#include "quality_control.h"
using namespace gamtools;

//std::vector<int> tids = {1,2,3,4,5,6,7,8,  9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24};
std::vector<int> tids = {0, 1, 2, 3};
std::vector<int> freqs = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 13, 15, 17, 19, 21, 23, 25, 27, 29, 31, 33, 35, 37};
std::vector<int> poss = {30, 200, 300, 500, 700, 900, 1100,
                         1300, 1500, 1700, 1900, 2100, 2300, 2500, 2700, 2800  };


void wes_stat() {
//    std::string ref_file = "/home/ruoshui/data/ucsc.hg19/ucsc.hg19/ucsc.hg19.fasta.fai";
    std::string ref_file = "/home/ruoshui/test_data/test.fa.fai";
    std::string bed_file = "/home/ruoshui/test_data/test.bed";
//    std::string bed_file = "/home/ruoshui/data/ucsc.hg19/ex_region.sort.bed";
    std::string report_file = "test_wes.report";
    std::unique_ptr<QualityControl> qc_report(new QualityControl(ref_file, bed_file,  report_file));
    qc_report->Init();
    {
        int cnt = 0;
        StatisticsSlice slice_stat;
        for (auto tid : tids) {
            slice_stat.tid = tid;
            for (auto pos : poss) {
//            for (int pos = 30; pos < 10000; pos += ((cnt % 3 + 1) * 50)){
                for (auto freq : freqs) {
                    cnt++;
                    slice_stat.pos = pos + freq;
                    slice_stat.qlen = 100;
                    slice_stat.rlen = 100;
                    slice_stat.mapq = cnt % 20 == 19 ? 9 : 60;
                    slice_stat.is_dup = cnt % 60 == 59;
                    qc_report->Statistics(slice_stat);
                }
            }
        }
    }
    qc_report->Report();
}

void wgs_stat() {
//    std::string ref_file = "/home/ruoshui/data/ucsc.hg19/ucsc.hg19/ucsc.hg19.fasta.fai";
    std::string ref_file = "/home/ruoshui/test_data/test.fa.fai";
    std::string report_file = "test_wgs.report";
    std::unique_ptr<QualityControl> qc_report(new QualityControl(ref_file, report_file));
    qc_report->Init();
    {
        int cnt = 0;
        StatisticsSlice slice_stat;
        for (auto tid : tids) {
            slice_stat.tid = tid;
            for (auto pos : poss){
//            for (int pos = 30; pos < 40000; pos += ((cnt % 3 + 1) * 100)){
                for (auto freq : freqs) {
                    cnt++;
                    slice_stat.pos = pos + freq;
                    slice_stat.qlen = 100;
                    slice_stat.rlen = 100;
                    slice_stat.mapq = cnt % 20 == 19 ? 9 : 60;
                    slice_stat.is_dup = cnt % 60 == 59;
                    qc_report->Statistics(slice_stat);
                }
            }
        }
    }
    qc_report->Report();
}

int main() {
    wgs_stat();
    wes_stat();


    return 0;
}