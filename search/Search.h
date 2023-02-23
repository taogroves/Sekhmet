//
// Created by Tao G on 2/12/2023.
//

#ifndef SEKHMET_SEARCH_H
#define SEKHMET_SEARCH_H


#include <atomic>
#include <chrono>
#include "../models/Board.h"
#include "../transpositions/zTable.h"
#include "OrderingData.h"

class Searcher {

public:
    struct searchRestrictions {
        bool infinite = false;
        int depth = 0;
        int movetime = 0;
        int time[2] = {0, 0};
        int increment[2] = {0, 0};
        int movesToGo = 0;
    };

    Searcher();

    Move restrictedSearch(const Board &b, const searchRestrictions &restrictions);
    Move timedSearch(const Board &b, int time);
    Move depthSearch(Board b, int depth, int alpha = -1000000, int beta = 1000000, int* extern_score = nullptr);
    int rootSearch(Board b, int depth, int alpha, int beta);

    void printPV(const Board &b, int depth);

    enum Algorithm {
        NEGAMAX,
        ZOBRIST,
        NULLMOVE_PVS,
        NL_LM_PVS
    };
    void setAlgorithm(Algorithm a);

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

    std::chrono::time_point<std::chrono::high_resolution_clock> startTime;
    int timeLimit = 0;
    std::atomic<bool> stopSearch = false;
    bool checkTime();
    int timeCheckRequestCount = 0;

    static const int TIME_CHECK_INTERVAL = 4096;

    static const int NULL_MOVE_REDUCTION = 3;

    static const int FULL_DEPTH_MOVES = 4;
    static const int REDUCTION_LIMIT = 3;

    zTable zobristTable;
    Algorithm algorithm = ZOBRIST;
    Evaluation evaluation = QUIESCENT_PST;


    OrderingData orderingData;
    Board initialBoard;


    int negamax(Board b, int depth, int alpha, int beta);
    int zobristNMax(const Board &b, int depth, int alpha, int beta);
    int nullMovePVS(const Board &b, int depth, int alpha, int beta, bool verify);

    int lateMovePVS(const Board &b, int depth, int alpha, int beta, bool verify);
    bool okToReduce(const Board &b, Move m);

};


#endif //SEKHMET_SEARCH_H
