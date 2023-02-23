//
// Created by Tao Groves on 2/9/23.
//

#ifndef SEKHMET_MOVES_H
#define SEKHMET_MOVES_H

#endif //SEKHMET_MOVES_H

#include <vector>
#include "../models/Board.h"
#include "../models/Move.h"

typedef std::vector<Move> MoveList;

class MoveGen {
public:
    static void init();

    static MoveList getValidMoves(const Board& b);
    static MoveList getLegalMoves(const Board& b);
    static MoveList getLegalMovesFast(const Board& b);
    static MoveList getCaptures(const Board& b);

    static void addMoves(MoveList* list, const Board &b, int square, PieceType piece, U64 destinations, U64 attackable);

    static U64 getBishopAttacks(int square, U64 blockers);
    static U64 getRookAttacks(int square, U64 blockers);

    static U64 bishopTable[64][1024];

    static U64 getBlockers(int square, U64 mask);
private:

    static U64 rookTable[64][4096];

    static const int MOVE_LIST_RESERVE = 218; // general upper bound for moves in a given position

    static void whitePawnMoves(MoveList* list, const Board &b);
    static void blackPawnMoves(MoveList* list, const Board &b);

    static void whiteKingMoves(MoveList* list, const Board &b);
    static void blackKingMoves(MoveList* list, const Board &b);
    static void kingMoves(MoveList *list, const Board &b, U64 king, U64 own, U64 attackable);

    static void knightMoves(MoveList* list, const Board &b, bool isWhite);

    static U64 rayBishopAttacks(int square, U64 blockers);

    static U64 rayRookAttacks(int square, U64 blockers);

    static void bishopQueenMoves(MoveList* list, const Board &b, bool isWhite);
    static void rookQueenMoves(MoveList* list, const Board &b, bool isWhite);

    // helper function to get blockers for sliding pieces


    // helper function to add all four promotion pieces
    static void genPawnPromotions(MoveList *list, int from, int to, int flags=0, PieceType=PAWN);

    // helper functions for bit manipulation
    static int popLSB(U64 &bb);
    static int popCount(U64 bb);
};


