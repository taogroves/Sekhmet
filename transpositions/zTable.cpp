//
// Created by Tao G on 2/12/2023.
//

#include <algorithm>
#include "zTable.h"

void zTable::store(const zKey &key, TranspTableEntry data) {
    auto result = table.insert(std::make_pair(key.getValue(), data));

    // if the key already exists, replace the value
    if (!result.second) {
        result.first->second = data;
    }
}

const TranspTableEntry *zTable::lookup(const zKey &key) const {
    auto result = table.find(key.getValue());
    if (result != table.end()) {
        return &result->second;
    } else {
        return nullptr;
    }
}

int zTable::size() {
    return table.size();
}

void zTable::clear() {
    table.clear();
}


