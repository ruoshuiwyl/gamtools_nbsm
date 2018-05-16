//
// Created by ruoshui on 5/8/18.
//

#ifndef BWA_0_7_15_GAM_READ_H
#define BWA_0_7_15_GAM_READ_H

#include <stdint.h>

typedef struct {
    int32_t tid;
    int32_t pos;
    uint32_t bin:16, qual:8, l_qname:8;
    uint32_t flag:16, n_cigar:16;
    int32_t l_qseq;
    int32_t mtid;
    int32_t mpos;
    int32_t isize;
} gam1_core_t;

struct GAMRead {
        int is_clean;
        int l_seq;    // read_len
        int bam_num;  // bam number
        int lane_id;  // lane_id
        int lib_id;   //
        int id;
        int64_t read_id;
        char *name;
        char *comment;
        char *seq;
        char *qual;
        char *bam;
        char *dup;
};

typedef struct {
    int16_t tid:8;
    int16_t lib_id:7;
    int16_t orientation:1;
    int16_t score;
    int32_t pos;
    uint64_t read_id;
} read_end_t;


#endif //BWA_0_7_15_GAM_READ_H
