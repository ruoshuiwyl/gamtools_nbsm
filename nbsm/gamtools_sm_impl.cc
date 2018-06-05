//
// Created by ruoshui on 4/23/18.
//

#include <htslib/sam.h>
#include <util/slice.h>
#include <util/glogger.h>
#include <util/nbsm_static.h>
#include <sharding/bam_sort_mkdup_impl.h>
#include "gamtools_sm_impl.h"
#include "options.h"
#include "sharding/bam_partition_data.h"
#include "sharding/bam_sharding_impl.h"
#include "mkdup/gam_mark_duplicate_impl.h"
#include "util/gam_read_buffer.h"
#include "bwa_mem/gam_read.h"


#ifdef DEBUG
#include "util/debug_util.h"
#endif
namespace gamtools {

    SMImpl::SMImpl(const bam_hdr_t *bam_hdr, const SMOptions &options,
                   const std::string &bam_file, ArrayBlockQueue<std::unique_ptr<BWAReadBuffer>> &input)
            : bam_hdr_(bam_hdr),
              options_(options),
              bam_file_(bam_file),
              input_(input),
              finish_markdup_flag_(false) {
        sharding_impl_ = std::unique_ptr<BAMShardingImpl>(new BAMShardingImpl(bam_hdr, options));
        markdup_impl_ = std::unique_ptr<GAMMarkDuplicateImpl>( new GAMMarkDuplicateImpl(bam_hdr, options));
        sharding_impl_->InitializeSharding();
        markdup_impl_->InitializeSharding();
    }
//
//    void SMImpl::PutSortSlice(const Slice &slice) {
//        sharding_impl_->Sharding(slice);
//    }
//
//    void SMImpl::ShardingMarkdupInfo(const read_end_t *read1_dup, const read_end_t *read2_dup) {
//        markdup_impl_->StorePairEndRecord(read1_dup, read2_dup);
//    }


//    void SMImpl::OutputBAM() {
//        markdup_impl_->MarkDuplication(ReadID::gTotalReadID());
//        sharding_impl_->StartMergeSort(bam_file_);
////        sharding_impl_->ReadGamBlock();
////        sharding_impl_->MergeSort();
////        sharding_impl_->FinishMergeSort();
//    }

    std::thread SMImpl::SpawnSharding() {
        return std::thread(&SMImpl::Sharding,  this);
    }

//    std::thread SMImpl::SpawnSortMkdup() {
//        return std::thread(&SMImpl::OutputBAM, this);
//    }

    void SMImpl::ProcessMarkDuplicate() {
        markdup_impl_->MarkDuplication(ReadID::gTotalReadID());
        BAMSortMkdupImpl sort_mkdup_impl(sharding_impl_->partition_datas(), options_, bam_hdr_, bam_file_);
        sort_mkdup_impl.ProcessSortMkdup();
    }

    void SMImpl::Sharding() {
        sharding_impl_->StartSharding();
        auto sharding_thread = std::thread(&SMImpl::ProcessSharding,  this);
        sharding_thread.join();
        sharding_impl_->FinishSharding();
    }

    void SMImpl::ProcessSharding() {
        GLOG_INFO << "Process Sharing " ;
        std::unique_ptr<BWAReadBuffer> read_buffer;
        while ( input_.read(read_buffer)) {
            GLOG_INFO << "Process sharding one batch";
            for (int i = 0; i < read_buffer->size; i += 2) {
                int gam_len = 0;
                const char *gam_data = read_buffer->seqs[i].bam;
                for (int j = 0; j < read_buffer->seqs[i].bam_num; ++j) {
                    gam_len = reinterpret_cast<const int32_t *>(gam_data)[5] + 24;
                    Slice slice(gam_data, gam_len);

                    sharding_impl_->Sharding(slice);
//                    PutSortSlice(slice);
                    gam_data += gam_len;
                }
                gam_len = 0;
                gam_data = read_buffer->seqs[i+1].bam;
                for (int j = 0; j < read_buffer->seqs[i+1].bam_num; ++j) {
                    gam_len = reinterpret_cast<const int32_t *>(gam_data)[5] + 24;
                    Slice slice(gam_data, gam_len);
                    sharding_impl_->Sharding(slice);
                    gam_data += gam_len;
                }
                read_end_t *read1_dup = read_buffer->seqs[i].dup;
                read_end_t *read2_dup = read_buffer->seqs[i+1].dup;
                read_buffer->seqs[i].dup = nullptr;
                read_buffer->seqs[i + 1].dup = nullptr;
                markdup_impl_->StorePairEndRecord(read1_dup, read2_dup);
            }
        }
        GLOG_INFO << "Finish Sharding";
        if (input_.eof()) {
            sharding_impl_->SendEof();
        }

    }
}