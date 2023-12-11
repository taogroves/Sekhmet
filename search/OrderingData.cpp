//
// Created by Tao Groves on 2/17/23.
//

#include "OrderingData.h"
#include <cstring>

OrderingData::OrderingData() {
    _ttTable = nullptr;
    ply = 0;
    std::memset(historyHeuristicTable, 0, sizeof(historyHeuristicTable));
}

OrderingData::OrderingData(const zTable *table) {
    _ttTable = table;
    ply = 0;
    std::memset(historyHeuristicTable, 0, sizeof(historyHeuristicTable));
}

void OrderingData::clear() {
    ply = 0;
    std::memset(historyHeuristicTable, 0, sizeof(historyHeuristicTable));
    std::memset(killerMoves, 0, sizeof(killerMoves));
}

void OrderingData::nextMove() {
    // increment the killer moves and clear the history heuristic table
    ply = 0;
    incrementKillerMoves(2);
    //reduceHistoryHeuristic(2);
     std::memset(historyHeuristicTable[0], 0, sizeof(historyHeuristicTable[0]));
}

void OrderingData::updateKillerMoves(const Move &move) {
    killerMoves[ply][1] = killerMoves[ply][0];
    killerMoves[ply][0] = move;
}

Move OrderingData::getKillerMove(int p, int index) const {
    return killerMoves[p][index];
}

void OrderingData::incrementHistory(bool isWhite, int from, int to, int depth) {
    historyHeuristicTable[isWhite][from][to] += depth * depth;
}

int OrderingData::getHistoryHeuristic(bool isWhite, int from, int to) const {
    return historyHeuristicTable[isWhite][from][to];
}

void OrderingData::incrementKillerMoves(int numPly) {
    // move the killer moves toward the front of the table by the given # of plies
    for (int i = 0; i < 50 - numPly; i++) {
        killerMoves[i][0] = killerMoves[i + numPly][0];
        killerMoves[i][1] = killerMoves[i + numPly][1];
    }
}

void OrderingData::reduceHistoryHeuristic(int factor) {
    // reduce the history heuristic table by the given factor
    for (auto & i : historyHeuristicTable) {
        for (auto & j : i) {
            for (int & k : j) {
                k /= factor;
            }
        }
    }
}

