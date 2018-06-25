//
// Created by ruoshui on 5/22/18.
//

#include <util/glogger.h>
#include "util/slice.h"
#include "util/create_index.h"



#include <nbsm/options.h>
#include <snappy.h>
#include <mkdup/gam_mark_duplicate_impl.h>
#include <htslib/bgzf.h>
#include <lib/htslib-1.3.1/htslib/sam.h>
#include "bam_sort_mkdup_impl.h"
#include "sharding/bam_partition_data.h"
#include "sharding/bam_block.h"

#ifdef DEBUG
#include "util/debug_util.h"
#endif



namespace gamtools {

    struct BAMSlice {
        int block_idx;
        int slice_idx;
        uint64_t pos;
        int64_t read_id;
//        std::vector< Slice>::const_iterator slice_itr_;
    };

    class BAMSliceComparator {
    public:
        bool operator() (const BAMSlice &a, const BAMSlice &b) {
            return a.pos > b.pos || (a.pos == b.pos && a.read_id > b.read_id);
        }
    };

    const static int kBAMBlockSize = 65536;


    BAMSortMkdupImpl::BAMSortMkdupImpl( std::vector<std::unique_ptr<ShardingPartitionData>> &partition_datas,
                                        const SMOptions &sm_opt,
                                        const bam_hdr_t *bam_hdr,
                                        const std::string &bam_filename)
            : partition_datas_(partition_datas),
              bam_hdr_(bam_hdr),
              bam_filename_(bam_filename),
              sm_options_(sm_opt),
              output_bam_queue_(16),
              quality_control_queue_(16){
        block_size_ = sm_opt.sort_block_size;
        qc_stat_impl_ = std::unique_ptr<QCStatImpl>
                (new QCStatImpl(quality_control_queue_, sm_options_.ref_file, sm_options_.bed_file, sm_options_.report_file));
    }




    void BAMSortMkdupImpl::ProcessSortMkdup() {
        std::vector<std::thread> read_gam_threads;
        gam_block_idx_.store(0);
        for (int i = 0; i < sm_options_.read_gam_thread_num; ++i) {
            read_gam_threads.push_back(std::thread(&BAMSortMkdupImpl::ReadGAMPartitionData, this));
        }
//        std::vector<std::thread> sort_threads;
//        for (int i = 0; i < sm_options_.merge_sort_thread_num; ++i) {
//            sort_threads.push_back(std::thread(&BAMSortMkdupImpl::PartitionDecompressMerge, this));
//        }
        std::thread output_bam_thread = std::thread(&BAMSortMkdupImpl::OutputBAM, this);
        std::vector<std::thread> qc_threads;
        for (int i = 0; i < sm_options_.stat_thread_num; ++i) {
            qc_threads.push_back(qc_stat_impl_->StaticsSpawn());
        }
//        std::thread qc_report_thread = std::thread(&BAMSortMkdupImpl::QCStatics, this);
        for (auto &read_gam_thread :read_gam_threads ) {
            read_gam_thread.join();
        }
        GLOG_INFO << "Finish Read and Merge gam block";
//        for (auto &th : sort_threads) {
//            th.join();
//        }
//        output_bam_channel_.SendEof();
        output_bam_queue_.SendEof();
        quality_control_queue_.SendEof();
//        GLOG_INFO << "Finish Merge gam block";
        output_bam_thread.join();
        for (auto &qc_thread : qc_threads) {
            qc_thread.join();
        }
        GLOG_INFO << "Finish fastAln and QC report";
    }

    void BAMSortMkdupImpl::ReadGAMPartitionData() {
//        for (auto &part : partition_datas_) {
//            GLOG_INFO << "Start Read one sharding idx:" << part->partition_data().sharding_id();
        const int sharding_size = partition_datas_.size();
        while (true) {
            int block_idx = gam_block_idx_.load(std::memory_order::memory_order_seq_cst);
            gam_block_idx_.fetch_add(1, std::memory_order::memory_order_seq_cst);
            if (block_idx >= sharding_size) {
                break;
            }
            auto partition_data = ReadGAMBlock(partition_datas_[block_idx]);
            MergePartition(partition_data);
        }
//        }

    }




