//
// Created by ruoshui on 5/2/18.
//

#ifndef GAMTOOLS_SM_CREATE_INDEX_H
#define GAMTOOLS_SM_CREATE_INDEX_H

#include <vector>

#include <hts/htslib/sam.h>
#include <string>


namespace  gamtools {
    enum  class IndexType {
        SortIndex,
        MarkDupIndex
    };

    class PartitionData {
    public:
        explicit PartitionData(int sharding_id, int chromosome_idx,  std::string &filename)
                :sharding_id_(sharding_id), chromosome_idx_(chromosome_idx), filename_(filename){
        }
        const std::string& filename() const {
            return filename_;
        }
        inline const int sharding_id() const {
            return sharding_id_;
        }
        inline const int chromosome_idx() const {
            return chromosome_idx_;
        }
    protected:
        int sharding_id_;
        int chromosome_idx_;
        std::string filename_;
    };
    struct SMOptions;
    class CreateIndex {
    public:
        explicit CreateIndex(const bam_hdr_t *head, const SMOptions &options, IndexType type);
        const std::vector<PartitionData>& partition_datas() const {
            return partition_datas_;
        }
        const std::vector<std::vector<int>>& sharding_index() const {
            return sharding_index_;
        }
    private:
        std::string CreateIndexDirectory(int dir_id, IndexType type, const std::string &store_dir);
        std::vector<std::vector<int>> sharding_index_;
        std::vector<PartitionData> partition_datas_;
    };
}


#endif //GAMTOOLS_SM_CREATE_INDEX_H
