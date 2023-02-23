//
// Created by Tao Groves on 2/10/23.
//

#ifndef SEKHMET_ZKEY_H
#define SEKHMET_ZKEY_H

#include "../core/defs.h"

class Board;

class zKey {

public:
    zKey();

    /**
     * @brief Generates a zobrist key for a given board
     * @param b The board to generate a key for
     * @return The zobrist key for the given board
     */
    explicit zKey(const Board &b);

    // initialize all zobrist keys
    static void init();

    U64 getValue() const;

    void movePiece(bool isWhite, PieceType piece, int from, int to);

    // if piece is set, unset it, otherwise set it
    void flipPiece(bool isWhite, PieceType piece, int square);

    void flipTurn();

    void setEnPassant(short file);
    void clearEnPassant();

    void updateCastlingRights(short rights);

    bool operator==(const zKey &other) const;


private:

    U64 key;

    // helper functions to flip castling rights
    void flipKS(bool isWhite);
    void flipQS(bool isWhite);

    short enPassantFile;
    short castlingRights;

    static U64 pieceKeys[2][6][64];
    static U64 enPassantKeys[8];
    static U64 KSCastlingKeys[2];
    static U64 QSCastlingKeys[2];
    static U64 whiteTurnKey;

    static const uint64_t rngSeed = 0x123456789abcdef0;
    struct xorshift64_state {
        uint64_t a;
    };
    static xorshift64_state rng_state;

    static uint64_t xorshift64(xorshift64_state *state);
};


#endif //SEKHMET_ZKEY_H
