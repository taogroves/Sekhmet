//
// Created by Tao Groves on 2/9/23.
//

#include <iostream>
#include "MoveGen.h"
#include "LookupTables.h"

U64 MoveGen::rookTable[64][4096] = {{0}};
U64 MoveGen::bishopTable[64][1024] = {{0}};

void MoveGen::init() {
    // init rook table
    for (int square = 0; square < 64; square++) {
        // for all possible blocker configurations
        for (int blockerIndex = 0; blockerIndex < (1 << LookupTables::rookIndexBits[square]); blockerIndex++) {
            U64 blockers = getBlockers(blockerIndex, LookupTables::rookMasks[square]);
            rookTable[square][(blockers * LookupTables::rookMagics[square]) >> (64 - LookupTables::rookIndexBits[square])] = rayRookAttacks(square, blockers);
        }
    }

    // init bishop table
    for (int square = 0; square < 64; square++) {
        // for all possible blocker configurations
        for (int blockerIndex = 0; blockerIndex < (1 << LookupTables::bishopIndexBits[square]); blockerIndex++) {
            U64 blockers = getBlockers(blockerIndex, LookupTables::bishopMasks[square]);
            bishopTable[square][(blockers * LookupTables::bishopMagics[square]) >> (64 - LookupTables::bishopIndexBits[square])] = rayBishopAttacks(square, blockers);
        }
    }
}

MoveList MoveGen::getValidMoves(const Board& b) {
    MoveList list;
    list.reserve(MOVE_LIST_RESERVE);

    if (b.isWhiteTurn) {
        whitePawnMoves(&list, b);
        whiteKingMoves(&list, b);
        knightMoves(&list, b, true);
        bishopQueenMoves(&list, b, true);
        rookQueenMoves(&list, b, true);
    } else {
        blackPawnMoves(&list, b);
        blackKingMoves(&list, b);
        knightMoves(&list, b, false);
        bishopQueenMoves(&list, b, false);
        rookQueenMoves(&list, b, false);
    }

    return list;
}

MoveList MoveGen::getLegalMoves(const Board& b) {
    MoveList valid = getValidMoves(b);

    MoveList legal;
    legal.reserve(valid.size());

    // remove all moves that leave king in check
    for (auto move : valid) { // TODO can this be done more easily?

        Board copy = b;
        copy.tempMove(move);
        if (!copy.isInCheck(b.isWhiteTurn)) {
            legal.push_back(move);
        }
    }

    return legal;
}

