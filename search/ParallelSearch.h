//
// Created by Tao Groves on 2/24/23.
//

#ifndef SEKHMET_PARALLELSEARCH_H
#define SEKHMET_PARALLELSEARCH_H


#include <atomic>
#include <chrono>
#include "../models/Board.h"
#include "../transpositions/threadSafeTable.h"
#include "OrderingData.h"
#include "SearchThread.h"
//#include <vector>

class ParallelSearcher {

public:
    struct searchRestrictions {
        bool infinite = false;
        int depth = 0;
        int movetime = 0;
        int time[2] = {0, 0};
        int increment[2] = {0, 0};
        int movesToGo = 0;
    };

    ParallelSearcher();

    Move restrictedSearch(const Board &b, const searchRestrictions &restrictions);
    Move timedSearch(const Board &b, int time);
    void printPV(const Board &b, int depth);

    enum Evaluation {
        MATERIAL,
        QUIESCENT_MATERIAL,
        PST,
        QUIESCENT_PST,
    };

    void setEvaluation(Evaluation e);
    void setTimeLimit(int timeLimit);

    void reset(bool continueGame = false);
private:

    SearchThread threads[4];

    std::chrono::time_point<std::chrono::high_resolution_clock> startTime;
    int timeLimit = 0;
    std::atomic<bool> stopSearch = false;
    std::atomic<int> timeCheckRequestCount = 0;

    // higher for parallel search because checkTime() is called more often
    static const int TIME_CHECK_INTERVAL = 20000;

    static const int NULL_MOVE_REDUCTION = 3;

    static const int FULL_DEPTH_MOVES = 4;
    static const int REDUCTION_LIMIT = 3;

    threadSafeTable tTable;
    Evaluation evaluation = QUIESCENT_PST;

    Board initialBoard;

    int LM_NM_PVS_PARALLEL(const Board &b, int depth, int alpha, int beta, bool verify);
    bool okToReduce(const Board &b, Move m);

};



#endif //SEKHMET_PARALLELSEARCH_H
