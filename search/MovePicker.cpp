//
// Created by Tao Groves on 2/17/23.
//

#include "MovePicker.h"
#include <vector>

MovePicker::MovePicker(MoveList *list) {
    _moves = list;
}

void MovePicker::init() {
    // TODO MVV/LVA table
}

SearchMovePicker::SearchMovePicker(MoveList *list, OrderingData *data, const Board *b)
    : MovePicker(list) {
    _data = data;
    _moves = list;
    _board = b;
    _index = 0;
    scoreMoves();
}

Move SearchMovePicker::getNext() {
    size_t bestIndex;
    int bestValue = -INF;

    for (size_t i = _index; i < _moves->size(); i++) {
        if (_moves->at(i).getValue() > bestValue) {
            bestValue = _moves->at(i).getValue();
            bestIndex = i;
        }
    }

    std::swap(_moves->at(_index), _moves->at(bestIndex));
    return _moves->at(_index++);
}

bool SearchMovePicker::hasNext() const {
    return _index < _moves->size();
}

void SearchMovePicker::scoreMoves() {

    /*const TranspTableEntry *entry = _data->getTtTable()->lookup(_board->getZobristKey());
    Move pvMove;
    if (entry) {
        pvMove = entry->getBestMove();
    }*/ // TODO add this back

    for (auto &move : *_moves) {
        /*if (move == pvMove) {
            move.setValue(INF);*/
        /*} else*/ if (move.getFlags() & Move::CAPTURE) {
            move.setValue(CAPTURE_BONUS + pieceValues[move.getCapturedPieceType()] - pieceValues[move.getPieceType()]);
        } else if (move.getFlags() & Move::PROMOTION) {
            move.setValue(PROMOTION_BONUS + pieceValues[move.getPromotionPieceType()]);
        } else if (move == _data->getKillerMove(_data->getPly(), 0)) {
            move.setValue(KILLER1_BONUS);
        } else if (move == _data->getKillerMove(_data->getPly(), 1)) {
            move.setValue(KILLER2_BONUS);
        } else {
            move.setValue(QUIET_BONUS + _data->getHistoryHeuristic(_board->isWhiteTurn, move.getFrom(), move.getTo()));
            //move.setValue(0);
        }
    }
}

QSearchMovePicker::QSearchMovePicker(MoveList *list) : MovePicker(list) {
    _index = 0;

    scoreMoves();
}

Move QSearchMovePicker::getNext() {
    size_t bestIndex;
    int bestValue = -INF;

    for (size_t i = _index; i < _moves->size(); i++) {
        if (_moves->at(i).getValue() > bestValue) {
            bestValue = _moves->at(i).getValue();
            bestIndex = i;
        }
    }

    std::swap(_moves->at(_index), _moves->at(bestIndex));
    return _moves->at(_index++);
}

bool QSearchMovePicker::hasNext() const {
    return _index < _moves->size();
}

void QSearchMovePicker::scoreMoves() {
    for (auto &move : *_moves) {
        if (move.getFlags() & Move::CAPTURE) {
            move.setValue(CAPTURE_BONUS + pieceValues[move.getCapturedPieceType()] - pieceValues[move.getPieceType()]);
        } else if (move.getFlags() & Move::PROMOTION) {
            move.setValue(PROMOTION_BONUS + pieceValues[move.getPromotionPieceType()]);
        }
    }
}
