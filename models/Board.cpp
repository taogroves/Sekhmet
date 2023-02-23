//
// Created by Tao G on 2/7/2023.
//

#include "Board.h"
#include "../core/LookupTables.h"
#include "../core/MoveGen.h"

const std::unordered_map<PieceType, int> Board::pieceToBoardMap = {
{PAWN, 0},
{KNIGHT, 1},
{BISHOP, 2},
{ROOK, 3},
{QUEEN, 4},
{KING, 5},
{PAWN, 6},
};

const std::unordered_map<char, int> Board::charToBoardMap = {
        {'P', 0},
        {'N', 1},
        {'B', 2},
        {'R', 3},
        {'Q', 4},
        {'K', 5},
        {'p', 0},
        {'n', 1},
        {'b', 2},
        {'r', 3},
        {'q', 4},
        {'k', 5}
};

const std::unordered_map<char, short> Board::rightsMap = {
        {'K', 1},
        {'Q', 2},
        {'k', 4},
        {'q', 8}
};

Board::Board() {
    // initialize board to default state
    bitboards[1][0] = 71776119061217280ULL;
    bitboards[1][1] = 4755801206503243776ULL;
    bitboards[1][2] = 2594073385365405696ULL;
    bitboards[1][3] = 9295429630892703744ULL;
    bitboards[1][4] = 576460752303423488ULL;
    bitboards[1][5] = 1152921504606846976ULL;
    bitboards[0][0] = 65280UL;
    bitboards[0][1] = 66UL;
    bitboards[0][2] = 36UL;
    bitboards[0][3] = 129UL;
    bitboards[0][4] = 8UL;
    bitboards[0][5] = 16UL;

    // initialize occupancy bitboards
    for (int i = 0; i < 2; i++) {
        occupancy[i] = 0;
        for (int j = 0; j < 6; j++) {
            occupancy[i] |= bitboards[i][j];
        }
    }

    // initialize variables
    isWhiteTurn = true;
    castlingRights = 15;
    enPassant = 0;
    halfMoveClock = 0;
    fullMoveNumber = 1;
    zobristKey = zKey(*this);
    positionHistory[0] = zobristKey.getValue();
}

/*Board::Board(const Board &b) {
    for (int i = 0; i < 6; i++) {
        for (int j = 0; j < 2; j++) {
            bitboards[j][i] = b.bitboards[j][i];
        }
    }

    isWhiteTurn = b.isWhiteTurn;
    castlingRights = b.castlingRights;
    enPassant = b.enPassant;
    halfMoveClock = b.halfMoveClock;
    fullMoveNumber = b.fullMoveNumber;

    occupancy[0] = b.occupancy[0];
    occupancy[1] = b.occupancy[1];

    checkSources = b.checkSources;

    //validMoves = b.validMoves;
    //captureMoves = b.captureMoves;
    //legalMoves = b.legalMoves;

    zobristKey = b.zobristKey;
}*/

Board::Board(const char *fen) {

    int i = 0;
    int j = 0;
    while (fen[i] != ' ') {
        auto x = fen[i];
        if (fen[i] == '/') {
            i++;
        }
        else if (fen[i] >= '1' && fen[i] <= '8') {
            j += fen[i] - '0';
            i ++;
            continue;
        }
        else {
            if (isupper(fen[i])) {
                bitboards[1][charToBoardMap.at(fen[i])] |= 1UL << j;
            }
            else {
                bitboards[0][charToBoardMap.at(fen[i])] |= 1UL << j;
            }
            j++;
            i++;
        }
    }

    isWhiteTurn = fen[i + 1] == 'w';

    castlingRights = 0;
    i += 3;
    while (fen[i] != ' ') {
        if (fen[i] == '-') {
            i++;
            break;
        }
        castlingRights |= rightsMap.at(fen[i]);
        i++;
    }
    enPassant = 0;
    if (fen[i + 1] != '-') {
        enPassant = 1UL << ((fen[i + 1] - 'a') + (fen[i + 2] - '1') * 8);
    }
    i += 4;
    halfMoveClock = 0;
    while (fen[i] != ' ') {
        halfMoveClock *= 10;
        halfMoveClock += fen[i] - '0';
        i++;
    }
    i++;
    fullMoveNumber = 0;
    while (fen[i] != '\0' && fen[i] != ' ') {
        fullMoveNumber *= 10;
        fullMoveNumber += fen[i] - '0';
        i++;
    }

    zobristKey = zKey(*this);
    positionHistory[0] = zobristKey.getValue();

    // initialize occupancy bitboards
    for (int i = 0; i < 2; i++) {
        occupancy[i] = 0;
        for (int j = 0; j < 6; j++) {
            occupancy[i] |= bitboards[i][j];
        }
    }
}


