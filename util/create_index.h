//
// Created by ruoshui on 5/2/18.
//

#ifndef GAMTOOLS_SM_CREATE_INDEX_H
#define GAMTOOLS_SM_CREATE_INDEX_H

#include <vector>

#include <htslib/sam.h>
#include <string>


namespace  gamtools {
    enum  class IndexType {
        SortIndex,
        MarkDupIndex
    };

    class PartitionData {
    public:
        explicit PartitionData(int sharding_id, std::string &filename):sharding_id_(sharding_id), filename_(filename){

        }
        const std::string& filename() const {
            return filename_;
        }
        inline const int sharding_id() const {
            return sharding_id_;
        }
    protected:
        int sharding_id_;
        std::string filename_;
    };
    struct SmOptions;
    class CreateIndex {
    public:
        explicit CreateIndex(const bam_hdr_t *head, const SmOptions &options, IndexType type);
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
