//
// Created by ruoshui on 5/2/18.
//

#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <htslib/sam.h>


#include "create_index.h"
#include "nbsm/options.h"
#include "glogger.h"

#include "boost/filesystem.hpp"

namespace gamtools {
    CreateIndex::CreateIndex(const bam_hdr_t *head, const SMOptions &options, IndexType type) {

        int sharding_idx = 0;
        int file_cnt = 0;
        int dir_cnt= 0;
        std::string dir_name = CreateIndexDirectory(dir_cnt, type, options.directory);
        std::string file_name;
        int region_size = type == IndexType::SortIndex? 1 << options.sort_region_size : 1 << options.markdup_region_size;
        std::string file_extension = type == IndexType::SortIndex? ".gam" : ".mk";
        sharding_index_.resize(head->n_targets);
        for (int i = 0; i < head->n_targets; ++i) {
            for (int j = 0; j < head->target_len[i]; j += region_size) {
                sharding_index_[i].push_back(sharding_idx);
                if (file_cnt >= options.max_file_size) {
                    dir_cnt++;
                    dir_name = CreateIndexDirectory(dir_cnt, type, options.directory);
                    file_cnt = 0;
                } else {
                    file_cnt++;
                }
                file_name = dir_name + "/sharding_" + std::to_string(sharding_idx) + file_extension;
                partition_datas_.push_back(PartitionData(sharding_idx, file_name));
                sharding_idx++;
            }
        };
        file_name = dir_name + "/sharding_" +  std::to_string(sharding_idx) + file_extension;
        partition_datas_.push_back(PartitionData(sharding_idx, file_name)); //add unmap read
    }

    std::string CreateIndex::CreateIndexDirectory(int dir_id, IndexType type, const std::string &store_dir) {
        std::string dir_name;
        if (type == IndexType::SortIndex) {
            dir_name = store_dir + "/Sort/" + std::to_string(dir_id) ;
        } else if (type == IndexType::MarkDupIndex) {
            dir_name = store_dir + "/MarkDup/" + std::to_string(dir_id) ;
        }
        boost::filesystem::path dir_path = dir_name;

        if (boost::filesystem::exists(dir_path)) {
            boost::filesystem::remove_all(dir_path);
        } else {
            boost::filesystem::create_directories(dir_path);
        }
        bool status = boost::filesystem::create_directory(dir_path); //read & write

        if (!status) {
            GLOG_ERROR << "Create directory name  " << dir_name << " failed" ;
            exit(1);
        } else {
            GLOG_INFO << "Create directory name" << dir_name << "\tOK ";
        }
        return dir_name;
    }

}