void Board::printBoard(bool showMoves, bool flipped) const {
    if (flipped)
        printf("   h g f e d c b a\n");
    else
        printf("   a b c d e f g h\n");
    printf("  -----------------\n");
    std::vector<Move> moves;
    if (showMoves) {
        //moves = getLegalMoves();
    }
    int start = 7;
    int end = 0;
    int increment = -1;
    if (flipped) {
        start = 0;
        end = 7;
        increment = 1;
    }
    for (int rank = start; rank != end + increment; rank += increment) {
        printf("%d |", rank + 1);
        for (int file = 0; file < 8; file++) {
            int square = (7 - rank) * 8 + file;
            if (flipped) {
                square = (7 - rank) * 8 + (7 - file);
            }
            if (showMoves) {
                for (auto move : moves) {
                    if (move.getTo() == square) {
                        printf("x|");
                        goto next;
                    }
                }
            }
            for (int i = 0; i < 2; i++) {
                for (int j = 0; j < 6; j++) {
                    if (bitboards[i][j] & (1UL << square)) {
                        printf("%c|", boardToPieceMap[j + (1-i) * 6]);
                        goto next;
                    }
                }
            }
            printf(" |");
            next:;
        }
        printf("|\n");
    }
    printf("  -----------------\n");
    if (flipped)
        printf("   h g f e d c b a\n");
    else
        printf("   a b c d e f g h\n");
    printf("%s to move | ", isWhiteTurn ? "White" : "Black");
    printf("Castling rights: %d\n", castlingRights);
}

void Board::nullMove() {

    // clear en passant
    enPassant = 0;

    // deal with full move number
    if (!isWhiteTurn) {
        fullMoveNumber++;
    }

    zobristKey.flipTurn();
    isWhiteTurn = !isWhiteTurn;
}

void Board::tempMove(Move move) {

    // same as makeMove but only does the bare minimum to update the board
    // used for move generation

    unsigned int flags = move.getFlags();
    if (!flags) { // quiet move
        movePiece(isWhiteTurn, move.getPieceType(), move.getFrom(), move.getTo());
    } else if (flags & Move::CAPTURE && flags & Move::PROMOTION) { // promoting from capture special case
        removePiece(isWhiteTurn, move.getPieceType(), move.getFrom());
        removePiece(!isWhiteTurn, move.getCapturedPieceType(), move.getTo());
        addPiece(isWhiteTurn, move.getPromotionPieceType(), move.getTo());
    } else if (flags & Move::CAPTURE) { // capture move
        removePiece(!isWhiteTurn, move.getCapturedPieceType(), move.getTo());
        movePiece(isWhiteTurn, move.getPieceType(), move.getFrom(), move.getTo());
    } else if (flags & Move::KSIDE_CASTLE) { // castle kingside
        // move the king
        movePiece(isWhiteTurn, PieceType::KING, move.getFrom(), move.getTo());

        // move the correct rook
        if (isWhiteTurn) {
            movePiece(isWhiteTurn, PieceType::ROOK, 63, 61);
        } else {
            movePiece(isWhiteTurn, PieceType::ROOK, 7, 5);
        }
    } else if (flags & Move::QSIDE_CASTLE) { // castle queenside
        // move the king
        movePiece(isWhiteTurn, PieceType::KING, move.getFrom(), move.getTo());

        // move the correct rook
        if (isWhiteTurn) {
            movePiece(isWhiteTurn, PieceType::ROOK, 56, 59);
        } else {
            movePiece(isWhiteTurn, PieceType::ROOK, 0, 3);
        }
    } else if (flags & Move::EN_PASSANT) { // en passant
        // remove the captured pawn
        if (isWhiteTurn) {
            removePiece(!isWhiteTurn, PieceType::PAWN, move.getTo() + 8);
        } else {
            removePiece(!isWhiteTurn, PieceType::PAWN, move.getTo() - 8);
        }

        // move the pawn
        movePiece(isWhiteTurn, PieceType::PAWN, move.getFrom(), move.getTo());
    } else if (flags & Move::PROMOTION) { // promotion
        removePiece(isWhiteTurn, PieceType::PAWN, move.getFrom());
        addPiece(isWhiteTurn, move.getPromotionPieceType(), move.getTo());
    } else if (flags & Move::DOUBLE_PAWN_PUSH) { // double pawn push
        // move the pawn
        movePiece(isWhiteTurn, PieceType::PAWN, move.getFrom(), move.getTo());
    }

    isWhiteTurn = !isWhiteTurn;

    // zobrist key, castling rights, en passant, etc. not updated
    // this method should only be used when no further moves are made on the board and it is not compared to any other board
}

