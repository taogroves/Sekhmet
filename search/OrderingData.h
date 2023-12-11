//
// Created by Tao Groves on 2/17/23.
//

#ifndef SEKHMET_ORDERINGDATA_H
#define SEKHMET_ORDERINGDATA_H

#include "../models/Move.h"
#include "../transpositions/zTable.h"

class OrderingData {

public:

    OrderingData();
    OrderingData(const zTable *ttable);

    void clear();
    void nextMove();

    const zTable *getTtTable() const { return _ttTable; }

    void incrementPly() { ply++; }
    void decrementPly() { ply--; }
    int getPly() const { return ply; }

    void incrementHistory(bool isWhite, int from, int to, int depth);
    int getHistoryHeuristic(bool isWhite, int from, int to) const;

    void updateKillerMoves(const Move &move);
    Move getKillerMove(int ply, int index) const;

private:

    const zTable *_ttTable;
    int historyHeuristicTable[2][64][64] = {{{0}}};
    int ply;
    Move killerMoves[50][2];

    void incrementKillerMoves(int numPly);
    void reduceHistoryHeuristic(int factor);
};


#endif //SEKHMET_ORDERINGDATA_H