MoveList MoveGen::getLegalMovesFast(const Board& b) {
    MoveList valid = getValidMoves(b);

    MoveList legal;
    legal.reserve(valid.size());

    // if the king is in check, revert to slow method
    if (b.isInCheck(b.isWhiteTurn)) {
        for (auto move : valid) {
            Board copy = b;
            copy.tempMove(move);
            if (!copy.isInCheck(b.isWhiteTurn)) {
                legal.push_back(move);
            }
        }
        return legal;
    }

    int king = __builtin_ctzll(b.bitboards[b.isWhiteTurn][KING]);
    U64 kingB = LookupTables::bishopMasks[king];
    U64 kingR = LookupTables::rookMasks[king];


    // for each move
    for (auto move : valid) {
        U64 from = (1UL << move.getFrom());
        U64 to = (1UL << move.getTo());

        U64 blockers = ((b.occupancy[0] | b.occupancy[1]) ^ from) | to;

        // if the move is a king move, do a full check
        if (move.getPieceType() == KING) {
            Board copy = b;
            copy.tempMove(move);
            if (!copy.isInCheck(b.isWhiteTurn)) {
                legal.push_back(move);
            }
            continue;
        }

        // if it's a pawn move
        if (move.getPieceType() == PAWN) {
            // if the move is an en passant, remove the captured piece
            if (move.getFlags() & Move::EN_PASSANT) {
                blockers ^= (1UL << (move.getTo() + (b.isWhiteTurn ? 8 : -8)));
            }

            // if the move is along a vert/diagonal with the king, it must be legal
            if (popCount((from | to) & (kingB | kingR)) == 2) {
                legal.push_back(move);
                continue;
            }
            // if the move starts on the same diagonal, check that line only
            else if (from & kingB) {
                // if there is a bishop/queen on the same diagonal, the move is illegal
                if (getBishopAttacks(king, blockers) & (b.bitboards[!b.isWhiteTurn][BISHOP] | b.bitboards[!b.isWhiteTurn][QUEEN])) {
                    continue;
                }
            }
            // if the move starts on the same vert, check that line only
            else if (from & kingR) {
                // if there is a rook/queen on the same vert, the move is illegal
                if (getRookAttacks(king, blockers) & (b.bitboards[!b.isWhiteTurn][ROOK] | b.bitboards[!b.isWhiteTurn][QUEEN])) {
                    continue;
                }
            }

            legal.push_back(move);
            continue;
        }

        // if it's a knight move
        else if (move.getPieceType() == KNIGHT) {
            // if we are moving from a king line, check the file
            if (from & (kingB)) {
                // if there is a bishop/queen on the same file, the move is illegal
                if (getBishopAttacks(king, blockers) & (b.bitboards[!b.isWhiteTurn][ROOK] | b.bitboards[!b.isWhiteTurn][QUEEN])) {
                    continue;
                }
            } else if (from & (kingR)) {
                // if there is a rook/queen on the same file, the move is illegal
                if (getRookAttacks(king, blockers) & (b.bitboards[!b.isWhiteTurn][ROOK] | b.bitboards[!b.isWhiteTurn][QUEEN])) {
                    continue;
                }
            }

            legal.push_back(move);
            continue;
        }

        // if it's a bishop, rook, or queen move
        else {
            // if we are moving out of a king line, check the file
            if (popCount((from | to) & (kingB | kingR)) == 2) {
                legal.push_back(move);
                continue;
            }
            // if there is a bishop/queen on the same file, the move is illegal
            if ((from) & (kingB)) {
                if (getBishopAttacks(king, blockers) &
                    (b.bitboards[!b.isWhiteTurn][BISHOP] | b.bitboards[!b.isWhiteTurn][QUEEN])) {
                    continue;
                }
            }
            else if ((from) & (kingR)) {
                // if there is a rook/queen on the same file, the move is illegal
                if (getRookAttacks(king, blockers) & (b.bitboards[!b.isWhiteTurn][ROOK] | b.bitboards[!b.isWhiteTurn][QUEEN])) {
                    continue;
                }
            }

            legal.push_back(move);
            continue;
        }
    }

    return legal;
}