    std::unique_ptr<GAMPartitionData> BAMSortMkdupImpl::ReadGAMBlock(
            std::unique_ptr<gamtools::ShardingPartitionData> &part_data) {
        auto &part = part_data->partition_data();
        std::unique_ptr<GAMPartitionData> partition_data_ptr(new GAMPartitionData(part.sharding_id()));
        auto &filename = part.filename() ;
        int block_cnt = part_data->block_cnt();
        GLOG_TRACE << "Read gam block idx:" << part.sharding_id() << "block num:" << block_cnt;
        if (block_cnt > 0) {
            std::ifstream ifs(filename);
            if (ifs.is_open()) {
                GLOG_INFO << "Merge Stage Open file OK" << filename;
            } else {
                GLOG_ERROR << "Merge Stage Open file Error " << filename;
            }
            char *data = new char[block_size_];
            char *uncompress = new char[block_size_];
            for (int block_idx = 0; block_idx < block_cnt; ++block_idx) {
                size_t size = 0;
                ifs.read((char *) &size, sizeof(size_t));
                assert(block_size_ > size);
                ifs.read(data, size);
//                std::unique_ptr<Block> gam_block = std::unique_ptr<Block>(new Block(block_size_));
//                gam_block->Insert(data, size);
                size_t ulength = 0;
                if (!snappy::GetUncompressedLength(data, size, &ulength)) {
                    GLOG_ERROR << "Snappy compressed length" << part.sharding_id() << ":" << block_idx << "Error";
                }
                assert(block_size_ >= ulength);
                std::unique_ptr<Block> dblock(new Block(block_size_));
                if (!snappy::RawUncompress(data, size, uncompress)) {
                    GLOG_ERROR << "Snappy  uncompressed " << part.sharding_id() << ":" << block_idx << " Error";
                }
                dblock->Insert(uncompress, ulength);
                // Build GAM slice ;
                auto &slices = dblock->slices();
                slices.clear();
                const char *alloc_ptr_ = dblock->data();
                size_t use_len = 0;
                while (use_len < dblock->size()) {
                    int bam_size = reinterpret_cast<const int *>(alloc_ptr_)[5] + 24;
                    Slice slice(alloc_ptr_, bam_size);
#ifdef DEBUG
                    DebugGAMSlice(slice);
#endif
                    slices.push_back(slice);
                    alloc_ptr_ += bam_size;
                    use_len += bam_size;
                }
                partition_data_ptr->blocks.push_back(std::move(dblock));
            }
            delete[] data;
            delete[] uncompress;
            ifs.close();
        }
#ifdef DEBUG
    GLOG_ERROR << "read gam " << partition_data_ptr->order() ;
#endif
        return partition_data_ptr;
//        gam_part_queue_.write(std::move(partition_data_ptr));
    }

//    void BAMSortMkdupImpl::PartitionDecompressMerge() {
//        std::unique_ptr<GAMPartitionData> partition_data_ptr;
//        while (gam_part_queue_.read(partition_data_ptr)) {
//            GLOG_INFO << "Start merge one sharding idx: " << partition_data_ptr->sharding_idx ;
////            Decompress(partition_data_ptr);
//            MergePartition(partition_data_ptr);
//            GLOG_INFO << "Finish merge sharding " << partition_data_ptr->sharding_idx;
//        }
//
//    }

    /*
    void BAMSortMkdupImpl::Decompress(std::unique_ptr<gamtools::GAMPartitionData> &gam_part) {
        std::vector<std::unique_ptr<Block>> decompress_blocks;
        char *uncompress = new char[block_size_];
        for (auto& block : gam_part->blocks) {
            size_t ulength = 0;
            if (!snappy::GetUncompressedLength(block->data(), block->size(), &ulength)) {
                GLOG_ERROR << "Snappy get un compressed length Error" << gam_part->sharding_idx ;
            }
            assert(block_size_ >= ulength);
            std::unique_ptr<Block> dblock(new Block(block_size_));
            if (!snappy::RawUncompress(block->data(), block->size(), uncompress)){
                GLOG_ERROR << "Snappy  uncompressed Error" << gam_part->sharding_idx;
            }
            dblock->Insert(uncompress, ulength);

            // Build GAM slice ;
            auto &slices = dblock->slices();
            slices.clear();
            const char *alloc_ptr_ = dblock->data();
            size_t use_len= 0;
            while (use_len < dblock->size()) {
                int  bam_size = reinterpret_cast<const int *>(alloc_ptr_)[5] + 24;
                Slice slice(alloc_ptr_, bam_size);
#ifdef DEBUG
                DebugGAMSlice(slice);
#endif
                slices.push_back(slice);
                alloc_ptr_ += bam_size;
                use_len += bam_size;
            }
            decompress_blocks.push_back(std::move(dblock));

        }
        int block_cnt = decompress_blocks.size();
        for (int i = 0; i < block_cnt; ++i) {
            gam_part->blocks[i].swap(decompress_blocks[i]);
        }
//        swap(gam_part->blocks, decompress_blocks);

        delete [] uncompress;
    }
     */

