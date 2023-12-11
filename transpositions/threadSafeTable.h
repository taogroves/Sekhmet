//
// Created by Tao Groves on 2/24/23.
//

#ifndef SEKHMET_THREADSAFETABLE_H
#define SEKHMET_THREADSAFETABLE_H

#include "zTable.h"
#include <mutex>

struct safeEntry {
    TranspTableEntry data;
    U64 checksum;
};

class threadSafeTable {
public:
    threadSafeTable() = default;

    TranspTableEntry lookup(zKey key);
    void store(zKey key, TranspTableEntry data);

    int size();
    void clear();

private:
    std::unordered_map<U64, safeEntry> table;
    std::mutex writeLock;
};


#endif //SEKHMET_THREADSAFETABLE_H
