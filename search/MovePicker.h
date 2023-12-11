//
// Created by Tao Groves on 2/17/23.
//

#ifndef SEKHMET_MOVEPICKER_H
#define SEKHMET_MOVEPICKER_H

#include "../models/Board.h"
#include "OrderingData.h"

typedef std::vector<Move> MoveList;

class MovePicker {
public:

    MovePicker(MoveList *list);

    virtual Move getNext() = 0;

    virtual bool hasNext() const = 0;

    static void init();

protected:

    MoveList *_moves;

    // bonuses for certain types of moves
    static const int CAPTURE_BONUS = 2500;
    static const int PROMOTION_BONUS = 2200;
    static const int KILLER1_BONUS = 2000;
    static const int KILLER2_BONUS = 1000;
    static const int QUIET_BONUS = 0;

    constexpr static short pieceValues[6] = {100, 280, 320, 479, 929, 0};
};

class SearchMovePicker : MovePicker {
public:

    SearchMovePicker(MoveList *list, OrderingData *data, const Board *b);

    Move getNext() override;

    bool hasNext() const override;

private:

    void scoreMoves();

    size_t _index;

    const OrderingData *_data;
    const Board *_board;
};

class ParallelSearchMovePicker : MovePicker {
public:

    ParallelSearchMovePicker(MoveList *list, OrderingData *data, const Board *b);

    Move getNext() override;

    bool hasNext() const override;

private:

    void scoreMoves();

    size_t _index;

    const OrderingData *_data;
    const Board *_board;
};

class QSearchMovePicker : MovePicker {
public:

    QSearchMovePicker(MoveList *list);

    Move getNext() override;

    bool hasNext() const override;

private:

        void scoreMoves();

        size_t _index;
};


#endif //SEKHMET_MOVEPICKER_H