    void  BAMSortMkdupImpl::MergePartition(std::unique_ptr<GAMPartitionData> &gam_part){
//        auto bam_block_ptr = std::unique_ptr<BAMBlock>( new BAMBlock(kBAMBlockSize, gam_part->sharding_idx, 0 ,false));
        auto bam_sharding_ptr = std::unique_ptr<GAMPartitionData>(new GAMPartitionData(gam_part->sharding_idx));
        auto qc_sharding_ptr = std::unique_ptr<QCShardingData> ( new QCShardingData(gam_part->sharding_idx));
        auto &gam_blocks = gam_part->blocks;
        if (gam_blocks.empty()) {
//            bam_block_ptr->SendEof();
            output_bam_queue_.write(std::move(bam_sharding_ptr));
            quality_control_queue_.write(std::move(qc_sharding_ptr));
            return ;
        }
        if (gam_blocks.size() > 1) {
            std::priority_queue<BAMSlice, std::vector<BAMSlice>, BAMSliceComparator> bam_heap;
#ifdef DEBUG
            for (int i = 0; i < gam_blocks.size(); ++i) {
                for (int j = 0; j < gam_blocks[i]->slices().size(); ++ j){
                    const char *gam_data = gam_blocks[i]->slices()[j].data();
                    uint64_t sort_pos = reinterpret_cast<const uint64_t *>(gam_data)[0];
                    int64_t read_id = reinterpret_cast<const int64_t *>(gam_data)[1];
                    int tid = sort_pos>>32;
                    int pos = (sort_pos & 0xffffffff )>> 1;
                    if (gam_part->sharding_idx == 0) {
                        GLOG_TRACE << "before_merge_sharding_id:" << gam_part->sharding_idx << "_" << i << "\t" << tid
                                   << "_" << pos << "\t" << sort_pos << " " << read_id;
                    }
                }
            }
#endif
            for (int i = 0; i < gam_blocks.size(); ++i) {
                BAMSlice bam_slice;
                bam_slice.block_idx = i;
                bam_slice.slice_idx = 0;
                if (bam_slice.slice_idx != gam_blocks[i]->slices().size()) {
                    const char *gam_data = gam_blocks[bam_slice.block_idx]->slices()[bam_slice.slice_idx].data();
                    uint64_t sort_pos = reinterpret_cast<const uint64_t *>(gam_data)[0];
                    int64_t read_id = reinterpret_cast<const int64_t *>(gam_data)[1];
                    bam_slice.read_id = read_id;
                    bam_slice.pos = sort_pos;
                    bam_heap.push(bam_slice);
                }
            }

            while (!bam_heap.empty()) {
                auto data = bam_heap.top();
                const char *gam_data = gam_blocks[data.block_idx]->slices()[data.slice_idx].data();
                uint64_t sort_pos = reinterpret_cast<const uint64_t *>(gam_data)[0];
                int64_t read_id = reinterpret_cast<const int64_t *>(gam_data)[1];
                assert(sort_pos == data.pos);
                assert(read_id == data.read_id);
                const bool markdup_flag = GAMMarkDuplicateImpl::IsMarkDuplicate(read_id); // Markdup read
                const char *bam = gam_data + 20;
                if (markdup_flag) { // flag add mark dup flag 0x400 1024
                    int orginal_flag = reinterpret_cast< const int *>(bam)[4];
                    if (!(orginal_flag & (0x800 << 16))) { // skip flag 2048
                        int bam_flag = reinterpret_cast< const int *>(bam)[4] | (0x400 << 16);
                        reinterpret_cast< int *>(const_cast<char *> (bam))[4] = bam_flag;
                    }
                }
                Slice slice(bam, reinterpret_cast<const int *>(bam)[0] + 4);
                int tid = sort_pos>>32;
                int pos = (sort_pos & 0xffffffff )>> 1;
//                if (gam_part->sharding_idx == 0) {
//                    GLOG_TRACE << "after_merge_sharding_id:" << gam_part->sharding_idx << "_" << data.block_idx << "\t"
//                               << tid << "_" << pos << "\t" << sort_pos << " " << read_id;
//                }
//                InsertBAMSlice(slice, bam_block_ptr);
                bam_sharding_ptr->InsertBAMSlice(slice);
                StatisticsSlice stat_data;
                stat_data.tid = reinterpret_cast<const int*>(bam)[1];
                stat_data.pos = reinterpret_cast<const int*>(bam)[2];
                stat_data.qlen = reinterpret_cast<const int*>(bam)[5];
                stat_data.rlen = reinterpret_cast<const uint32_t*>(gam_data)[4] >> 16;
                stat_data.mapq = (reinterpret_cast<const uint32_t*>(bam)[3] >> 8 ) & 0xff;;
                stat_data.is_dup = markdup_flag;
//                assert(stat_data.rlen > 0);
                qc_sharding_ptr->InsertStatData(stat_data);
                bam_heap.pop();
                ++data.slice_idx;
                if (data.slice_idx != gam_blocks[data.block_idx]->slices().size()) {
                    const char *gam_data = gam_blocks[data.block_idx]->slices()[data.slice_idx].data();
                    uint64_t sort_pos = reinterpret_cast<const uint64_t *>(gam_data)[0];
                    int64_t read_id = reinterpret_cast<const int64_t *>(gam_data)[1];
                    data.read_id = read_id;
                    data.pos = sort_pos;
//                    if (gam_part->sharding_idx == 0) {
//                        GLOG_TRACE << "add_merge_sharding_id:" << gam_part->sharding_idx << "_" << data.block_idx
//                                   << "\t" <<
//                                   (data.pos >> 32) << "_" << ((pos & 0xffffffff) >> 1) << "\t" << data.pos << " "
//                                   << read_id;
//                    }
                    bam_heap.push(data);
                } else {
                    gam_blocks[data.block_idx].reset(nullptr); // release block memory
                }
            }

        } else {
            auto &slice = gam_blocks[0]->slices();
            for (auto it = slice.begin(); it != slice.end(); ++it){
                int64_t read_id = reinterpret_cast<const int64_t *>(it->data())[1];
                const bool markdup_flag = GAMMarkDuplicateImpl::IsMarkDuplicate(read_id); // Markdup read
                const char *bam = it->data() + 20;
                if (markdup_flag) { // flag add mark dup flag 0x400
                    int bam_flag = reinterpret_cast< const int *>(bam)[4] | (0x400 << 16);
                    reinterpret_cast< int *>(const_cast<char *> (bam))[4] = bam_flag;
                }
                Slice slice(bam, reinterpret_cast<const int *>(bam)[0] + 4);
//                InsertBAMSlice(slice, bam_block_ptr);
                uint64_t sort_pos = reinterpret_cast<const uint64_t *>(it->data())[0];

                int tid = sort_pos>>32;
                int pos = (sort_pos & 0xffffffff )>> 1;
                StatisticsSlice stat_data;
                stat_data.tid = reinterpret_cast<const int*> (bam)[1];
                stat_data.pos = reinterpret_cast<const int*> (bam)[2];
                stat_data.qlen = reinterpret_cast<const int*>(bam)[5];
                stat_data.rlen = reinterpret_cast<const uint32_t*>(it->data())[4] >> 16;
                stat_data.mapq = (reinterpret_cast<const uint32_t*>(bam)[3] >> 8 ) & 0xff;
                stat_data.is_dup = markdup_flag;
//                assert(stat_data.rlen > 0);
                qc_sharding_ptr->InsertStatData(stat_data);
                bam_sharding_ptr->InsertBAMSlice(slice);
            }
        }
//        bam_block_ptr->SendEof();
        bam_sharding_ptr->End();
        GLOG_INFO << "Finish read and merge sharding :" << bam_sharding_ptr->sharding_idx ;
        output_bam_queue_.write(std::move(bam_sharding_ptr));
        quality_control_queue_.write(std::move(qc_sharding_ptr));
    }