void Board::makeMove(Move move) {
    // clear en passant from zobrist key
    if (enPassant) {
        zobristKey.clearEnPassant();
        enPassant = 0;
    }

    unsigned int flags = move.getFlags();
    if (!flags) { // quiet move
        movePiece(isWhiteTurn, move.getPieceType(), move.getFrom(), move.getTo());
    } else if (flags & Move::CAPTURE && flags & Move::PROMOTION) { // promoting from capture special case
        removePiece(isWhiteTurn, move.getPieceType(), move.getFrom());
        removePiece(!isWhiteTurn, move.getCapturedPieceType(), move.getTo());
        addPiece(isWhiteTurn, move.getPromotionPieceType(), move.getTo());
    } else if (flags & Move::CAPTURE) { // capture move
        removePiece(!isWhiteTurn, move.getCapturedPieceType(), move.getTo());
        movePiece(isWhiteTurn, move.getPieceType(), move.getFrom(), move.getTo());
    } else if (flags & Move::KSIDE_CASTLE) { // castle kingside
        // move the king
        movePiece(isWhiteTurn, PieceType::KING, move.getFrom(), move.getTo());

        // move the correct rook
        if (isWhiteTurn) {
            movePiece(isWhiteTurn, PieceType::ROOK, 63, 61);
        } else {
            movePiece(isWhiteTurn, PieceType::ROOK, 7, 5);
        }
    } else if (flags & Move::QSIDE_CASTLE) { // castle queenside
        // move the king
        movePiece(isWhiteTurn, PieceType::KING, move.getFrom(), move.getTo());

        // move the correct rook
        if (isWhiteTurn) {
            movePiece(isWhiteTurn, PieceType::ROOK, 56, 59);
        } else {
            movePiece(isWhiteTurn, PieceType::ROOK, 0, 3);
        }
    } else if (flags & Move::EN_PASSANT) { // en passant
        // remove the captured pawn
        if (isWhiteTurn) {
            removePiece(!isWhiteTurn, PieceType::PAWN, move.getTo() + 8);
        } else {
            removePiece(!isWhiteTurn, PieceType::PAWN, move.getTo() - 8);
        }

        // move the pawn
        movePiece(isWhiteTurn, PieceType::PAWN, move.getFrom(), move.getTo());
    } else if (flags & Move::PROMOTION) { // promotion
        removePiece(isWhiteTurn, PieceType::PAWN, move.getFrom());
        addPiece(isWhiteTurn, move.getPromotionPieceType(), move.getTo());
    } else if (flags & Move::DOUBLE_PAWN_PUSH) { // double pawn push
        // move the pawn
        movePiece(isWhiteTurn, PieceType::PAWN, move.getFrom(), move.getTo());

        // set en passant
        if (isWhiteTurn) {
            enPassant = ONE << (move.getTo() + 8);
        } else {
            enPassant = ONE << (move.getTo() - 8);
        }
        zobristKey.setEnPassant(short(enPassant % 8));
    }

    // deal with halfmove clock
    if (move.getPieceType() == PieceType::PAWN || move.getFlags() & Move::CAPTURE) {
        halfMoveClock = 0;
    } else {
        halfMoveClock++;
    }

    // deal with full move number
    if (!isWhiteTurn) {
        fullMoveNumber++;
    }

    // deal with 3-fold repetition
    positionHistory[halfMoveClock] = zobristKey.getValue();

    // update castling rights
    updateCastlingRights(move);

    zobristKey.flipTurn();
    isWhiteTurn = !isWhiteTurn;
}

zKey Board::getZobristKey() const {
    return zobristKey;
}

void Board::setZobristKey(zKey key) {
    zobristKey = key;
}

bool Board::operator==(const Board &b) const {
    return false;
}

void Board::generateCaptureMoves() {

}

void Board::generateLegalMoves() {

}

void Board::addPiece(bool isWhite, PieceType piece, int square) {
    U64 mask = ONE << square;

    bitboards[isWhite][piece] |= mask;
    occupancy[isWhite] |= mask;

    zobristKey.flipPiece(isWhite, piece, square);
}

void Board::removePiece(bool isWhite, PieceType piece, int square) {
    U64 mask = ONE << square;

    bitboards[isWhite][piece] ^= mask;
    occupancy[isWhite] ^= mask;

    zobristKey.flipPiece(isWhite, piece, square);
}

void Board::movePiece(bool isWhite, PieceType piece, int from, int to) {
    U64 mask = (ONE << from) | (ONE << to);

    bitboards[isWhite][piece] ^= mask;
    occupancy[isWhite] ^= mask;

    zobristKey.movePiece(isWhite, piece, from, to);

}

