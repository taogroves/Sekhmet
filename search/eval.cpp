//
// Created by Tao G on 2/12/2023.
//

#include "eval.h"
#include "../core/MoveGen.h"
#include "MovePicker.h"
#include "../core/LookupTables.h"

short eval::pieceSquareTables[2][6][64];
short eval::pieceSquareTablesEndgame[2][6][64];

void eval::init() {
    // initialize piece square tables
    for (int i = 0; i < 6; i++) {
        for (int col = 0; col < 8; col++) {
            for (int row = 0; row < 8; row++) {
                pieceSquareTables[1][i][row * 8 + col] = LookupTables::pieceSquareValues[i][row * 8 + col];
                pieceSquareTables[0][i][row * 8 + col] = LookupTables::pieceSquareValues[i][(7 - row) * 8 + col];

                pieceSquareTablesEndgame[1][i][row * 8 + col] = LookupTables::pieceSquareValuesEndGame[i][row * 8 + col];
                pieceSquareTablesEndgame[0][i][row * 8 + col] = LookupTables::pieceSquareValuesEndGame[i][(7 - row) * 8 + col];
            }
        }
    }
}

int eval::evaluateBoard(bool isWhite, const Board &b) {

    int score = 0;

    // calculate the phase of the game (1 = opening, 0 = endgame)
    double phase = 0;
    for (int i = 1; i < 5; i++) {
        phase += phaseWeights[i] * (numSetBits(b.bitboards[1][i]) + numSetBits(b.bitboards[0][i]));
    }
    phase /= 24;

    // for each non-king piece type
    for (int i = 0; i < 5; i++) {
        U64 pieces = b.bitboards[1][i];
        while (pieces) {
            //numPieces++;
            int square = __builtin_ctzll(pieces);
            pieces &= pieces - 1;
            score += pieceValues[i] + pstSlerp(true, square, (PieceType)i, phase);

            // bishops and queens are more valuable when they have more mobility
            if (i == BISHOP || i == QUEEN) {
                score += numSetBits(MoveGen::getBishopAttacks(square, b.occupancy[0] | b.occupancy[1])) * 2;
            }

            // rooks and queens are more valuable when they are on open files
            if (i == ROOK || i == QUEEN) {
                if (!(FILES[square % 8] & (b.bitboards[1][PAWN] | b.bitboards[0][PAWN]))) {
                    score += 20;
                }
            }

            if (i == PAWN) {
                // penalize for doubled pawns
                if (numSetBits(FILES[square % 8] & b.bitboards[1][PAWN]) > 1) {
                    score -= DOUBLED_PAWN_PENALTY;
                }

                // bonus for passed pawns
                if (!(LookupTables::passedPawnMasksW[square] & b.bitboards[0][PAWN])) {
                    score += PASSED_PAWN_BONUS;
                }
                // penalize for isolated pawns
                else if (!(LookupTables::isolatedPawnMasks[square % 8] & b.bitboards[1][PAWN])) {
                    score -= ISOLATED_PAWN_PENALTY;
                }
            }

        }
        pieces = b.bitboards[0][i];
        while (pieces) {
            //numPieces++;
            int square = __builtin_ctzll(pieces);
            pieces &= pieces - 1;
            score -= pieceValues[i] + pstSlerp(false, square, (PieceType)i, phase);

            // bishops and queens are more valuable when they have more mobility
            if (i == BISHOP || i == QUEEN) {
                score -= numSetBits(MoveGen::getBishopAttacks(square, b.occupancy[0] | b.occupancy[1])) * 2;
            }

            // rooks and queens are more valuable when they are on open files
            if (i == ROOK || i == QUEEN) {
                if (!(FILES[square % 8] & (b.bitboards[1][PAWN] | b.bitboards[0][PAWN]))) {
                    score -= 20;
                }
            }

            if (i == PAWN) {
                // penalize for doubled pawns
                if (numSetBits(FILES[square % 8] & b.bitboards[0][PAWN]) > 1) {
                    score += DOUBLED_PAWN_PENALTY;
                }

                // bonus for passed pawns
                if (!(LookupTables::passedPawnMasksB[square] & b.bitboards[1][PAWN])) {
                    score -= PASSED_PAWN_BONUS;
                }
                // penalize for isolated pawns
                else if (!(LookupTables::isolatedPawnMasks[square % 8] & b.bitboards[0][PAWN])) {
                    score += ISOLATED_PAWN_PENALTY;
                }
            }
        }
    }

    // add king scores
    score -= pstSlerp(false, __builtin_ctzll(b.bitboards[0][5]), KING, phase);
    score += pstSlerp(true, __builtin_ctzll(b.bitboards[1][5]), KING, phase);

    // the more pawns surrounding the king, the better
    // this becomes less important as the game progresses
    score += int((numSetBits(b.bitboards[1][PAWN] & LookupTables::kingMasks[__builtin_ctzll(b.bitboards[1][5])]) -
              numSetBits(b.bitboards[0][PAWN] & LookupTables::kingMasks[__builtin_ctzll(b.bitboards[0][5])])) * (10.0 * phase));

    // if your king is in the middle, it's bad to lose castling rights
    if (b.castlingRights != 0) {
        if (!(b.castlingRights & 0b1100)) {
            if (b.bitboards[0][5] & 0b00011100) {
                score += 140;
            }
        }
        if (!(b.castlingRights & 0b0011)) {
            if (b.bitboards[1][5] & 0b00011100ULL << 56) {
                score -= 140;
            }
        }
    }


    // add bonus for defended pawns
    U64 defendedPawnsW = (((b.bitboards[1][PAWN] & ~FILE_H) >> 7) | ((b.bitboards[1][PAWN] & ~FILE_A) >> 9)) & b.bitboards[1][PAWN];
    U64 defendedPawnsB = (((b.bitboards[0][PAWN] & ~FILE_H) << 9) | ((b.bitboards[0][PAWN] & ~FILE_A) << 7)) & b.bitboards[0][PAWN];
    score += (numSetBits(defendedPawnsW) - numSetBits(defendedPawnsB)) * DEFENDED_PAWN_BONUS;



    return isWhite ? score : -score;
}

