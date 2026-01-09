#include "cache/CacheSet.h"

CacheSet::CacheSet(size_t associativity)
    : lines(associativity) {
            for (auto& line : lines) {
        line.valid = false;
        line.tag = 0;
        line.last_used = 0;
        line.inserted_at = 0;
        line.freq = 0;
    }
    }

    

bool CacheSet::access(size_t tag,
                      const std::string& policy,
                      size_t timestamp,
                      bool& hit) {

    //Checking for hit
    for (auto& line : lines) {
        if (line.valid && line.tag == tag) {
            hit = true;
            line.last_used = timestamp;
            line.freq++;
            return true;
        }
    }

    //Miss
    hit = false;
    CacheLine* victim = nullptr;

    // Prefer invalid line
    for (auto& line : lines) {
        if (!line.valid) {
            victim = &line;
            break;
        }
    }

    //If no invalid line
    if (!victim) {
        victim = &lines[0];
        for (auto& line : lines) {
            if (policy == "LRU" && line.last_used < victim->last_used)
                victim = &line;
            else if (policy == "FIFO" && line.inserted_at < victim->inserted_at)
                victim = &line;
            else if (policy == "LFU" && line.freq < victim->freq)
                victim = &line;
        }
    }

    // Replace victim
    victim->tag = tag;
    victim->valid = true;
    victim->inserted_at = timestamp;
    victim->last_used = timestamp;
    victim->freq = 1;

    return false;
}
