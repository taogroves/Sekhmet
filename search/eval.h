//
// Created by Tao G on 2/12/2023.
//

#ifndef SEKHMET_EVAL_H
#define SEKHMET_EVAL_H
#include "../models/Board.h"
#include "OrderingData.h"

class eval {
public:
    static void init();

    static int evaluateBoard(bool isWhite, const Board &b);
    static short evaluateMaterial(bool isWhite, const Board &b);

    static int searchAllCaptures(bool isWhite, const Board &b, int alpha, int beta);
    static int qSearch(const Board &b, int alpha, int beta);

protected:

    constexpr static short DOUBLED_PAWN_PENALTY = 15;
    constexpr static short ISOLATED_PAWN_PENALTY = 15;
    constexpr static short PASSED_PAWN_BONUS = 50;
    constexpr static short DEFENDED_PAWN_BONUS = 5;

    constexpr static short pieceValues[6] = {100, 280, 305, 479, 929, 0};
    constexpr static short phaseWeights[6] = {0, 1, 1, 2, 4, 0};

    static short pieceSquareTables[2][6][64];
    static short pieceSquareTablesEndgame[2][6][64];

    static short numSetBits(U64 i);

    static int pstSlerp(bool isWhite, int square, PieceType piece, double phase);

};


#endif //SEKHMET_EVAL_H
