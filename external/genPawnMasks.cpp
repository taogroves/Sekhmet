//
// Created by Tao Groves on 2/21/23.
//
#include "../core/defs.h"
#include <iostream>

void printLongAsSquare(unsigned long bitboard) {
    for (int i = 0; i < 64; i++) {
        if (bitboard & (1UL << i)) {
            std::cout << "1";
        } else {
            std::cout << "0";
        }
        if (i % 8 == 7) {
            std::cout << std::endl;
        }
    }
    std::cout << std::endl;
}

void passedPawnMasksW() {
    for (int i = 0; i < 64; i++) {
        int rank = i / 8;
        int file = i % 8;
        U64 mask = 0;
        int j = 0;
        while (j < rank) {
            U64 temp = 0;
            if (file > 0) {
                temp |= FILES[file - 1];
            }
            temp |= FILES[file];
            if (file < 7) {
                temp |= FILES[file + 1];
            }
            mask |= (temp) & RANKS[j];
            j++;
        }
        //printLongAsSquare(mask);
        std::cout << "0x" << std::hex << mask << ", ";
        if (file == 7) {
            std::cout << std::endl;
        }
    }
}

void passedPawnMasksB() {
    for (int i = 0; i < 64; i++) {
        int rank = i / 8;
        int file = i % 8;
        U64 mask = 0;
        int j = 7;
        while (j > rank) {
            U64 temp = 0;
            if (file > 0) {
                temp |= FILES[file - 1];
            }
            temp |= FILES[file];
            if (file < 7) {
                temp |= FILES[file + 1];
            }
            mask |= (temp) & RANKS[j];
            j--;
        }
        //printLongAsSquare(mask);
        std::cout << "0x" << std::hex << mask << ", ";
        if (file == 7) {
            std::cout << std::endl;
        }
    }
}

void isolatedPawnMasks() {
    for (int i = 0; i < 64; i++) {
        int rank = i / 8;
        int file = i % 8;
        U64 mask = 0;
        if (file > 0) {
            mask |= FILES[file - 1];
        }
        if (file < 7) {
            mask |= FILES[file + 1];
        }
        //printLongAsSquare(mask);
        std::cout << "0x" << std::hex << mask << ", ";
        if (file == 7) {
            std::cout << std::endl;
        }
    }
}

int main() {
    passedPawnMasksB();
    return 0;
}