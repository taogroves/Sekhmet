//
// Created by Tao Groves on 2/24/23.
//

#include "SearchThread.h"
#include "../core/MoveGen.h"
#include "MovePicker.h"
#include "eval.h"

SearchThread::SearchThread(threadSafeTable *tTable, Board startingBoard, int threadID, int depth, int alpha, int beta,
                           std::chrono::time_point<std::chrono::high_resolution_clock> *startTime, int timeLimit,
                           std::atomic<bool> *stopSearch, std::atomic<int> *timeCheckCount)
        : startingBoard(startingBoard) {
    this->tTable = tTable;
    this->threadID = threadID;
    this->rootDepth = depth;
    this->lowerBound = alpha;
    this->upperBound = beta;

    bestMove = Move();
    bestScore = -1000000;
    orderingData = OrderingData();

    this->startTime = startTime;
    this->stopSearch = stopSearch;
    this->timeCheckCount = timeCheckCount;
    this->timeLimit = timeLimit;
}

void SearchThread::run() {
    // if the thread is even, search at depth + 1
    // if the thread is odd, search at depth
    int d = rootDepth;
    if (threadID % 2 == 1) {
        //d--;
    }
    if (threadID % 4 == 3) {
        //d--;
    }

    MoveList legal = MoveGen::getLegalMoves(startingBoard);

    // if there are no legal moves return
    if (legal.empty()) {
        return;
    }

    // order moves
    SearchMovePicker picker(&legal, &orderingData, &startingBoard);
    int score;
    bool fullWindow = true;
    while (picker.hasNext()) {
        Move m = picker.getNext();
        Board b = startingBoard;
        b.makeMove(m);

        orderingData.incrementPly();
        if (fullWindow) {
            score = -search(b, d - 1, -upperBound, -lowerBound, true);
        } else {
            score = -search(b, d - 1, -lowerBound - 1, -lowerBound, true);
            if (score > lowerBound) {
                score = -search(b, d - 1, -upperBound, -lowerBound, true);
            }
        }
        orderingData.decrementPly();

        // if the main thread has requested that the search be aborted, stop searching
        if (checkTime() || *stopSearch) {
            *stopSearch = true;
            break;
        }

        if (score > lowerBound) {
            fullWindow = false;
            lowerBound = score;
            bestMove = m;
            bestScore = score;
        }

        if (lowerBound >= upperBound) {
            break;
        }
    }

    if (!*stopSearch) {
        // if no good move was found, pick the first one
        if (bestMove.getFlags() & Move::NULL_MOVE) {
            bestMove = legal.at(0);
        }

        // add the best move to the transposition table
        TranspTableEntry entry(lowerBound, upperBound, TranspTableEntry::EXACT, bestMove);
        tTable->store(startingBoard.getZobristKey(), entry);
    }
}

Move SearchThread::getBestMove() {
    return bestMove;
}

int SearchThread::getBestScore() const {
    return bestScore;
}

