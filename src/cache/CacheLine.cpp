#include "cache/CacheLine.h"

CacheLine::CacheLine()
    : tag(0), valid(false), last_used(0), inserted_at(0), freq(0) {}