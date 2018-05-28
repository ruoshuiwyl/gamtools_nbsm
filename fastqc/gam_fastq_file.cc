//
// Created by ruoshui on 5/10/18.
//

#include <cassert>
#include <bwa_mem/gam_read.h>
#include <cstring>
#include <util/nbsm_static.h>
#include "gam_fastq_file.h"
#include "util/gam_read_buffer.h"


namespace gamtools {
    GAMFastqFile::GAMFastqFile(const GAMFastqFileInfo &fq_info)
            :fq_info_(fq_info) {
        fastq_1_file_ = gzopen(fq_info.fastq1_filename.c_str(), "r");
        fastq_2_file_ = gzopen(fq_info.fastq2_filename.c_str(), "r");
        assert( fastq_1_file_ );
        assert( fastq_2_file_ );
    }

    GAMFastqFile::~GAMFastqFile() {
        gzclose(fastq_1_file_);
        gzclose(fastq_2_file_);
    }

    std::unique_ptr<GAMReadBuffer> GAMFastqFile::ReadFastq(int batch_size) {
        int current_size = 0;
        std::unique_ptr<GAMReadBuffer> read_buffer(new GAMReadBuffer(fq_info_.lane_id));
        while (current_size < batch_size) {
            GAMRead* read1 = ReadOneFastq(fastq_1_file_);
            GAMRead* read2 = ReadOneFastq(fastq_2_file_);
            if (read1 && read2) {
                read1->read_id = ReadID::gTotalReadID();
                read2->read_id = ReadID::gTotalReadID();
                ReadID::gIncreamReadID();
                read_buffer->AddPERead(read1, read2);
                current_size += 2;
            } else {
                if (read_buffer->size() > 0) {
                    return read_buffer;
                } else {
                    return nullptr;
                }
            }
        }
        return read_buffer;
    }

    static int kMaxLineLength = 512;
    GAMRead* GAMFastqFile::ReadOneFastq(gzFile &fastq_file) {
        char data[kMaxLineLength];
        if (nullptr == gzgets(fastq_file, data, kMaxLineLength)) {
            return nullptr;
        }
        GAMRead *read =(GAMRead *)calloc(1, sizeof(GAMRead));
        read->lane_id = fq_info_.lane_id;
        read->lib_id  = fq_info_.lib_id;

        int l_name = strlen(data) - 1;
        if (data[l_name-2] == '/' && isdigit(data[l_name - 1])) { //remove readname index
            l_name -= 2;
            data[l_name] = 0;
        } else {
            data[l_name] = 0;
        }
        read->name = (char *) malloc (l_name);
        memcpy(read->name, data + 1, l_name);
        assert(gzgets(fastq_file, data, kMaxLineLength)) ; // read seq
        read->l_seq = strlen(data) - 1;
        data[read->l_seq] = 0;
        read->seq = (char *)malloc(read->l_seq + 1);
        memcpy(read->seq, data, read->l_seq + 1);
        assert(gzgets(fastq_file, data, kMaxLineLength));//skip commits
        assert(gzgets(fastq_file, data, kMaxLineLength) ); // read qual
        assert(read->l_seq == strlen(data) - 1);
        data[read->l_seq] = 0;
        read->qual = (char *)malloc(read->l_seq + 1);
        memcpy(read->qual, data, read->l_seq + 1);
        return read;
    }
}