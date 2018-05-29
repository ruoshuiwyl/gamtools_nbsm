//
// Created by ruoshui on 5/7/18.
//

#ifndef GAMTOOLS_SM_GAM_READ_BUFFER_H
#define GAMTOOLS_SM_GAM_READ_BUFFER_H

#include <vector>
#include <list>
#include <bwa_mem/gam_read.h>
#include "fastqc/base_fastq_info.h"

namespace gamtools {

    class GAMReadBuffer {
    public:
        explicit GAMReadBuffer(int lane_id);
        void AddPERead(GAMRead *read1, GAMRead *read2);
//        void AddSERead1(GAMRead *read);
        GAMRead *read1(int index);
        GAMRead *read2(int index);
        inline const int size() {
            return size_;
        }
        ~GAMReadBuffer();
        void AddFastqInfo(BaseFqInfo &fq_info1, BaseFqInfo &fq_info2);
        const BaseFqInfo& fq_info1() const {
            return fq_info1_;
        };
        const BaseFqInfo& fq_info2() const {
            return fq_info1_;
        };
        const int lane_id() const {
            return lane_id_;
        }
    private:
        int size_;
        int lane_id_;
        std::vector<GAMRead *> reads1_;
        std::vector<GAMRead *> reads2_;
        BaseFqInfo fq_info1_;
        BaseFqInfo fq_info2_;
    };

    struct BWAReadBuffer {
        BWAReadBuffer(int batch);
        ~BWAReadBuffer();
        bool AddPairEndRead(GAMRead *read1, GAMRead *read2);
        bool empty();
        void GAMReadCopy(GAMRead *dst, const GAMRead *src);
        int size;
        int batch_size;
        GAMRead *seqs;
    };
}


#endif //GAMTOOLS_SM_GAM_READ_BUFFER_H