    void GAMPartitionData::InsertBAMSlice(gamtools::Slice &slice) {
        if (bam_block_ptr == nullptr) {
            bam_block_ptr = std::unique_ptr<BAMBlock>(
                    new BAMBlock(kBAMBlockSize, sharding_idx, 0, false));
        }

        if (!bam_block_ptr->Insert(slice)) {
            if (bam_block_ptr->full()) {
                blocks.push_back(std::move(bam_block_ptr));
//                int sharding_idx = bam_block_ptr->sharding_idx();
                int bam_block_idx = blocks.size();
//                output_bam_channel_.write(std::move(bam_block_ptr));
                bam_block_ptr = std::unique_ptr<BAMBlock>(
                        new BAMBlock(kBAMBlockSize, sharding_idx, bam_block_idx, false));
                bam_block_ptr->Insert(slice);
            } else {
                GLOG_ERROR << "BAM Block insert failed but not full";
            }
        }
    }
    void GAMPartitionData::End() {
        blocks.push_back(std::move(bam_block_ptr));
    }
//    void
//    BAMSortMkdupImpl::InsertBAMSlice(gamtools::Slice &slice, std::unique_ptr<BAMBlock> &bam_block_ptr) {
//
//#ifdef DEBUG
////        DebugBAMSlice(slice);
//#endif
////        int tid = reinterpret_cast<const int *>(slice.data())[1];
////        int pos = reinterpret_cast<const int *>(slice.data())[2];
////        std::cerr << "sharding_idx" << bam_block_ptr->sharding_idx() << ":" << tid << "_" << pos << std::endl;
//
//        if (!bam_block_ptr->Insert(slice)) {
//            if (bam_block_ptr->full()) {
//                int sharding_idx = bam_block_ptr->sharding_idx();
//                int bam_block_idx = bam_block_ptr->block_idx() + 1;
//                output_bam_channel_.write(std::move(bam_block_ptr));
//                bam_block_ptr = std::unique_ptr<BAMBlock>(
//                        new BAMBlock(kBAMBlockSize, sharding_idx, bam_block_idx, false));
//                bam_block_ptr->Insert(slice);
//            } else {
//                GLOG_ERROR << "BAM Block insert failed but not full";
//            }
//        }
//    }


