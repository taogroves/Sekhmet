//
// Created by Tao G on 2/7/2023.
//

#ifndef SEKHMET_BOARD_H
#define SEKHMET_BOARD_H

#include <list>
#include <vector>
#include "../core/defs.h"
#include "Move.h"
#include "../transpositions/zKey.h"

#include <unordered_map>



class Board {

public:
    Board();
    //Board(const Board &b);
    explicit Board(const char fen[]);

    void printBoard(bool showMoves = false, bool flipped = false) const;

    bool operator==(const Board &b) const;

    U64 bitboards[2][6] = {{0}};
    U64 positionHistory[50] = {0};

    void nullMove();
    void makeMove(Move m);
    void tempMove(Move m);
    void unmakeMove(Move m);


    bool isWhiteTurn = true;
    short castlingRights = 0b1111;
    U64 enPassant = 0;
    unsigned int halfMoveClock = 0;
    unsigned int fullMoveNumber = 1;

    U64 occupancy[2] = {0};

    unsigned long checkSources = 0;

    void generateMoves();
    void generateCaptureMoves();
    void generateLegalMoves();
    //std::vector<Move> getLegalMoves() const;
    //std::vector<Move> getCaptureMoves() const;

    PieceType getPieceOnSquare(int square, bool isWhite) const;

    bool isSquareAttacked(int square, bool isWhite) const;
    bool isInCheck(bool isWhite) const;

    bool whiteCanCastleKingside() const;
    bool whiteCanCastleQueenside() const;
    bool blackCanCastleKingside() const;
    bool blackCanCastleQueenside() const;

private:

    void updateCastlingRights(Move m);

    void addPiece(bool isWhite, PieceType piece, int square);
    void removePiece(bool isWhite, PieceType piece, int square);
    void movePiece(bool isWhite, PieceType piece, int from, int to);

    //std::vector<Move> validMoves;
    //std::vector<Move> captureMoves;
    //std::vector<Move> legalMoves;

    zKey zobristKey;
public:
    zKey getZobristKey() const;

    void setZobristKey(zKey zobristKey);

private:

    const static std::unordered_map<PieceType, int> pieceToBoardMap;

    const static std::unordered_map<char, int> charToBoardMap;

    constexpr static char boardToPieceMap[12] = {'P', 'N', 'B', 'R', 'Q', 'K', 'p', 'n', 'b', 'r', 'q', 'k'};

    const static std::unordered_map<char, short> rightsMap;
};


#endif //SEKHMET_BOARD_H
