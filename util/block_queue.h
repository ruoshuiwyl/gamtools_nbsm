//
// Created by ruoshui on 6/5/18.
//

#ifndef GAMTOOLS_SM_BLOCK_QUEUE_H
#define GAMTOOLS_SM_BLOCK_QUEUE_H

#include <cstddef>

namespace gamtools {
    template <typename T>
    class BlockQueue {
    public:
        virtual void SendEof() = 0;
        virtual bool eof() = 0;
        virtual size_t size() = 0;
        virtual bool read(T &elem) = 0;
        virtual void write(T&& elem) = 0;
    };
}

#endif //GAMTOOLS_SM_BLOCK_QUEUE_H
