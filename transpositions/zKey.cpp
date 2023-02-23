//
// Created by Tao Groves on 2/10/23.
//

#include "zKey.h"
#include <iostream>
#include "../models/Board.h"

U64 zKey::pieceKeys[2][6][64];
U64 zKey::enPassantKeys[8];
U64 zKey::KSCastlingKeys[2];
U64 zKey::QSCastlingKeys[2];
U64 zKey::whiteTurnKey;


zKey::xorshift64_state zKey::rng_state;

// function to generate a random 64-bit integer
uint64_t zKey::xorshift64(xorshift64_state *state)
{
    uint64_t x = state->a;
    x ^= x << 13;
    x ^= x >> 7;
    x ^= x << 17;
    return state->a = x;
}

zKey::zKey() {
    // initialize a new blank zobrist key
    key = 0;
    castlingRights = 0;
    enPassantFile = -1;
}

zKey::zKey(const Board &b) {
    key = 0;

    // add turn key
    if(b.isWhiteTurn) {
        key ^= whiteTurnKey;
    }

    PieceType pieces[6] = {PAWN, KNIGHT, BISHOP, ROOK, QUEEN, KING};

    // add piece keys
    for (auto & piece : pieces) {
        for (int i = 0; i < 64; i++) {
            U64 square = ONE << i;
            if (b.bitboards[0][piece] & square) {
                key ^= pieceKeys[0][piece][i];
            } else if (b.bitboards[1][piece] & square) {
                key ^= pieceKeys[1][piece][i];
            }
        }
    }

    // add en passant key
    if(b.enPassant != 0) {
        enPassantFile = short(b.enPassant % 8);
        key ^= enPassantKeys[enPassantFile];
    } else {
        enPassantFile = -1;
    }

    // add castling keys
    castlingRights = b.castlingRights;
    if(b.castlingRights & 1) {
        key ^= KSCastlingKeys[0];
    }
    if(b.castlingRights & 2) {
        key ^= QSCastlingKeys[0];
    }
    if(b.castlingRights & 4) {
        key ^= KSCastlingKeys[1];
    }
    if(b.castlingRights & 8) {
        key ^= QSCastlingKeys[1];
    }
}

void zKey::init() {
    // initialize xorshit state
    rng_state = {rngSeed};

    // initialize all zobrist keys
    for(auto & pieceKey : pieceKeys) {
        for(auto & j : pieceKey) {
            for(unsigned long long & k : j) {
                k = xorshift64(&rng_state);
            }
        }
    }
    for(unsigned long long & enPassantKey : enPassantKeys) {
        enPassantKey = xorshift64(&rng_state);
    }
    for(unsigned long long & KSCastlingKey : KSCastlingKeys) {
        KSCastlingKey = xorshift64(&rng_state);
    }
    for(unsigned long long & QSCastlingKey : QSCastlingKeys) {
        QSCastlingKey = xorshift64(&rng_state);
    }
    whiteTurnKey = xorshift64(&rng_state);
}

U64 zKey::getValue() const {
    return key;
}

void zKey::movePiece(bool isWhite, PieceType piece, int from, int to) {
    flipPiece(isWhite, piece, from);
    flipPiece(isWhite, piece, to);
}

void zKey::flipPiece(bool isWhite, PieceType piece, int square) {
    key ^= pieceKeys[isWhite][piece][square];
}

void zKey::flipTurn() {
    key ^= whiteTurnKey;
}

void zKey::setEnPassant(short file) {
    enPassantFile = file;
    key ^= enPassantKeys[enPassantFile];
}

void zKey::clearEnPassant() {
    if (enPassantFile != -1) {
        key ^= enPassantKeys[enPassantFile];
        enPassantFile = -1;
    }
}

void zKey::updateCastlingRights(short rights) {
    if (castlingRights != rights) {
        if (castlingRights & 1) {
            flipKS(true);
        }
        if (castlingRights & 2) {
            flipQS(true);
        }
        if (castlingRights & 4) {
            flipKS(false);
        }
        if (castlingRights & 8) {
            flipQS(false);
        }
        castlingRights = rights;
    }
}

bool zKey::operator==(const zKey &other) const {
    return false;
}

void zKey::flipKS(bool isWhite) {
    key ^= KSCastlingKeys[isWhite];
}

void zKey::flipQS(bool isWhite) {
    key ^= QSCastlingKeys[isWhite];
}
