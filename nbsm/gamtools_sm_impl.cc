//
// Created by ruoshui on 4/23/18.
//

#include <htslib/sam.h>
#include <util/slice.h>
#include "gamtools_sm_impl.h"
#include "sm/options.h"
#include "sharding/bam_partition_data.h"
#include "sharding/bam_sharding_impl.h"
#include "mkdup/gam_mark_duplicate_impl.h"
#include "util/gam_read_buffer.h"
#include "bwa_mem/gam_read.h"

namespace gamtools {

    SMImpl::SMImpl(const bam_hdr_t *bam_hdr, const SmOptions &options,
                   const std::string &bam_file, Channel<std::unique_ptr<BWAReadBuffer>> &input)
            : bam_hdr_(bam_hdr),
              options_(options),
              bam_file_(bam_file),
              input_(input) {
        sharding_impl_ = std::unique_ptr<BAMShardingImpl>(new BAMShardingImpl(bam_hdr, options));
        markdup_impl_ = std::unique_ptr<GAMMarkDuplicateImpl>( new GAMMarkDuplicateImpl(bam_hdr, options));
        sharding_impl_->InitializeSharding();
        markdup_impl_->InitializeSharding();
    }

    void SMImpl::PutSortSlice(const Slice &slice) {
        sharding_impl_->Sharding(slice);
    }

    void SMImpl::ShardingMarkdupInfo(const char *read1_dup, const char *read2_dup) {
        markdup_impl_->StorePairEndRecord(read1_dup, read2_dup);
    }


    void SMImpl::OutputBAM() {
        sharding_impl_->FinishSharding();
        markdup_impl_->MarkDuplication(0);
        sharding_impl_->InitializeMergeSort(bam_file_);
        sharding_impl_->ReadGamBlock();
        sharding_impl_->MergeSort();
        sharding_impl_->FinishMergeSort();
    }

    std::thread SMImpl::spawn() {
        return std::thread(&SMImpl::ProcessSharding,  this);
    }

    void SMImpl::ProcessSharding() {
        std::unique_ptr<BWAReadBuffer> read_buffer;
        while ( input_.read(read_buffer)) {
            for (int i = 0; i < read_buffer->size; i += 2) {
                int gam_len = 0;
                const char *gam_data = read_buffer->seqs[i].bam;
                for (int j = 0; j < read_buffer->seqs[i].bam_num; ++j) {
                    gam_len = reinterpret_cast<const int32_t *>(gam_data)[5] + 20;
                    Slice slice(gam_data, gam_len);
                    PutSortSlice(slice);
                    gam_data += gam_len;
                }
                gam_len = 0;
                gam_data = read_buffer->seqs[i+1].bam;
                for (int j = 0; j < read_buffer->seqs[i+1].bam_num; ++j) {
                    gam_len = reinterpret_cast<const int32_t *>(gam_data)[5] + 20;
                    Slice slice(gam_data, gam_len);
                    PutSortSlice(slice);
                    gam_data += gam_len;
                }
                const char *read1_dup = read_buffer->seqs[i].dup;
                const char *read2_dup = read_buffer->seqs[i+1].dup;
                ShardingMarkdupInfo(read1_dup, read2_dup);
            }
        }
    }
}