    void BAMSortMkdupImpl::OutputBAM() {
        GLOG_INFO << "Start write bam ";
        bam_file_ = hts_open_format(bam_filename_.c_str(), "wb", nullptr);
        if (bam_file_ == nullptr) {
            GLOG_ERROR << "Write bam open bam file failed ";
        }
//        if (!hts_set_threads(bam_file_, sm_options_.bam_output_thread_num)) {
//            GLOG_ERROR << "Write bam set compress thread failed ";
//        }
        if (!hts_set_threads(bam_file_, sm_options_.bam_output_thread_num)) {
            GLOG_ERROR << "Write bam set compress thread failed ";
        }
        if (!bam_hdr_write(bam_file_->fp.bgzf, bam_hdr_)) {
            GLOG_ERROR << "Write bam head failed";
        }
//        std::unique_ptr<BAMBlock> bam_block;
        int current_sharding_idx = 0;
        std::priority_queue<int, std::vector<int>, std::greater<int>> finish_sharding_idxs;
        std::unique_ptr<GAMPartitionData> bam_block;
        while (output_bam_queue_.read(bam_block)) {

            auto sharding_idx = bam_block->sharding_idx;
//            std::cerr << "sharding_idx " << sharding_idx <<  "size:" << bam_block->blocks.size() << std::endl;
                      //            bool eof = bam_block->eof();
//            chunks_[sharding_idx].push_back(std::move(bam_block));
//            bam_chunks_.push_back();
//            bam_chunks_[sharding_idx] = std::move(bam_block);
//            if (eof) {
            assert(current_sharding_idx == sharding_idx);
//            finish_sharding_idxs.push(sharding_idx);
//            while (current_sharding_idx == finish_sharding_idxs.top()) {
            GLOG_TRACE << "Output BAM sharding idx " << current_sharding_idx ;
//                OutputShardingBAM(current_sharding_idx);
            OutputShardingBAM(bam_block);
            ++current_sharding_idx;
//            finish_sharding_idxs.pop();
//                if (finish_sharding_idxs.empty()) {
//                    break;
//                }
//            }
//            }
        }

//        if (!finish_sharding_idxs.empty()) {
//            while (current_sharding_idx == finish_sharding_idxs.top()) {
//                OutputShardingBAM(current_sharding_idx);
//                ++current_sharding_idx;
//                finish_sharding_idxs.pop();
//            }
//        }
        if (!finish_sharding_idxs.empty()) {
            GLOG_ERROR << "finish sharding should be empty";
        }
        if (output_bam_queue_.eof()) {
            hts_close(bam_file_);
        } else {
            GLOG_ERROR << "close bam file failed";
        }

    }



//    void BAMSortMkdupImpl::QCStatics() {
//
//
//    }
//
//    void BAMSortMkdupImpl::QCCompute() {
//        std::unique_ptr<QualityControl> qc_report;
//        std::unique_ptr<QCShardingData> qc_data;
//        if (sm_options_.bed_file.empty()) {
//            qc_report = std::unique_ptr<QualityControl>(new QualityControl(sm_options_.ref_file, sm_options_.report_file));
//        } else {
//            qc_report = std::unique_ptr<QualityControl>(new QualityControl(sm_options_.ref_file, sm_options_.bed_file, sm_options_.report_file));
//        }
//        qc_report->Init();
//        while (qc_buffer_queue_.read(qc_data)) {
//            auto &stat_datas = qc_data->stat_datas;
//            for (auto &stat_data : stat_datas) {
//                qc_report->Statistics(stat_data);
//            }
//        }
//        qc_report->Report();
//
//    }

