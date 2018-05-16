//
// Created by ruoshui on 5/10/18.
//

#ifndef GAMTOOLS_SM_GAM_FASTQ_READ_IMPL_H
#define GAMTOOLS_SM_GAM_FASTQ_READ_IMPL_H


#include <vector>
#include <memory>
#include <util/channel.h>

namespace gamtools {
    class GAMFastqFile;
    class GAMReadBuffer;
    class GAMFastqFileInfo;
    class GAMFastqReadImpl {
    public:
        explicit GAMFastqReadImpl(std::vector<gamtools::GAMFastqFileInfo> &fastq_file_lists,
                                          gamtools::Channel<std::unique_ptr<gamtools::GAMReadBuffer>> &output_fastq_channel_,
                                          const int batch_size);
        void ReadFastQ();
        std::thread spawn();
    private:
        std::vector<std::unique_ptr<GAMFastqFile>> fastq_file_lists_;
        Channel<std::unique_ptr<GAMReadBuffer>> &output_fastq_channel_;
        int fastq_batch_size_;
    };
}


#endif //GAMTOOLS_SM_GAM_FASTQ_READ_IMPL_H
