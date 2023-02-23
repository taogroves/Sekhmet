//
// Created by Tao Groves on 2/9/23.
//

#include <iostream>
#include "tests.h"
#include "../core/LookupTables.h"
#include "../search/Search.h"
#include "../search/eval.h"


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

std::string compareMoves = "a2a3: 636\n"
                           "b2b3: 679\n"
                           "c2c3: 681\n"
                           "d2d3: 636\n"
                           "f2f3: 569\n"
                           "g2g3: 679\n"
                           "h2h3: 635\n"
                           "e3e4: 656\n"
                           "a2a4: 678\n"
                           "b2b4: 680\n"
                           "c2c4: 638\n"
                           "d2d4: 747\n"
                           "f2f4: 659\n"
                           "g2g4: 636\n"
                           "h2h4: 679\n"
                           "b1a3: 657\n"
                           "b1c3: 723\n"
                           "g1e2: 505\n"
                           "g1f3: 613\n"
                           "g1h3: 635\n"
                           "f1e2: 614\n"
                           "f1d3: 721\n"
                           "f1c4: 744\n"
                           "f1b5: 696\n"
                           "f1a6: 662\n"
                           "d1e2: 614\n"
                           "d1f3: 811\n"
                           "d1g4: 979\n"
                           "d1h5: 812\n"
                           "e1e2: 482\n";

int test() {
    zKey::init();
    //printLongAsSquare(9592139778506752);
    MoveGen::init();

    auto b = Board();
    b.makeMove(Move(e2, e4, PAWN));
    b.makeMove(Move(d7, d5, PAWN));
    b.makeMove(Move(d1, h5, QUEEN));
    b.printBoard();

    printLongAsSquare(4521260802379886UL);

    std::cout << MoveGen::getLegalMovesFast(b).size() << std::endl;

    return 0;
}