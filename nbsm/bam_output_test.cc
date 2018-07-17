//
// Created by ruoshui on 7/16/18.
//



#include <hts/htslib/hts.h>
#include <hts/htslib/sam.h>
#include <string>
#include <hts/htslib/bgzf.h>


int main() {

    std::string in_bam_file = "/home/ruoshui/data/na12878/chr21_5w/test/chr21_9411000_9459000_name_id_sort.bam";
    std::string out_bam_file = "/home/ruoshui/data/na12878/chr21_5w/test/out.bam";
    htsFile *out = hts_open_format(out_bam_file.c_str(), "wb", nullptr);
    if (hts_set_threads(out, 2) != 0) {
        perror ( "Set Threads error");
    }

    samFile *in = sam_open_format(in_bam_file.c_str(), "r", NULL);

    bam1_t *b= bam_init1();
    bam_hdr_t *header =  sam_hdr_read(in) ;
    if (header != nullptr) {
        if (sam_hdr_write(out, header) != 0) {
            perror ("Write bam header Error");
        }
    }
    int r;
    while (( r= sam_read1(in, header, b)) >= 0) {
//        if (sam_write1(out, header, b) < 0){
//            perror ("Write bam Error");
//        }

        int size = b->l_data + 36;
        int ok = (bgzf_flush_try(out->fp.bgzf, size) >= 0);
        if (ok) {
            size -= 4;
            ok = bgzf_write(out->fp.bgzf, (char *)&(size)  ,4); // write bam length
        }
        if (ok) {
            int x[8];
            bam1_core_t *c = &b->core;
            x[0] = c->tid;
            x[1] = c->pos;
            x[2] = (uint32_t)c->bin<<16 | c->qual<<8 | c->l_qname;
            x[3] = (uint32_t)c->flag<<16 | c->n_cigar;
            x[4] = c->l_qseq;
            x[5] = c->mtid;
            x[6] = c->mpos;
            x[7] = c->isize;
            ok = bgzf_write(out->fp.bgzf, x , 32); // write bam core data 32B
        }
        if (ok) {
            // write bam data variant length

            ok = bgzf_write(out->fp.bgzf, b->data, b->l_data);
        }
        if (!ok) {
            perror( "bgzf write error");
        }
    }
    sam_close(in);
    sam_close(out);


    return 0;
}