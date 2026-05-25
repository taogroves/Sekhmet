//
// Created by Tao G on 2/12/2023.
//

#include "zTable.h"

zTable::zTable() : table(TABLE_SIZE) {
}

void zTable::store(const zKey &key, TranspTableEntry data) {
    U64 keyValue = key.getValue();
    Slot &slot = table[keyValue & TABLE_MASK];

    if (!slot.occupied) {
        entryCount++;
    } else if (slot.key != keyValue && slot.entry.getDepth() > data.getDepth()) {
        return;
    }

    slot.key = keyValue;
    slot.occupied = true;
    slot.entry = data;
}

const TranspTableEntry *zTable::lookup(const zKey &key) const {
    U64 keyValue = key.getValue();
    const Slot &slot = table[keyValue & TABLE_MASK];
    return slot.occupied && slot.key == keyValue ? &slot.entry : nullptr;
}

int zTable::size() {
    return entryCount;
}

void zTable::clear() {
    for (Slot &slot : table) {
        slot.occupied = false;
    }
    entryCount = 0;
}

