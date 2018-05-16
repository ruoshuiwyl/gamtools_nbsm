//
// Created by ruoshui on 5/10/18.
//

#ifndef GAMTOOLS_SM_GAM_FASTQ_FILE_H
#define GAMTOOLS_SM_GAM_FASTQ_FILE_H


#include <string>
#include <zlib.h>
#include <memory>
#include <bwa_mem/gam_read.h>
#include <util/gam_read_buffer.h>

namespace  gamtools {
//    class GAMReadBuffer;

    struct GAMFastqFileInfo{
        std::string fastq1_filename;
        std::string fastq2_filename;
        int lane_id ;
        int lib_id;
    };


    class GAMFastqFile {
    public:
        explicit GAMFastqFile(const GAMFastqFileInfo &fastq_info);
        ~GAMFastqFile();
        std::unique_ptr<GAMReadBuffer> ReadFastq(int batch_size);
    private:
        GAMRead *ReadOneFastq(gzFile &fastq_file);
        gzFile fastq_1_file_;
        gzFile fastq_2_file_;
        GAMFastqFileInfo fq_info_;
//        int lane_id_;
//        int lib_id_;
    };
}


#endif //GAMTOOLS_SM_GAM_FASTQ_FILE_H
