//
// Created by Tao G on 2/11/2023.
//

#ifndef SEKHMET_TESTS_H
#define SEKHMET_TESTS_H

#include "../models/Board.h"
#include "../core/MoveGen.h"
#include "../search/Search.h"

class tests {


public:
    static bool legalMovesTest(const Board& b, int expected = -1);

    static bool perfTest(const Board& b, int depth, int expected = -1);
    static bool perfTestMultithreaded(const Board &b, int depth, int expected = -1);

    static void NodeCountCompare(const Board &b, int depth, const std::string &compareTo);

private:
    static U64 nodeCount(const Board& b, int depth);
};


#endif //SEKHMET_TESTS_H
