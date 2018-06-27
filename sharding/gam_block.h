//
// Created by ruoshui on 4/25/18.
//

#ifndef GAMTOOLS_SM_GAM_BLOCK_H
#define GAMTOOLS_SM_GAM_BLOCK_H

#include <util/arena.h>
#include <string>
#include <util/block.h>
#include <cstring>
#include <memory>
#include <util/skiplist.h>

#ifdef DEBUG
#include <hts/htslib/sam.h>
#endif

namespace gamtools {
    class Slice;
    bool GAMComparator(const Slice &a, const Slice &b);
    class GAMBlock : public Block {
    public:
        explicit GAMBlock(size_t block_size, bool append, int block_id, std::string file_name);


        ~GAMBlock();
        void Compress();
        bool Decompress();
        void BuildIterator();
        void Write();
        std::unique_ptr<GAMBlock> BlockSort();
    private:
        size_t GAMLen(const char *data);
        int BAMLen(const char *data);
        bool append_;
        bool compress_;
        int block_id_;
        std::string file_name_;
    };

//    const  size_t kGAMKeyLength = sizeof(uint64_t) * 3;
    inline size_t GAMBlock::GAMLen(const char *data) {
        return 24 + BAMLen(data);
    }

    inline int GAMBlock::BAMLen(const char *data) {
        int bam_len;
        memcpy((void *) &bam_len, data + 20, sizeof(bam_len));
        return bam_len;
    }
}


#endif //GAMTOOLS_SM_GAM_BLOCK_H
