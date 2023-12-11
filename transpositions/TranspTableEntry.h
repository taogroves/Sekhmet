//
// Created by Tao G on 2/13/2023.
//

#ifndef SEKHMET_TRANSPTABLEENTRY_H
#define SEKHMET_TRANSPTABLEENTRY_H

#endif //SEKHMET_TRANSPTABLEENTRY_H

#include "../models/Move.h"
#include "../core/spinLock.h"

class TranspTableEntry {

public:

    // flag types
    enum Flag {
        EXACT, // exact score
        LOWERBOUND, // score is at least as good as the score
        UPPERBOUND, // score is at most as good as the score
    };

    TranspTableEntry(int score, int depth, Flag flag, Move bestMove)
        : score(score), depth(depth), flag(flag), bestMove(bestMove) {}


    int getScore() const { return score; }
    int getDepth() const { return depth; }
    Flag getFlag() const { return flag; }
    Move getBestMove() const { return bestMove; }

private:

        int score;
        int depth;
        Flag flag;
        Move bestMove;
};