MoveList MoveGen::getCaptures(const Board &b) {
    MoveList list;
    list.reserve(MOVE_LIST_RESERVE);

    if (b.isWhiteTurn) {
        // white pawn captures
        U64 whitePawns = b.bitboards[1][PAWN];
        U64 whitePawnCapturesL = (whitePawns >> 7) & ~FILE_A & b.occupancy[0];
        U64 whitePawnCapturesR = (whitePawns >> 9) & ~FILE_H & b.occupancy[0];
        // promotions
        U64 whitePawnPromotionsL = whitePawnCapturesL & 0xFFULL;
        U64 whitePawnPromotionsR = whitePawnCapturesR & 0xFFULL;
        whitePawnCapturesL &= ~0xFFULL;
        whitePawnCapturesR &= ~0xFFULL;

        // add all captures
        while (whitePawnCapturesL) {
            int to = popLSB(whitePawnCapturesL);
            Move m = Move(to + 7, to, PAWN, Move::CAPTURE);
            m.setCapturedPieceType(b.getPieceOnSquare(to, false));
            list.push_back(m);
        }
        while (whitePawnCapturesR) {
            int to = popLSB(whitePawnCapturesR);
            Move m = Move(to + 9, to, PAWN, Move::CAPTURE);
            m.setCapturedPieceType(b.getPieceOnSquare(to, false));
            list.push_back(m);
        }
        while (whitePawnPromotionsL) {
            int to = popLSB(whitePawnPromotionsL);
            genPawnPromotions(&list, to + 7, to, Move::CAPTURE, b.getPieceOnSquare(to, false));
        }
        while (whitePawnPromotionsR) {
            int to = popLSB(whitePawnPromotionsR);
            genPawnPromotions(&list, to + 9, to, Move::CAPTURE, b.getPieceOnSquare(to, false));
        }
    } else {
        // black pawn captures
        U64 blackPawns = b.bitboards[0][PAWN];
        U64 blackPawnCapturesL = (blackPawns << 9) & ~FILE_A & b.occupancy[1];
        U64 blackPawnCapturesR = (blackPawns << 7) & ~FILE_H & b.occupancy[1];
        // promotions
        U64 blackPawnPromotionsL = blackPawnCapturesL & 0xFF00000000000000ULL;
        U64 blackPawnPromotionsR = blackPawnCapturesR & 0xFF00000000000000ULL;
        blackPawnCapturesL &= ~0xFF00000000000000ULL;
        blackPawnCapturesR &= ~0xFF00000000000000ULL;

        // add all captures
        while (blackPawnCapturesL) {
            int to = popLSB(blackPawnCapturesL);
            Move m = Move(to - 9, to, PAWN, Move::CAPTURE);
            m.setCapturedPieceType(b.getPieceOnSquare(to, true));
            list.push_back(m);
        }
        while (blackPawnCapturesR) {
            int to = popLSB(blackPawnCapturesR);
            Move m = Move(to - 7, to, PAWN, Move::CAPTURE);
            m.setCapturedPieceType(b.getPieceOnSquare(to, true));
            list.push_back(m);
        }
        while (blackPawnPromotionsL) {
            int to = popLSB(blackPawnPromotionsL);
            genPawnPromotions(&list, to - 9, to, Move::CAPTURE, b.getPieceOnSquare(to, true));
        }
        while (blackPawnPromotionsR) {
            int to = popLSB(blackPawnPromotionsR);
            genPawnPromotions(&list, to - 7, to, Move::CAPTURE, b.getPieceOnSquare(to, true));
        }
    }

    // knight captures
    U64 knights = b.bitboards[b.isWhiteTurn][KNIGHT];
    U64 attackable = b.occupancy[!b.isWhiteTurn];
    while (knights) {
        int from = popLSB(knights);
        U64 destinations = LookupTables::knightMasks[from] & attackable;
        addMoves(&list, b, from, KNIGHT, destinations, attackable);
    }

    // bishop captures
    U64 bishops = b.bitboards[b.isWhiteTurn][BISHOP];
    while (bishops) {
        int from = popLSB(bishops);
        U64 destinations = getBishopAttacks(from, b.occupancy[b.isWhiteTurn] | b.occupancy[!b.isWhiteTurn]) & attackable;
        addMoves(&list, b, from, BISHOP, destinations, attackable);
    }

    // rook captures
    U64 rooks = b.bitboards[b.isWhiteTurn][ROOK];
    while (rooks) {
        int from = popLSB(rooks);
        U64 destinations = getRookAttacks(from, b.occupancy[b.isWhiteTurn] | b.occupancy[!b.isWhiteTurn]) & attackable;
        addMoves(&list, b, from, ROOK, destinations, attackable);
    }

    // queen captures
    U64 queens = b.bitboards[b.isWhiteTurn][QUEEN];
    while (queens) {
        int from = popLSB(queens);
        U64 destinations = getBishopAttacks(from, b.occupancy[b.isWhiteTurn] | b.occupancy[!b.isWhiteTurn]) & attackable;
        destinations |= getRookAttacks(from, b.occupancy[b.isWhiteTurn] | b.occupancy[!b.isWhiteTurn]) & attackable;
        addMoves(&list, b, from, QUEEN, destinations, attackable);
    }

    // king captures
    U64 kings = b.bitboards[b.isWhiteTurn][KING];
    while (kings) {
        int from = popLSB(kings);
        U64 destinations = LookupTables::kingMasks[from] & attackable;
        addMoves(&list, b, from, KING, destinations, attackable);
    }

    std::vector<Move> legal;
    legal.reserve(list.size());
    // check legality
    for (Move &move : list) {
        Board copy = b;
        copy.tempMove(move);
        if (!copy.isInCheck(b.isWhiteTurn)) {
            legal.push_back(move);
        }
    }

    return legal;
}

void MoveGen::addMoves(MoveList *list, const Board &b, int square, PieceType piece, U64 destinations, U64 attackable) {
    // ignore all moves/attacks to kings
    destinations &= ~b.bitboards[!b.isWhiteTurn][KING];

    // add all moves to empty squares
    U64 nonAttacks = destinations & ~attackable;
    while (nonAttacks) {
        int to = popLSB(nonAttacks);
        list->push_back(Move(square, to, piece));
    }

    // add all attacks
    U64 attacks = destinations & attackable;
    while (attacks) {
        int to = popLSB(attacks);

        Move move = Move(square, to, piece, Move::CAPTURE);
        move.setCapturedPieceType(b.getPieceOnSquare(to, !b.isWhiteTurn));

        list->push_back(move);
    }
}

