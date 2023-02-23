//
// Created by Tao G on 2/7/2023.
//

#include "Move.h"

#include <iostream>
#include <algorithm>

const char Move::RANKS[] = {'1', '2', '3', '4', '5', '6', '7', '8'};
const char Move::FILES[] = {'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h'};

Move::Move() {
    _move = ((NULL_MOVE & 0x7F) << 21);
    _value = 0;
}

Move::Move(unsigned int from, unsigned int to, PieceType piece, unsigned int flags) {
    _move = ((flags & 0x7F) << 21) | ((to & 0x3F) << 15) | ((from & 0x3f) << 9) | (piece & 0x7);
    _value = 0;
}

PieceType Move::getPieceType() const {
    return static_cast<PieceType>(_move & 0x7);
}

PieceType Move::getCapturedPieceType() const {
    return static_cast<PieceType>((_move >> 6) & 0x7);
}

void Move::setCapturedPieceType(PieceType piece) {
    unsigned int mask = 0x7 << 6;
    _move = (_move & ~mask) | ((piece << 6) & mask);
}

PieceType Move::getPromotionPieceType() const {
    return static_cast<PieceType>((_move >> 3) & 0x7);
}

void Move::setPromotionPieceType(PieceType piece) {
    unsigned int mask = 0x7 << 3;
    _move = (_move & ~mask) | ((piece << 3) & mask);
}

int Move::getValue() const {
    return _value;
}

void Move::setValue(int value) {
    _value = value;
}

unsigned int Move::getFrom() const {
    return (_move >> 9) & 0x3F;
}

unsigned int Move::getTo() const {
    return (_move >> 15) & 0x3F;
}

unsigned int Move::getFlags() const {
    return (_move >> 21) & 0x7F;
}

void Move::setFlag(Flag flag) {
    _move |= (flag << 21);
}

bool Move::operator==(Move move) const {
    return _move == move._move && _value == move._value;
}

std::string Move::getNotation() const {

    if (getFlags() & NULL_MOVE) {
        return "0000";
    }

    int from = getFrom();
    int to = getTo();

    std::string notation = squareToNotation(from) + squareToNotation(to);

    if (getFlags() & PROMOTION) {
        switch (getPromotionPieceType()) {
            case QUEEN:
                notation += 'q';
                break;
            case ROOK:
                notation += 'r';
                break;
            case BISHOP:
                notation += 'b';
                break;
            case KNIGHT:
                notation += 'n';
                break;
            default:
                break;
        }
    }

    return notation;
}

std::string Move::squareToNotation(int square) {
    return std::string({FILES[square % 8], RANKS[(63 - square) / 8]});
}

unsigned int Move::notationToSquare(std::string notation) {
    return std::find(FILES, FILES + 8, notation[0]) - FILES + 8 * (std::find(RANKS, RANKS + 8, notation[1]) - RANKS);
}
