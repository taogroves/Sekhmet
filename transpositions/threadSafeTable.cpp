//
// Created by Tao Groves on 2/24/23.
//

#include "threadSafeTable.h"
#include <iostream>

void threadSafeTable::store(zKey key, TranspTableEntry data) {
    // create a new safeEntry
    U64 checksum = key.getValue() ^ data.getScore() ^ data.getDepth();
    safeEntry newEntry = {data, checksum};
    //safeEntry newEntry = {TranspTableEntry(0, 0, static_cast<TranspTableEntry::Flag>(0), Move()), checksum};

        writeLock.lock();
        auto result = table.insert({key.getValue(), newEntry});//std::make_pair(key.getValue(), newEntry));

    // if the key already exists, make sure the checksum matches
    if (!result.second) {
        if (result.first->second.checksum == checksum) {
            // if the checksum matches, replace the value
            result.first->second.data = data;
        }
        // otherwise, discard the new value
        //std::cout << "Discarded a value" << std::endl;
    }

    writeLock.unlock();
}

TranspTableEntry threadSafeTable::lookup(zKey key) {
    writeLock.lock();
    auto result = table.find(key.getValue());
    if (result != table.end()) {
        // if the checksum matches, return the value
        if (result->second.checksum == (key.getValue() ^ result->second.data.getScore() ^ result->second.data.getDepth())) {
            writeLock.unlock();
            return result->second.data;
        }
        std::cout << "Corrupted data!" << std::endl;
    }
    writeLock.unlock();
    return {0, -1, static_cast<TranspTableEntry::Flag>(0), Move()};
}

int threadSafeTable::size() {
    return table.size();
}

void threadSafeTable::clear() {
    table.clear();
}