void MoveGen::genPawnPromotions(MoveList *list, int from, int to, int flags, PieceType captured) {
    Move base = Move(from, to, PAWN, flags | Move::PROMOTION);
    if (flags & Move::CAPTURE) {
        base.setCapturedPieceType(captured);
    }

    Move queen = base;
    queen.setPromotionPieceType(QUEEN);
    list->push_back(queen);

    Move rook = base;
    rook.setPromotionPieceType(ROOK);
    list->push_back(rook);

    Move bishop = base;
    bishop.setPromotionPieceType(BISHOP);
    list->push_back(bishop);

    Move knight = base;
    knight.setPromotionPieceType(KNIGHT);
    list->push_back(knight);
}

void MoveGen::whitePawnMoves(MoveList *list, const Board &b) {
    // single moves
    U64 singleMoves = (b.bitboards[1][PAWN] >> 8) & ~b.occupancy[1] & ~b.occupancy[0];
    U64 promotions = singleMoves & 0xFFULL;
    singleMoves &= ~promotions;

    // double moves
    U64 doubleMoves = ((singleMoves & (0xFFULL << 40)) >> 8) & ~b.occupancy[1] & ~b.occupancy[0];

    // captures
    U64 capturesL = (b.bitboards[1][PAWN] >> 7) & ~FILE_A & b.occupancy[0];
    U64 capturesR = (b.bitboards[1][PAWN] >> 9) & ~FILE_H & b.occupancy[0];
    U64 promotionsCL = capturesL & 0xFFULL;
    U64 promotionsCR = capturesR & 0xFFULL;
    capturesL &= ~promotionsCL;
    capturesR &= ~promotionsCR;

    // en passant captures
    U64 enPassantL = (b.bitboards[1][PAWN] >> 7) & ~FILE_A & b.enPassant;
    U64 enPassantR = (b.bitboards[1][PAWN] >> 9) & ~FILE_H & b.enPassant;

    // add all single moves
    while (singleMoves) {
        int to = popLSB(singleMoves);
        list->push_back(Move(to + 8, to, PAWN));
    }

    // add all double moves
    while (doubleMoves) {
        int to = popLSB(doubleMoves);
        list->push_back(Move(to + 16, to, PAWN, Move::DOUBLE_PAWN_PUSH));
    }

    // add all captures
    while (capturesL) {
        int to = popLSB(capturesL);
        Move m = Move(to + 7, to, PAWN, Move::CAPTURE);
        m.setCapturedPieceType(b.getPieceOnSquare(to, false));
        list->push_back(m);
    }
    while (capturesR) {
        int to = popLSB(capturesR);
        Move m = Move(to + 9, to, PAWN, Move::CAPTURE);
        m.setCapturedPieceType(b.getPieceOnSquare(to, false));
        list->push_back(m);
    }
    while (enPassantL) {
        int to = popLSB(enPassantL);
        list->push_back(Move(to + 7, to, PAWN, Move::EN_PASSANT));
    }
    while (enPassantR) {
        int to = popLSB(enPassantR);
        list->push_back(Move(to + 9, to, PAWN, Move::EN_PASSANT));
    }

    // add all promotions
    while (promotions) {
        int to = popLSB(promotions);
        genPawnPromotions(list, to + 8, to);
    }
    while (promotionsCL) {
        int to = popLSB(promotionsCL);
        genPawnPromotions(list, to + 7, to, Move::CAPTURE, b.getPieceOnSquare(to , false));
    }
    while (promotionsCR) {
        int to = popLSB(promotionsCR);
        genPawnPromotions(list, to + 9, to, Move::CAPTURE, b.getPieceOnSquare(to, false));
    }
}