    /*
    void BAMSortMkdupImpl::OutputShardingBAM(int current_sharding_idx) {
        if (!chunks_[current_sharding_idx].empty()) {
            for (auto &block: chunks_[current_sharding_idx]) {
                auto &slices = block->slices();
                for (auto it = slices.begin(); it != slices.end(); ++it) {
                    int block_len = it->size() - 4;
#ifdef DEBUG
                    DebugBAMSlice(*it);
#endif
                    int ok = (bgzf_flush_try(bam_file_->fp.bgzf, it->size()) >= 0);
                    if (ok) {
                        ok = bgzf_write(bam_file_->fp.bgzf, (char *)&(block_len)  ,4); // write bam length
                    }
                    if (ok) {
                        ok = bgzf_write(bam_file_->fp.bgzf, it->data() + 4, 32); // write bam core data 32B
                    }
                    if (ok) {
            // write bam data variant length
                        ok = bgzf_write(bam_file_->fp.bgzf, it->data() + 36, block_len - 32);
                    }
                    if (!ok) {
                        GLOG_ERROR << "Write BAM error ";
                    }
                }
                block.reset(nullptr);
            }
        }
    }
     */

    void BAMSortMkdupImpl::OutputShardingBAM(std::unique_ptr<gamtools::GAMPartitionData> &bam_chunk) {
        for ( auto &block : bam_chunk->blocks) {
            auto &slices = block->slices();
            for (auto it = slices.begin(); it != slices.end(); ++it) {
                int block_len = it->size() - 4;
#ifdef DEBUG
                DebugBAMSlice(*it);
#endif
                int ok = (bgzf_flush_try(bam_file_->fp.bgzf, it->size()) >= 0);
                if (ok) {
                    ok = bgzf_write(bam_file_->fp.bgzf, (char *)&(block_len)  ,4); // write bam length
                }
                if (ok) {
                    ok = bgzf_write(bam_file_->fp.bgzf, it->data() + 4, 32); // write bam core data 32B
                }
                if (ok) {
                    // write bam data variant length
                    ok = bgzf_write(bam_file_->fp.bgzf, it->data() + 36, block_len - 32);
                }
                if (!ok) {
                    GLOG_ERROR << "Write BAM error ";
                }
            }
            block.reset(nullptr);
        }
    }

}