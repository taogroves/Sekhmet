//
// Created by Tao Groves on 2/27/23.
//

#include "../search/ParallelSearch.h"
#include "../search/eval.h"
#include "../core/MoveGen.h"

#include <iostream>

int main() {
    zKey::init();
    MoveGen::init();
    eval::init();

    Board b = Board("6k1/5p1p/p1p2p2/1bP5/1P1pP3/6PB/5P1P/6K1 w - - 0 34");

    ParallelSearcher searcher = ParallelSearcher();

    Move bestMove = searcher.timedSearch(b, 1000000);

    std::cout << bestMove.getNotation() << std::endl;

    return 0;
}