void MoveGen::blackPawnMoves(MoveList *list, const Board &b) {
    // single moves
    U64 singleMoves = (b.bitboards[0][PAWN] << 8) & ~b.occupancy[1] & ~b.occupancy[0];
    U64 promotions = singleMoves & 0xFF00000000000000ULL;
    singleMoves &= ~promotions;

    // double moves
    U64 doubleMoves = ((singleMoves & (0xFFULL << 16)) << 8) & ~b.occupancy[1] & ~b.occupancy[0];

    // captures
    U64 capturesL = (b.bitboards[0][PAWN] << 7) & ~FILE_H & b.occupancy[1];
    U64 capturesR = (b.bitboards[0][PAWN] << 9) & ~FILE_A & b.occupancy[1];
    U64 promotionsCL = capturesL & 0xFF00000000000000ULL;
    U64 promotionsCR = capturesR & 0xFF00000000000000ULL;
    capturesL &= ~promotionsCL;
    capturesR &= ~promotionsCR;

    // en passant captures
    U64 enPassantL = (b.bitboards[0][PAWN] << 7) & ~FILE_H & b.enPassant;
    U64 enPassantR = (b.bitboards[0][PAWN] << 9) & ~FILE_A & b.enPassant;

    // add all single moves
    while (singleMoves) {
        int to = popLSB(singleMoves);
        list->push_back(Move(to - 8, to, PAWN, 0));
    }

    // add all double moves
    while (doubleMoves) {
        int to = popLSB(doubleMoves);
        list->push_back(Move(to - 16, to, PAWN, Move::DOUBLE_PAWN_PUSH));
    }

    // add all captures
    while (capturesL) {
        int to = popLSB(capturesL);
        Move m = Move(to - 7, to, PAWN, Move::CAPTURE);
        m.setCapturedPieceType(b.getPieceOnSquare(to, true));
        list->push_back(m);
    }
    while (capturesR) {
        int to = popLSB(capturesR);
        Move m = Move(to - 9, to, PAWN, Move::CAPTURE);
        m.setCapturedPieceType(b.getPieceOnSquare(to, true));
        list->push_back(m);
    }
    if (enPassantL) { // no need for loop, only one en passant capture possible
        int to = popLSB(enPassantL);
        Move m = Move(to - 7, to, PAWN, Move::EN_PASSANT);
        m.setCapturedPieceType(PAWN);
        list->push_back(m);
    }
    if (enPassantR) { // no need for loop, only one en passant capture possible
        int to = popLSB(enPassantR);
        Move m = Move(to - 9, to, PAWN, Move::EN_PASSANT);
        m.setCapturedPieceType(PAWN);
        list->push_back(m);
    }

    // add all promotions
    while (promotions) {
        int to = popLSB(promotions);
        genPawnPromotions(list, to - 8, to);
    }
    while (promotionsCL) {
        int to = popLSB(promotionsCL);
        genPawnPromotions(list, to - 7, to, Move::CAPTURE, b.getPieceOnSquare(to, true));
    }
    while (promotionsCR) {
        int to = popLSB(promotionsCR);
        genPawnPromotions(list, to - 9, to, Move::CAPTURE, b.getPieceOnSquare(to, true));
    }
}

void MoveGen::whiteKingMoves(MoveList *list, const Board &b) {

    kingMoves(list, b, b.bitboards[1][KING], b.occupancy[1], b.occupancy[0] | b.bitboards[0][KING]);

    // castling
    if (b.whiteCanCastleKingside()) {
        // white kingside
        list->push_back(Move(e1, g1, KING, Move::KSIDE_CASTLE));
    }
    if (b.whiteCanCastleQueenside()) {
        // white queenside
        list->push_back(Move(e1, c1, KING, Move::QSIDE_CASTLE));
    }
}

void MoveGen::blackKingMoves(MoveList *list, const Board &b) {

    kingMoves(list, b, b.bitboards[0][KING], b.occupancy[0], b.occupancy[1] | b.bitboards[1][KING]);

    // castling
    if (b.blackCanCastleKingside()) {
        // black kingside
        list->push_back(Move(e8, g8, KING, Move::KSIDE_CASTLE));
    }
    if (b.blackCanCastleQueenside()) {
        // black queenside
        list->push_back(Move(e8, c8, KING, Move::QSIDE_CASTLE));
    }
}

void MoveGen::kingMoves(MoveList *list, const Board &b, U64 king, U64 own, U64 attackable) {
    if (king == 0) return; // no king

    int square = popLSB(king);

    U64 destinations = LookupTables::kingMasks[square] & ~own;
    addMoves(list, b, square, KING, destinations, attackable);
}

void MoveGen::knightMoves(MoveList *list, const Board &b, bool isWhite) {
    U64 knights = b.bitboards[isWhite][KNIGHT];
    U64 attackable = b.occupancy[!isWhite];

    while (knights) {
        int square = popLSB(knights);
        U64 destinations = LookupTables::knightMasks[square] & ~b.occupancy[isWhite];
        addMoves(list, b, square, KNIGHT, destinations, attackable);
    }
}