int SearchThread::search(const Board &b, int depth, int alpha, int beta, bool verify) {

    // if thread has been requested to stop, return
    if (checkTime() || *stopSearch) {
        *stopSearch = true;
        return 0;
    }

    // if there have been at least 4 irreversible moves, check for repetition
    if (b.halfMoveClock >= 4) {
        for (int i = 0; i < b.halfMoveClock; i++) { // TODO only need to look at every other move
            if (b.positionHistory[i] == b.getZobristKey().getValue()) {
                return 0; // TODO if there is only one occurence, don't return 0 unless curr_ply > root_ply + 2
            }
        }
    }

    // check for 50 move rule
    if (b.halfMoveClock >= 50) {
        return 0;
    }

    int startingAlpha = alpha;
    const TranspTableEntry ttEntry = tTable->lookup(b.getZobristKey());
    // if we have a transposition table entry, use it
    if ((ttEntry.getDepth() >= depth)) {

        switch (ttEntry.getFlag()) {
            case TranspTableEntry::EXACT:
                return ttEntry.getScore();
            case TranspTableEntry::LOWERBOUND:
                alpha = std::max(alpha, ttEntry.getScore());
                break;
            case TranspTableEntry::UPPERBOUND:
                beta = std::min(beta, ttEntry.getScore());
                break;
        }
        if (alpha >= beta) {
            return ttEntry.getScore();
        }
    }

    MoveList legalMoves = MoveGen::getLegalMoves(b);

    // check for checkmate or stalemate
    if (legalMoves.empty()) {
        //return -INF;
        //std::cout << "Mate in " << 40 - depth << " ply " << std::endl;
        return b.isInCheck(b.isWhiteTurn) ? -1000000 + int(b.fullMoveNumber) : 0;
    }

    // extend search when evading check
    int extension = 0;
    if (b.isInCheck(b.isWhiteTurn)) {
        extension = 1;
    }

    // if we are at the end of the search, evaluate the position
    if (depth + extension <= 0) {
        return eval::qSearch(b, alpha, beta);
    }

    // null-move pruning
    bool fail_high = false;
    // only ok if we are not in check, TODO any more conditions?
    if (!b.isInCheck(b.isWhiteTurn) && (!verify || depth > 1)) {
        Board copy = b;
        copy.nullMove();
        int score = -search(copy, depth - 1 - NULL_MOVE_REDUCTION, -beta, -beta + 1, verify);
        if (score >= beta) {
            if (verify) {
                depth--;
                verify = false; // disable verification for the subtree
                fail_high = true;
            } else {
                return score;
            }
        }
    }

    re_search: // loop label for re-searching after a fail high

    // order moves
    SearchMovePicker picker(&legalMoves, &orderingData, const_cast<Board *>(&b));

    //Move bestMove;
    bool fullWindow = true;
    int movesSearched = 0;
    while(picker.hasNext()) {
        Move move = picker.getNext();

        Board copy = b;
        copy.makeMove(move);
        int score;
        orderingData.incrementPly();
        if (movesSearched >= FULL_DEPTH_MOVES && depth >= REDUCTION_LIMIT && okToReduce(copy, move)) {
            score = -search(copy, depth - 2, -(alpha + 1), -alpha, verify);
            if (score > alpha) {
                score = -search(copy, depth - 1, -beta, -alpha, verify);
            }
        }
        else {
            if (fullWindow) {
                score = -search(copy, depth - 1 + extension, -beta, -alpha, verify);
            } else {
                score = -search(copy, depth - 1 + extension, -alpha - 1, -alpha, verify);
                if (score > alpha) {
                    score = -search(copy, depth - 1 + extension, -beta, -alpha, verify);
                }
            }
        }
        orderingData.decrementPly();

        // beta cutoff
        if (score >= beta) {
            orderingData.updateKillerMoves(move);
            if (!(move.getFlags() & Move::CAPTURE)) {
                orderingData.incrementHistory(b.isWhiteTurn, move.getFrom(), move.getTo(), depth);
            }

            TranspTableEntry newEntry(score, depth, TranspTableEntry::LOWERBOUND, move);
            tTable->store(b.getZobristKey(), newEntry);
            return beta;
        }

        // alpha cutoff
        if (score > alpha) {
            alpha = score;
            bestMove = move;
            fullWindow = false;
        }

        movesSearched++;
    }

    // zungzwang detection
    if (fail_high && (alpha < beta)) {
        depth++;
        fail_high = false;
        verify = true;
        //std::cout << "Zungzwang detected, re-searching" << std::endl;
        goto re_search;
    }

    if (bestMove == Move()) {
        bestMove = legalMoves[0];
    }

    // store the best move in the transposition table
    TranspTableEntry::Flag flag;
    if (alpha <= startingAlpha) { // if we never raised alpha, we have an upper bound
        flag = TranspTableEntry::UPPERBOUND;
    } else { // otherwise we have an exact score
        flag = TranspTableEntry::EXACT;
    }
    tTable->store(b.getZobristKey(), TranspTableEntry(alpha, depth, flag, bestMove));

    return alpha;
}

bool SearchThread::okToReduce(const Board &b, Move m) {
    if (m.getFlags() & Move::CAPTURE) {
        return false;
    }
    if (m.getFlags() & Move::PROMOTION) {
        return false;
    }
    if (orderingData.getHistoryHeuristic(b.isWhiteTurn, m.getFrom(), m.getTo()) > 800) {
        return false;
    }
    if (b.isInCheck(b.isWhiteTurn)) {
        return false;
    }
    return true;
}

SearchThread::SearchThread() {
    tTable = nullptr;
    orderingData = nullptr;
}

bool SearchThread::checkTime() {
    if (*timeCheckCount++ < TIME_CHECK_INTERVAL || timeLimit == 0) {
        return false;
    }
    *timeCheckCount = 0;

    if (*stopSearch) {
        return true;
    }

    auto now = std::chrono::high_resolution_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - *startTime).count();
    return elapsed >= timeLimit;
}
