#ifndef CACHE_LINE_H
#define CACHE_LINE_H

#include <cstddef>

struct CacheLine {
    size_t tag;
    bool valid = false;
    size_t last_used;
    size_t inserted_at;
    size_t freq;        // access count, used by LFU

    CacheLine();
};

#endif
