//
// Created by Tao G on 2/12/2023.
//

#include "zTable.h"

zTable::zTable() : table(TABLE_SIZE) {
}

void zTable::store(const zKey &key, TranspTableEntry data) {
    U64 keyValue = key.getValue();
    Slot &slot = table[keyValue & TABLE_MASK];
    U64 oldData = slot.data.load(std::memory_order_relaxed);
    U64 oldKey = slot.key.load(std::memory_order_relaxed);
    U64 oldKeyValue = oldKey ^ oldData;

    if (oldData == 0) {
        entryCount.fetch_add(1, std::memory_order_relaxed);
    } else if (oldKeyValue != keyValue && unpack(oldData).getDepth() > data.getDepth()) {
        return;
    }

    U64 packed = pack(data);
    slot.data.store(packed, std::memory_order_relaxed);
    slot.key.store(keyValue ^ packed, std::memory_order_relaxed);
}

const TranspTableEntry *zTable::lookup(const zKey &key) const {
    static thread_local TranspTableEntry entry;
    U64 keyValue = key.getValue();
    const Slot &slot = table[keyValue & TABLE_MASK];
    U64 packed = slot.data.load(std::memory_order_relaxed);
    U64 storedKey = slot.key.load(std::memory_order_relaxed) ^ packed;
    if (packed == 0 || storedKey != keyValue) {
        return nullptr;
    }

    entry = unpack(packed);
    return &entry;
}

int zTable::size() {
    return entryCount.load(std::memory_order_relaxed);
}

void zTable::clear() {
    for (Slot &slot : table) {
        slot.data.store(0, std::memory_order_relaxed);
        slot.key.store(0, std::memory_order_relaxed);
    }
    entryCount.store(0, std::memory_order_relaxed);
}

U64 zTable::pack(const TranspTableEntry &entry) {
    constexpr int SCORE_BIAS = 1 << 20;
    U64 move = entry.getBestMove().getPacked() & ((ONE << 28) - 1);
    U64 score = U64(entry.getScore() + SCORE_BIAS) & ((ONE << 21) - 1);
    U64 depth = U64(entry.getDepth()) & 0xFFULL;
    U64 flag = U64(entry.getFlag()) & 0x3ULL;

    return move | (score << 28) | (depth << 49) | (flag << 57) | (ONE << 63);
}

TranspTableEntry zTable::unpack(U64 packed) {
    constexpr int SCORE_BIAS = 1 << 20;
    unsigned int moveBits = packed & ((ONE << 28) - 1);
    int score = int((packed >> 28) & ((ONE << 21) - 1)) - SCORE_BIAS;
    int depth = int((packed >> 49) & 0xFFULL);
    auto flag = TranspTableEntry::Flag((packed >> 57) & 0x3ULL);

    return TranspTableEntry(score, depth, flag, Move::fromPacked(moveBits));
}