void MoveGen::bishopQueenMoves(MoveList *list, const Board &b, bool isWhite) {
    U64 bishops = b.bitboards[isWhite][BISHOP];
    U64 queens = b.bitboards[isWhite][QUEEN];
    U64 attackable = b.occupancy[!isWhite] ^ b.bitboards[!isWhite][KING];

    while (bishops) {
        int square = popLSB(bishops);
        U64 destinations = getBishopAttacks(square, b.occupancy[isWhite] | b.occupancy[!isWhite]) & ~b.occupancy[isWhite];
        addMoves(list, b, square, BISHOP, destinations, attackable);
    }
    while (queens) {
        int square = popLSB(queens);
        U64 destinations = getBishopAttacks(square, b.occupancy[isWhite] | b.occupancy[!isWhite]) & ~b.occupancy[isWhite];
        addMoves(list, b, square, QUEEN, destinations, attackable);
    }
}

U64 MoveGen::rayBishopAttacks(int square, U64 blockers) {
    U64 attacks = 0;
    int directions[4] = {7, 9, -7, -9};

    for (int dir : directions) {

        for (int j = 1; j <= LookupTables::numSquaresToEdge[square][LookupTables::dirToIndex.at(dir)]; j++) {
            attacks |= (1ULL << (square + j * dir));
            if (blockers & (1ULL << (square + j * dir))) break;
        }

    }

    return attacks;
}

void MoveGen::rookQueenMoves(MoveList *list, const Board &b, bool isWhite) {
    U64 rooks = b.bitboards[isWhite][ROOK];
    U64 queens = b.bitboards[isWhite][QUEEN];
    U64 attackable = b.occupancy[!isWhite] ^ b.bitboards[!isWhite][KING];

    while (rooks) {
        int square = popLSB(rooks);
        U64 destinations = getRookAttacks(square, b.occupancy[isWhite] | b.occupancy[!isWhite]) & ~b.occupancy[isWhite];
        addMoves(list, b, square, ROOK, destinations, attackable);
    }
    while (queens) {
        int square = popLSB(queens);
        U64 destinations = getRookAttacks(square, b.occupancy[isWhite] | b.occupancy[!isWhite]) & ~b.occupancy[isWhite];
        addMoves(list, b, square, QUEEN, destinations, attackable);
    }
}

U64 MoveGen::rayRookAttacks(int square, U64 blockers) {
    U64 attacks = 0;
    int file = square % 8;
    int rank = square / 8;

    // up
    for (int i = 1; i < 8; i++) {
        if (rank + i > 7) break;
        attacks |= ONE << (square + 8 * i);
        if (blockers & (ONE << (square + 8 * i))) break;
    }

    // down
    for (int i = 1; i < 8; i++) {
        if (rank - i < 0) break;
        attacks |= ONE << (square - 8 * i);
        if (blockers & (ONE << (square - 8 * i))) break;
    }

    // right
    for (int i = 1; i < 8; i++) {
        if (file + i > 7) break;
        attacks |= ONE << (square + i);
        if (blockers & (ONE << (square + i))) break;
    }

    // left
    for (int i = 1; i < 8; i++) {
        if (file - i < 0) break;
        attacks |= ONE << (square - i);
        if (blockers & (ONE << (square - i))) break;
    }

    return attacks;
}

int MoveGen::popLSB(U64 &bb) {
    // remove the least significant bit from the bitboard and return its index
    int square = __builtin_ctzll(bb);
    bb &= bb - 1;
    return square;
}

U64 MoveGen::getBishopAttacks(int square, U64 blockers) {
    blockers &= LookupTables::bishopMasks[square];
    return bishopTable[square][((blockers * LookupTables::bishopMagics[square]) >> (64 - LookupTables::bishopIndexBits[square]))];
}

U64 MoveGen::getBlockers(int square, U64 mask) {


    U64 blockers = 0;
    U64 la = mask;
    int bits = popCount(mask);
    for (int i = 0; i < bits; i++) {
        int bitPos = popLSB(mask);
        if (square & (1 << i)) {
            blockers |= ONE << bitPos;
        }
    }
    return blockers;
}


U64 MoveGen::getRookAttacks(int square, U64 blockers) {
    blockers &= LookupTables::rookMasks[square];
    return rookTable[square][(blockers * LookupTables::rookMagics[square]) >> (64 - LookupTables::rookIndexBits[square])];
}

int MoveGen::popCount(U64 bb) {
    //return __builtin_popcountll(bb);
    U64 i = bb;
    i = i - ((i >> 1) & 0x5555555555555555UL);
    i = (i & 0x3333333333333333UL) + ((i >> 2) & 0x3333333333333333UL);
    return (int) ((((i + (i >> 4)) & 0xF0F0F0F0F0F0F0FUL) * 0x101010101010101UL) >> 56);
}
