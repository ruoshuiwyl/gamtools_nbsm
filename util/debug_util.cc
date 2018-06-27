//
// Created by ruoshui on 5/23/18.
//

#include "debug_util.h"

#ifdef DEBUG
#include <hts/htslib/sam.h>
#include <cstring>
#include "glogger.h"
#endif

namespace gamtools {

#ifdef DEBUG
    bam_hdr_t *g_bam_hdr = nullptr;
    void DebugGAMSlice(const Slice &slice) {
        bam1_t *b = (bam1_t * ) calloc(1, sizeof(bam1_t));
        kstring_t str = {0, 0, nullptr};
        uint64_t pos = ((const uint64_t *)slice.data() )[0] >> 1 ;
        int64_t read_id = ((const int64_t *)slice.data() )[1];
        int32_t lib_id = ((const int *)slice.data())[4];
        int32_t bam_len = ((const int *)slice.data())[5];
        const int32_t *x = (const int32_t *)(slice.data() + 24);
        bam1_core_t *c = &b->core;
        c->tid = x[0];
        c->pos = x[1];
        c->bin = x[2]>>16; c->qual = x[2]>>8&0xff; c->l_qname = x[2]&0xff;
        c->flag = x[3]>>16; c->n_cigar = x[3]&0xffff;
        c->l_qseq = x[4];
        c->mtid = x[5];
        c->mpos = x[6];
        c->isize = x[7];
        b->l_data = bam_len - 32;
        b->data = (uint8_t *)malloc(b->l_data);
        memcpy(b->data, slice.data() + 56, b->l_data);
        sam_format1(g_bam_hdr, b, &str);
        GLOG_DEBUG << "GAMSlice:" << pos << "\t" << read_id << "\t" << lib_id << "\t" << str.s;
        free(b->data);
        free(str.s);
        free(b);
    }
    void DebugBAMSlice(const Slice &slice) {
        bam1_t *b = (bam1_t * ) calloc(1, sizeof(bam1_t));
        kstring_t str = {0, 0, nullptr};;
        int32_t bam_len = ((const int *)slice.data())[0];
        const int32_t *x = (const int32_t *)(slice.data() + 4);
        bam1_core_t *c = &b->core;
        c->tid = x[0];
        c->pos = x[1];
        c->bin = x[2]>>16; c->qual = x[2]>>8&0xff; c->l_qname = x[2]&0xff;
        c->flag = x[3]>>16; c->n_cigar = x[3]&0xffff;
        c->l_qseq = x[4];
        c->mtid = x[5];
        c->mpos = x[6];
        c->isize = x[7];
        b->l_data = bam_len - 32;
        b->data = (uint8_t *)malloc(b->l_data);
        memcpy(b->data, slice.data() + 36, b->l_data);
        sam_format1(g_bam_hdr, b, &str);
        GLOG_DEBUG << "BAMSlice:" << str.s;
        free(b->data);
        free(str.s);
        free(b);
    }
#endif
}