void Board::updateCastlingRights(Move m) {
    unsigned int flags = m.getFlags();

    // update rights if rooks have been captured
    if (flags & Move::CAPTURE) {
        switch (m.getTo()) {
            case a1: castlingRights &= ~2; break;
            case h1: castlingRights &= ~1; break;
            case a8: castlingRights &= ~8; break;
            case h8: castlingRights &= ~4; break;
        }
    }

    // update rights if rooks or kings have been moved
    switch (m.getFrom()) {
        case e1: castlingRights &= ~0x3; break;
        case a1: castlingRights &= ~0x2; break;
        case h1: castlingRights &= ~0x1; break;
        case e8: castlingRights &= ~0xC; break;
        case a8: castlingRights &= ~0x8; break;
        case h8: castlingRights &= ~0x4; break;
    }

    zobristKey.updateCastlingRights(castlingRights);
}

PieceType Board::getPieceOnSquare(int square, bool isWhite) const {
    for (int i = 0; i < 6; i++) {
        if (bitboards[isWhite][i] & (ONE << square)) {
            return PieceType(i);
        }
    }
    return PieceType::NULL_PIECE;
}

bool Board::isSquareAttacked(int square, bool isWhite) const {

    // check non-sliding attacks
    if (isWhite) {
        if (bitboards[isWhite][PieceType::PAWN] & (LookupTables::blackPawnAttacks[square])) {
            return true;
        }
    } else {
        if (bitboards[isWhite][PieceType::PAWN] & (LookupTables::whitePawnAttacks[square])) {
            return true;
        }
    }
    if (LookupTables::knightMasks[square] & bitboards[isWhite][PieceType::KNIGHT]) {
        return true;
    }
    if (LookupTables::kingMasks[square] & bitboards[isWhite][PieceType::KING]) {
        return true;
    }

    // check bishop/queen attacks
    U64 bishopsQueens = bitboards[isWhite][PieceType::BISHOP] | bitboards[isWhite][PieceType::QUEEN];
    if ((MoveGen::getBishopAttacks(square, occupancy[0] | occupancy[1])) & bishopsQueens) return true;

    // check rook/queen attacks
    U64 rooksQueens = bitboards[isWhite][PieceType::ROOK] | bitboards[isWhite][PieceType::QUEEN];
    if ((MoveGen::getRookAttacks(square, occupancy[0] | occupancy[1])) & rooksQueens) return true;

    return false;
}

bool Board::isInCheck(bool isWhite) const {
    int king = __builtin_ctzll(bitboards[isWhite][PieceType::KING]);

    // if there is no king, there is no check
    if (king == 64) return false;

    return isSquareAttacked(king, !isWhite);
}

bool Board::whiteCanCastleKingside() const {
    // check castling rights
    if (!(castlingRights & 1)) return false;

    U64 mask = (ONE << 61) | (ONE << 62);
    if (mask & (occupancy[true] | occupancy[false])) return false; // if any of the squares are occupied, can't castle
    if (isSquareAttacked(61, false) || isSquareAttacked(62, false)) return false; // if any of the squares are attacked, can't castle

    return !isInCheck(true);
}

bool Board::whiteCanCastleQueenside() const {
    // check castling rights
    if (!(castlingRights & 2)) return false;

    U64 mask = (ONE << 59) | (ONE << 58) | (ONE << 57);
    if (mask & (occupancy[true] | occupancy[false])) return false; // if any of the squares are occupied, can't castle
    if (isSquareAttacked(59, false) || isSquareAttacked(58, false)) return false; // if any of the squares are attacked, can't castle

    return !isInCheck(true);
}

bool Board::blackCanCastleKingside() const {
    // check castling rights
    if (!(castlingRights & 4)) return false;

    U64 mask = (ONE << 5) | (ONE << 6);
    if (mask & (occupancy[true] | occupancy[false])) return false; // if any of the squares are occupied, can't castle
    if (isSquareAttacked(5, true) || isSquareAttacked(6, true)) return false; // if any of the squares are attacked, can't castle

    return !isInCheck(false);
}

bool Board::blackCanCastleQueenside() const {
    // check castling rights
    if (!(castlingRights & 8)) return false;

    U64 mask = (ONE << 3) | (ONE << 2) | (ONE << 1);
    if (mask & (occupancy[true] | occupancy[false])) return false; // if any of the squares are occupied, can't castle
    if (isSquareAttacked(3, true) || isSquareAttacked(2, true)) return false; // if any of the squares are attacked, can't castle

    return !isInCheck(false);
}
