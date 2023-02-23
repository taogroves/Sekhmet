//
// Created by Tao G on 2/7/2023.
//

#ifndef SEKHMET_DEFS_H
#define SEKHMET_DEFS_H

#endif //SEKHMET_DEFS_H

#pragma once

#include <limits>
#include <cstdint>

typedef unsigned long long U64;

const U64 RANK_1 = 0xffull;
const U64 RANK_2 = 0xff00ull;
const U64 RANK_3 = 0xff0000ull;
const U64 RANK_4 = 0xff000000ull;
const U64 RANK_5 = 0xff00000000ull;
const U64 RANK_6 = 0xff0000000000ull;
const U64 RANK_7 = 0xff000000000000ull;
const U64 RANK_8 = 0xff00000000000000ull;


const U64 FILE_H = 0x8080808080808080ull;
const U64 FILE_G = 0x4040404040404040ull;
const U64 FILE_F = 0x2020202020202020ull;
const U64 FILE_E = 0x1010101010101010ull;
const U64 FILE_D = 0x808080808080808ull;
const U64 FILE_C = 0x404040404040404ull;
const U64 FILE_B = 0x202020202020202ull;
const U64 FILE_A = 0x101010101010101ull;

const U64 RANKS[8] = {RANK_1, RANK_2, RANK_3, RANK_4, RANK_5, RANK_6, RANK_7, RANK_8};
const U64 FILES[8] = {FILE_A, FILE_B, FILE_C, FILE_D, FILE_E, FILE_F, FILE_G, FILE_H};

const U64 BLACK_SQUARES = 0xAA55AA55AA55AA55;
const U64 WHITE_SQUARES = 0x55AA55AA55AA55AA;

const U64 ONE = U64(1);

const int INF = std::numeric_limits<int>::max();

enum PieceType {
    PAWN,
    KNIGHT,
    BISHOP,
    ROOK,
    QUEEN,
    KING,
    NULL_PIECE,
};

enum SquareIndex {
    a8, b8, c8, d8, e8, f8, g8, h8,
    a7, b7, c7, d7, e7, f7, g7, h7,
    a6, b6, c6, d6, e6, f6, g6, h6,
    a5, b5, c5, d5, e5, f5, g5, h5,
    a4, b4, c4, d4, e4, f4, g4, h4,
    a3, b3, c3, d3, e3, f3, g3, h3,
    a2, b2, c2, d2, e2, f2, g2, h2,
    a1, b1, c1, d1, e1, f1, g1, h1
};

