//
// Created by ruoshui on 5/7/18.
//

#include <iomanip>
#include "base_fastq_info.h"
#include "string"
#include "sstream"


namespace  gamtools {


    void BaseFqInfo::Add(const gamtools::BaseFqInfo &base_fq_info) {
        raw_read_num += base_fq_info.raw_read_num;
        clean_read_num += base_fq_info.clean_read_num;

        raw_base_num += base_fq_info.raw_base_num;
        clean_base_num += base_fq_info.clean_base_num;


        raw_base_n += base_fq_info.raw_base_n;
        clean_base_n += base_fq_info.clean_base_n;


        raw_base_gc += base_fq_info.raw_base_gc;
        clean_base_gc += base_fq_info.clean_base_gc;


        raw_q20 += base_fq_info.raw_q20;
        clean_q20 += base_fq_info.clean_q20;


        raw_q30 += base_fq_info.raw_q30;
        clean_q30 += base_fq_info.clean_q30;

        adapter_num += base_fq_info.adapter_num;
        n_excess_num += base_fq_info.n_excess_num;
        low_qual_num += base_fq_info.low_qual_num;
    }


    std::string PrintBaseFqInfo(const std::string &head, const BaseFqInfo *fq_info1, const BaseFqInfo *fq_info2){
        std::stringstream oss;
        oss << head;
        if (fq_info2 != nullptr) { // PE
            oss << "Number of Reads:\t " << fq_info1->raw_read_num + fq_info2->raw_read_num <<"\t " << fq_info1->clean_read_num +  fq_info2->clean_read_num << std::endl;
            oss << "Data Size:\t " << fq_info1->raw_base_num + fq_info2->raw_base_num << "\t " << fq_info1->clean_base_num + fq_info2->clean_base_num << std::endl;
            oss << "N of fq1:\t " << fq_info1->raw_base_n << "\t " << fq_info1->clean_base_n << std::endl;
            oss << "N of fq2:\t " << fq_info2->raw_base_n << "\t " << fq_info2->clean_base_n << std::endl;
            oss << "CG(%) of fq1:\t "  << std::setprecision(4) <<  (float)fq_info1->raw_base_gc / fq_info1->raw_base_num << "\t " <<   (float)fq_info1->clean_base_gc / fq_info1->clean_base_num << std::endl;
            oss << "CG(%) of fq2:\t "  << std::setprecision(4) << (float)fq_info2->raw_base_gc / fq_info2->raw_base_num << "\t " <<   (float)fq_info2->clean_base_gc / fq_info2->clean_base_num << std::endl;
            oss << "Q20(%) of fq1:\t " << std::setprecision(4) << (float)fq_info1->raw_q20 / fq_info1->raw_base_num << "\t " << (float)fq_info1->clean_q20 / fq_info1->clean_base_num << std::endl;
            oss << "Q20(%) of fq2:\t " << std::setprecision(4) << (float)fq_info2->raw_q20 / fq_info2->raw_base_num << "\t " << (float)fq_info2->clean_q20 / fq_info2->clean_base_num << std::endl;
            oss << "Q30(%) of fq1:\t " << std::setprecision(4) << (float)fq_info1->raw_q30 / fq_info1->raw_base_num << "\t " << (float)fq_info1->clean_q30 / fq_info1->clean_base_num << std::endl;
            oss << "Q30(%) of fq2:\t " << std::setprecision(4) << (float)fq_info2->raw_q30 / fq_info2->raw_base_num << "\t " << (float)fq_info2->clean_q30 / fq_info2->clean_base_num << std::endl;
            oss << "Dicard Reads related to N:\t " << fq_info1->total_n_excess_num * 2<< std::endl;
            oss << "Dicard Reads related to low qual:\t " << fq_info1->total_low_qual_num * 2 << std::endl;
            oss << "Dicard Reads related to Adapter:\t " << fq_info1->total_adapter_num * 2 << std::endl;
        } else { // SE
            oss << "Number of Reads:\t " << fq_info1->raw_read_num + fq_info2->raw_read_num <<"\t " << fq_info1->clean_read_num +  fq_info2->clean_read_num << std::endl;
            oss << "Data Size:\t " << fq_info1->raw_base_num + fq_info2->raw_base_num << "\t " << fq_info1->clean_base_num + fq_info2->clean_base_num << std::endl;
            oss << "N of fq1:\t " << fq_info1->raw_base_n << "\t " << fq_info1->clean_base_n << std::endl;
            oss << "CG(%) of fq1:\t "  << std::setprecision(4) << (float)fq_info1->raw_base_gc / fq_info1->raw_base_num << "\t " <<   (float)fq_info1->clean_base_gc / fq_info1->clean_base_num << std::endl;
            oss << "Q20(%) of fq1:\t " << std::setprecision(4) << (float)fq_info1->raw_q20 / fq_info1->raw_base_num << "\t " << (float)fq_info1->clean_q20 / fq_info1->clean_base_num << std::endl;
            oss << "Q30(%) of fq1:\t " << std::setprecision(4) << (float)fq_info1->raw_q30 / fq_info1->raw_base_num << "\t " << (float)fq_info1->clean_q30 / fq_info1->clean_base_num << std::endl;
            oss << "Dicard Reads related to N:\t " << fq_info1->total_n_excess_num << std::endl;
            oss << "Dicard Reads related to low qual:\t " << fq_info1->total_low_qual_num << std::endl;
            oss << "Dicard Reads related to Adapter:\t " << fq_info1->total_adapter_num << std::endl;
        }
        return oss.str();
    }
}