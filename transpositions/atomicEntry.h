//
// Created by Tao Groves on 2/24/23.
//

#ifndef SEKHMET_ATOMICENTRY_H
#define SEKHMET_ATOMICENTRY_H

#endif //SEKHMET_ATOMICENTRY_H

#include "../models/Move.h"
#include "../core/spinLock.h"
#include "TranspTableEntry.h"

class atomicEntry {
    public:
    // flag types
        enum Flag {
            EXACT, // exact score
            LOWERBOUND, // score is at least as good as the score
            UPPERBOUND, // score is at most as good as the score
        };

    atomicEntry(int score, int depth, Flag flag, Move bestMove);
    atomicEntry(TranspTableEntry data) {
        this->score = data.getScore();
        this->depth = data.getDepth();
        this->flag = static_cast<Flag>(data.getFlag());
        this->bestMove = data.getBestMove();
    }


        int getScore() const { return score; }
        int getDepth() const { return depth; }
        Flag getFlag() const { return flag; }
        Move getBestMove() const { return bestMove; }

    // lock this entry
   /* void lock() {
        threadLock.lock();
    }

    // unlock this entry
    void unlock() {
        threadLock.unlock();
    }*/

private:

    int score;
    int depth;
    Flag flag;
    Move bestMove;

protected:
    // used to lock this entry from other threads
    // spinlocks were chosen because they have no context switching overhead
    spinLock threadLock;

};

atomicEntry::atomicEntry(int score, int depth, Flag flag, Move bestMove) {
    this->score = score;
    this->depth = depth;
    this->flag = flag;
    this->bestMove = bestMove;
}