short eval::evaluateMaterial(bool isWhite, const Board &b) {

    short score = 0;

    // loop through bitboards and count pieces
    for (int i = 0; i < 6; i++) {
        score += numSetBits(b.bitboards[1][i]) * pieceValues[i];
        score -= numSetBits(b.bitboards[0][i]) * pieceValues[i];
    }

    return isWhite ? score : -score;
}

short eval::numSetBits(U64 i) {

    i = i - ((i >> 1) & 0x5555555555555555UL);
    i = (i & 0x3333333333333333UL) + ((i >> 2) & 0x3333333333333333UL);
    return (short) ((((i + (i >> 4)) & 0xF0F0F0F0F0F0F0FUL) * 0x101010101010101UL) >> 56);
}


int eval::searchAllCaptures(bool isWhite, const Board &b, int alpha, int beta) {
    // if evaluation is greater than beta, return beta
    int eval = evaluateMaterial(isWhite, b);
    if (eval >= beta) {
        return beta;
    }
    alpha = std::max(alpha, eval);

    // get all moves and order them
    std::vector<Move> moves = MoveGen::getCaptures(b);
    QSearchMovePicker picker(&moves);

    // if there are no captures, return the evaluation
    if (moves.empty()) {
        return eval;
    }

    // loop through moves
    while(picker.hasNext()) {
        Move move = picker.getNext();

        // make the move
        Board newBoard = b;
        newBoard.makeMove(move);

        eval = -searchAllCaptures(!isWhite, newBoard, -beta, -alpha);
        if (eval >= beta) {
            return beta;
        }

        alpha = std::max(alpha, eval);
    }

    return alpha;
}

int eval::qSearch(const Board &b, int alpha, int beta) {

    int stand_pat = evaluateBoard(b.isWhiteTurn, b);

    // if evaluation is greater than beta, return
    if (stand_pat >= beta) {
        return beta;
    }
    if (alpha < stand_pat) {
        alpha = stand_pat;
    }

    MoveList legal = MoveGen::getCaptures(b);

    // look for checkmate
    if (legal.empty()) {
        if (b.isInCheck(b.isWhiteTurn)) {
            legal = MoveGen::getLegalMoves(b);
            if (legal.empty()) {
                return -1000000 + int(b.fullMoveNumber);
            }
        }
    }

    QSearchMovePicker picker(&legal);

    while(picker.hasNext()) {
        Move m = picker.getNext();

        Board copy = b;
        copy.makeMove(m);

        stand_pat = -qSearch(copy, -beta, -alpha);
        if (stand_pat >= beta) {
            return beta;
        }
        if (stand_pat > alpha) {
            alpha = stand_pat;
        }
    }

    return alpha;
}

int eval::pstSlerp(bool isWhite, int square, PieceType piece, double phase) {
    return int(pieceSquareTables[isWhite][piece][square] * phase + pieceSquareTablesEndgame[isWhite][piece][square] * (1.0 - phase));
}