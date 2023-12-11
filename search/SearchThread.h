//
// Created by Tao Groves on 2/24/23.
//

#ifndef SEKHMET_SEARCHTHREAD_H
#define SEKHMET_SEARCHTHREAD_H

//#include "../models/Move.h"
#include "../models/Board.h"
#include "OrderingData.h"
#include "../transpositions/threadSafeTable.h"

// small search process that can be run in parallel
// each thread has its own killer moves and history, but shares the transposition table
class SearchThread {

public:

    SearchThread();
    SearchThread(threadSafeTable *tTable, Board startingBoard, int threadID, int depth, int alpha, int beta,
                 std::chrono::time_point<std::chrono::high_resolution_clock> *startTime, int timeLimit,
                 std::atomic<bool> *stopSearch, std::atomic<int> *timeCheckCount);

    void run();
    //std::atomic<bool> abort = false;
    //bool abort = false;

    Move getBestMove();
    int getBestScore() const;

    static const int NULL_MOVE_REDUCTION = 3;

    static const int FULL_DEPTH_MOVES = 4;
    static const int REDUCTION_LIMIT = 3;

private:

    std::chrono::time_point<std::chrono::high_resolution_clock> *startTime;
    int timeLimit = 0;
    std::atomic<bool> *stopSearch;

    std::atomic<int> *timeCheckCount;
    static const int TIME_CHECK_INTERVAL = 20000;

    bool checkTime();

    OrderingData orderingData;
    threadSafeTable *tTable;

    Board startingBoard;
    int threadID{};
    int rootDepth{};
    int lowerBound{};
    int upperBound{};

    Move bestMove;
    int bestScore{};

    int search(const Board &b, int depth, int alpha, int beta, bool verify);
    bool okToReduce(const Board &b, Move m);

};


#endif //SEKHMET_SEARCHTHREAD_H
