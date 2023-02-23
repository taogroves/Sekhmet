//
// Created by Tao G on 2/12/2023.
//

#ifndef SEKHMET_ZTABLE_H
#define SEKHMET_ZTABLE_H

#include <unordered_map>
#include "zKey.h"
#include "TranspTableEntry.h"

class zTable {

public:

    enum Flag {
        EXACT, // exact score
        LOWERBOUND, // score is at least as good as the score
        UPPERBOUND, // score is at most as good as the score
    };

    zTable() = default;

    const TranspTableEntry *lookup(const zKey &key) const;
    void store(const zKey &key, TranspTableEntry data);

    int size();
    void clear();

private:
    std::unordered_map<U64, TranspTableEntry> table;
};


#endif //SEKHMET_ZTABLE_H
