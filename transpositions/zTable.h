//
// Created by Tao G on 2/12/2023.
//

#ifndef SEKHMET_ZTABLE_H
#define SEKHMET_ZTABLE_H

#include <vector>
#include "zKey.h"
#include "TranspTableEntry.h"

class zTable {

public:

    enum Flag {
        EXACT, // exact score
        LOWERBOUND, // score is at least as good as the score
        UPPERBOUND, // score is at most as good as the score
    };

    zTable();

    const TranspTableEntry *lookup(const zKey &key) const;
    void store(const zKey &key, TranspTableEntry data);

    int size();
    void clear();

private:
    struct Slot {
        U64 key = 0;
        bool occupied = false;
        TranspTableEntry entry;
    };

    static constexpr size_t TABLE_BITS = 20;
    static constexpr size_t TABLE_SIZE = ONE << TABLE_BITS;
    static constexpr size_t TABLE_MASK = TABLE_SIZE - 1;

    std::vector<Slot> table;
    int entryCount = 0;
};


#endif //SEKHMET_ZTABLE_H
