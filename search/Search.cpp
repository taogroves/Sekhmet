//
// Created by Tao G on 2/12/2023.
//

#include "Search.h"
#include "../core/MoveGen.h"
#include "MovePicker.h"
#include "eval.h"
#include <iostream>

Searcher::Searcher() :
    orderingData(OrderingData(&zobristTable)) {
    zobristTable = zTable();
}

Move Searcher::restrictedSearch(const Board &b, const searchRestrictions &restrictions) {
    if (restrictions.infinite) {
        timeLimit = 999999999;
        return depthSearch(b, 100);
    } else if (restrictions.depth > 0) {
        timeLimit = 0;
        auto start = std::chrono::high_resolution_clock::now();
        Move m = depthSearch(b, restrictions.depth);
        auto end = std::chrono::high_resolution_clock::now();
        std::cout << "Time taken: " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << "ms | ";
        std::cout << "Transpositions: " << zobristTable.size() << std::endl;
        return m;
    } else if (restrictions.movetime > 0) {
        return timedSearch(b, restrictions.movetime);
    }
    else if (restrictions.time[b.isWhiteTurn] != 0) {
        int timeAllocated = 0;
        int ourTime = restrictions.time[b.isWhiteTurn];
        int theirTime = restrictions.time[!b.isWhiteTurn];

        // divide up remaining time
        if (restrictions.movesToGo == 0) {

            // if opponent has more time, allocate less time
            double timeRatio = std::max((double) (ourTime / theirTime), 1.0);
            int movesToGo = (int) (40 * std::min(2.0, timeRatio));
            timeAllocated = ourTime / movesToGo;
        } else {
            timeAllocated = ourTime / (restrictions.movesToGo + 3);
        }

        // use all the increment if we have it
        timeAllocated += restrictions.increment[b.isWhiteTurn];

        std::cout << "Time allocated: " << timeAllocated << "ms" << std::endl;

        return timedSearch(b, timeAllocated);

    } else {
        timeLimit = 10000;
        return depthSearch(b, 10);
    }
}

Move Searcher::timedSearch(const Board &b, int time) {

    // if we only have one legal move, return it
    MoveList moves = MoveGen::getLegalMoves(b);
    if (moves.size() == 1) {
        std::cout << "Move " << moves[0].getNotation() << " is forced." << std::endl;
        return moves[0];
    }

    startTime = std::chrono::high_resolution_clock::now();
    timeLimit = time;
    Move bestMove = Move();
    int depth = 1;
    int alpha = -1000000;
    int beta = 1000000;
    int score = 0;
    while (true) {
        //orderingData.clear();
        Move found = depthSearch(b, depth, alpha, beta, &score);

        //bestMove = depthSearch(b, depth);
        int elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - startTime).count();

        if (stopSearch) { // limits were exceeded in the last search
            break; // we don't know if the last search was a fail high or fail low, so we return the best move we have
        }

        bestMove = found;

        // if at least 50% of the time has elapsed, return
        if (elapsed > time * 0.5) {
            break;
        }

        // break if we've found a checkmate
        if (score > 900000 || score < -900000) {
            goto output;
        }

        if (score <= alpha) {
            alpha = -(alpha * alpha);
            std::cout << "Fail low" << std::endl;
            continue;
        } else if (score >= beta) {
            beta = beta * beta;
            std::cout << "Fail high" << std::endl;
            continue;
        } else {
            alpha = score - 100;
            beta = score + 100;
        }

        depth++;
    }
    output: std::cout << std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - startTime).count() << "ms | ";
    std::cout << "Depth: " << depth << (stopSearch ? "*" : "") << " | " << "Transpositions: " << zobristTable.size() << " | ";
    std::string evalPrint;
    if (!b.isWhiteTurn) {
        score *= -1;
    }
    if (abs(score) >= 9999)
        evalPrint = "Mate in " + std::to_string((1000000 - abs(score) - b.fullMoveNumber)) + " ply";
    else {
        // print the evaluation in pawns, round to 2 decimal places
        evalPrint = std::to_string(((float) score / 100));
        evalPrint = evalPrint.substr(0, evalPrint.find('.') + 3);
    }
    std::cout << "Evaluation: " << evalPrint << std::endl;

    printPV(b, depth);

    stopSearch = false;
    return bestMove;
}

void Searcher::printPV(const Board &b, int depth) {
    const TranspTableEntry *currEntry;
    Board copy = b;
    std::cout << "PV: ";
    for (int i = 0; i < depth; i++) {
        currEntry = zobristTable.lookup(copy.getZobristKey());
        if (currEntry == nullptr || currEntry->getBestMove().getFlags() == Move::NULL_MOVE) {
            break;
        }
        std::cout << currEntry->getBestMove().getNotation() << " ";
        copy.makeMove(currEntry->getBestMove());
    }
    std::cout << std::endl;
}

Move Searcher::depthSearch(Board b, int depth, int alpha, int beta, int *extern_score) {

    Move bestMove = Move();
    MoveList legal = MoveGen::getLegalMoves(b);

    // if there are no legal moves, return
    if (legal.empty()) {
        return bestMove;
    }

    // order moves
    SearchMovePicker picker(&legal, &orderingData, const_cast<Board *>(&b));
    int score;
    bool fullWindow = true;
    while(picker.hasNext()) {
        Move move = picker.getNext();

        Board copy = b;
        copy.makeMove(move);

        orderingData.incrementPly();
        if (fullWindow) {
            //score = -zobristNMax(copy, depth - 1, -beta, -alpha);
            score = -rootSearch(copy, depth - 1, -beta, -alpha);
        } else {
            //score = -zobristNMax(copy, depth - 1, -alpha - 1, -alpha);
            score = -rootSearch(copy, depth - 1, -alpha - 1, -alpha);
            if (score > alpha) {
                //score = -zobristNMax(copy, depth - 1, -beta, -score);
                score = -rootSearch(copy, depth - 1, -beta, -alpha);
            }
        }
        orderingData.decrementPly();

        //std::cout << "Move: " << move.getNotation() << " Score: " << score << std::endl;

        // if time limit was exceeded, search was incomplete, so we can't trust this move
        if (stopSearch || checkTime()) {
            stopSearch = true;
            //if (score > alpha) std::cout << "Best score: " << score << std::endl;
            break;
        }

        if (score > alpha) {
            fullWindow = false;
            alpha = score;
            /*if (depth == 5) {
                std::cout << "New best move: " << move.getNotation() << " Score: " << score << std::endl;
            }*/
            bestMove = move;


        } /*else {
            std::cout << "Move: " << move.getNotation() << " was not chosen. Score: " << score << std::endl;
        }*/

        if (alpha >= beta) {
            break;
        }
    }

    if (!stopSearch) {
        // if we never found a good move, pick an arbitrary one so we don't add null to the transposition table
        if (bestMove.getFlags() & Move::NULL_MOVE) {
            bestMove = legal.at(0);
        }

        // add the best move to the transposition table
        TranspTableEntry e(alpha, depth, TranspTableEntry::EXACT, bestMove);
        zobristTable.store(b.getZobristKey(), e);
    }

    if (extern_score != nullptr) {
        *extern_score = alpha;
    }
    return bestMove;
}

void Searcher::setAlgorithm(Searcher::Algorithm a) {
    algorithm = a;
}

void Searcher::setEvaluation(Searcher::Evaluation e) {
    evaluation = e;
}

int Searcher::negamax(Board b, int depth, int alpha, int beta) {

    // check search limits
    if (stopSearch || checkTime()) {
        stopSearch = true;
        return 0;
    }

    if (depth == 0) {
        switch (evaluation) {
            case MATERIAL:
                return eval::evaluateMaterial(b.isWhiteTurn, b);
            case QUIESCENT_MATERIAL:
                return eval::searchAllCaptures(b.isWhiteTurn, b, -INF, INF);
            case PST:
                return eval::evaluateBoard(b.isWhiteTurn, b);
            case QUIESCENT_PST: ;
                return eval::qSearch(b, alpha, beta);;
        }
    }

    MoveList legal = MoveGen::getLegalMoves(b);

    // order moves
    //eval::orderMoves(b, &legal);

    // if there are no legal moves, we are in checkmate or stalemate
    if (legal.empty()) {
        if (b.isInCheck(b.isWhiteTurn)) {
            return -1000000 + int(b.fullMoveNumber);
        } else {
            return 0;
        }
    }

    // if there have been at least 4 irreversible moves, check for repetition
    if (b.halfMoveClock >= 4) {
        for (int i = b.isWhiteTurn ? 0 : 1; i < b.halfMoveClock; i+=2) {
            if (b.positionHistory[i] == b.getZobristKey().getValue()) {
                return 0; // TODO if there is only one occurence, don't return 0 unless curr_ply > root_ply + 2
            }
        }
    }

    int bestScore = -INF;
    for (Move move : legal) {
        Board copy = b;
        copy.makeMove(move);
        bestScore = std::max(bestScore, -negamax(copy, depth - 1, -beta, -alpha));
        if (bestScore > alpha) {
            alpha = bestScore;
        }

        if (alpha >= beta) {
            return beta;
        }
    }

    return bestScore;
}

int Searcher::zobristNMax(const Board &b, int depth, int alpha, int beta) {

    // check search limits
    if (stopSearch || checkTime()) {
        stopSearch = true;
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
    const TranspTableEntry *ttEntry = zobristTable.lookup(b.getZobristKey());
    // if we have a transposition table entry, use it
    if (ttEntry && (ttEntry->getDepth() >= depth)) {

        switch (ttEntry->getFlag()) {
            case TranspTableEntry::EXACT:
                return ttEntry->getScore();
            case TranspTableEntry::LOWERBOUND:
                alpha = std::max(alpha, ttEntry->getScore());
                break;
            case TranspTableEntry::UPPERBOUND:
                beta = std::min(beta, ttEntry->getScore());
                break;
        }
        if (alpha >= beta) {
            return ttEntry->getScore();
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
    if (depth + extension == 0) {
        switch (evaluation) {
            case MATERIAL:
                return eval::evaluateMaterial(b.isWhiteTurn, b);
            case QUIESCENT_MATERIAL:
                return eval::searchAllCaptures(b.isWhiteTurn, b, alpha, beta);
            case PST:
                //return eval::pst(b);
            case QUIESCENT_PST: ;
                return eval::qSearch(b, alpha, beta);
        }
    }

    // order moves
    SearchMovePicker picker(&legalMoves, &orderingData, const_cast<Board *>(&b));

    Move bestMove;
    bool fullWindow = true;
    while(picker.hasNext()) {
        Move move = picker.getNext();

        Board copy = b;
        copy.makeMove(move);
        int score;
        orderingData.incrementPly();
        if (fullWindow) {
            score = -zobristNMax(copy, depth - 1 + extension, -beta, -alpha);
        } else {
            score = -zobristNMax(copy, depth - 1 + extension, -alpha - 1, -alpha);
            if (score > alpha) {
                score = -zobristNMax(copy, depth - 1 + extension, -beta, -alpha);
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
            zobristTable.store(b.getZobristKey(), newEntry);
            return beta;
        }

        // alpha cutoff
        if (score > alpha) {
            alpha = score;
            bestMove = move;
            fullWindow = false;
        }
    }

    // if we never found a good move, just pick the first one
    if (bestMove.getFlags() & Move::NULL_MOVE) {
        bestMove = legalMoves.at(0);
    }

    // store the best move in the transposition table
    TranspTableEntry::Flag flag;
    if (alpha <= startingAlpha) { // if we never raised alpha, we have an upper bound
        flag = TranspTableEntry::UPPERBOUND;
    } else { // otherwise we have an exact score
        flag = TranspTableEntry::EXACT;
    }
    zobristTable.store(b.getZobristKey(), TranspTableEntry(alpha, depth, flag, bestMove));

    return alpha;
}

int Searcher::nullMovePVS(const Board &b, int depth, int alpha, int beta, bool verify) {
    // check search limits
    if (stopSearch || checkTime()) {
        stopSearch = true;
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
    const TranspTableEntry *ttEntry = zobristTable.lookup(b.getZobristKey());
    // if we have a transposition table entry, use it
    if (ttEntry && (ttEntry->getDepth() >= depth)) {

        switch (ttEntry->getFlag()) {
            case TranspTableEntry::EXACT:
                return ttEntry->getScore();
            case TranspTableEntry::LOWERBOUND:
                alpha = std::max(alpha, ttEntry->getScore());
                break;
            case TranspTableEntry::UPPERBOUND:
                beta = std::min(beta, ttEntry->getScore());
                break;
        }
        if (alpha >= beta) {
            return ttEntry->getScore();
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
        switch (evaluation) {
            case MATERIAL:
                return eval::evaluateMaterial(b.isWhiteTurn, b);
            case QUIESCENT_MATERIAL:
                return eval::searchAllCaptures(b.isWhiteTurn, b, alpha, beta);
            case PST:
                //return eval::pst(b);
            case QUIESCENT_PST: ;
                return eval::qSearch(b, alpha, beta);
        }
    }

    // null-move pruning
    bool fail_high = false;
    // only ok if we are not in check, TODO any more conditions?
    if (!b.isInCheck(b.isWhiteTurn) && (!verify || depth > 1)) {
        Board copy = b;
        copy.nullMove();
        int score = -nullMovePVS(copy, depth - 1 - NULL_MOVE_REDUCTION, -beta, -beta + 1, verify);
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

    Move bestMove;
    bool fullWindow = true;
    while(picker.hasNext()) {
        Move move = picker.getNext();

        Board copy = b;
        copy.makeMove(move);
        int score;
        orderingData.incrementPly();
        if (fullWindow) {
            score = -nullMovePVS(copy, depth - 1 + extension, -beta, -alpha, verify);
        } else {
            score = -nullMovePVS(copy, depth - 1 + extension, -alpha - 1, -alpha, verify);
            if (score > alpha) {
                score = -nullMovePVS(copy, depth - 1 + extension, -beta, -alpha, verify);
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
            zobristTable.store(b.getZobristKey(), newEntry);
            return beta;
        }

        // alpha cutoff
        if (score > alpha) {
            alpha = score;
            bestMove = move;
            fullWindow = false;
        }
    }

    // zungzwang detection
    if (fail_high && alpha < beta) {
        depth++;
        fail_high = false;
        verify = true;
        //std::cout << "Zungzwang detected, re-searching" << std::endl;
        goto re_search;
    }

    // store the best move in the transposition table
    TranspTableEntry::Flag flag;
    if (alpha <= startingAlpha) { // if we never raised alpha, we have an upper bound
        flag = TranspTableEntry::UPPERBOUND;
    } else { // otherwise we have an exact score
        flag = TranspTableEntry::EXACT;
    }
    zobristTable.store(b.getZobristKey(), TranspTableEntry(alpha, depth, flag, bestMove));

    return alpha;
}

int Searcher::lateMovePVS(const Board &b, int depth, int alpha, int beta, bool verify) {
    // check search limits
    if (stopSearch || checkTime()) {
        stopSearch = true;
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
    const TranspTableEntry *ttEntry = zobristTable.lookup(b.getZobristKey());
    // if we have a transposition table entry, use it
    if (ttEntry && (ttEntry->getDepth() >= depth)) {

        switch (ttEntry->getFlag()) {
            case TranspTableEntry::EXACT:
                return ttEntry->getScore();
            case TranspTableEntry::LOWERBOUND:
                alpha = std::max(alpha, ttEntry->getScore());
                break;
            case TranspTableEntry::UPPERBOUND:
                beta = std::min(beta, ttEntry->getScore());
                break;
        }
        if (alpha >= beta) {
            return ttEntry->getScore();
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
        switch (evaluation) {
            case MATERIAL:
                return eval::evaluateMaterial(b.isWhiteTurn, b);
            case QUIESCENT_MATERIAL:
                return eval::searchAllCaptures(b.isWhiteTurn, b, alpha, beta);
            case PST:
                //return eval::pst(b);
            case QUIESCENT_PST: ;
                return eval::qSearch(b, alpha, beta);
        }
    }

    // null-move pruning
    bool fail_high = false;
    // only ok if we are not in check, TODO any more conditions?
    if (!b.isInCheck(b.isWhiteTurn) && (!verify || depth > 1)) {
        Board copy = b;
        copy.nullMove();
        int score = -lateMovePVS(copy, depth - 1 - NULL_MOVE_REDUCTION, -beta, -beta + 1, verify);
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

    Move bestMove;
    bool fullWindow = true;
    int movesSearched = 0;
    while(picker.hasNext()) {
        Move move = picker.getNext();

        Board copy = b;
        copy.makeMove(move);
        int score;
        orderingData.incrementPly();
        if (movesSearched >= FULL_DEPTH_MOVES && depth >= REDUCTION_LIMIT && okToReduce(copy, move)) {
            score = -lateMovePVS(copy, depth - 2, -(alpha + 1), -alpha, verify);
            if (score > alpha) {
                score = -lateMovePVS(copy, depth - 1, -beta, -alpha, verify);
            }
        }
        else {
            if (fullWindow) {
                score = -lateMovePVS(copy, depth - 1 + extension, -beta, -alpha, verify);
            } else {
                score = -lateMovePVS(copy, depth - 1 + extension, -alpha - 1, -alpha, verify);
                if (score > alpha) {
                    score = -lateMovePVS(copy, depth - 1 + extension, -beta, -alpha, verify);
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
            zobristTable.store(b.getZobristKey(), newEntry);
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
    if (fail_high && alpha < beta) {
        depth++;
        fail_high = false;
        verify = true;
        //std::cout << "Zungzwang detected, re-searching" << std::endl;
        goto re_search;
    }

    // store the best move in the transposition table
    TranspTableEntry::Flag flag;
    if (alpha <= startingAlpha) { // if we never raised alpha, we have an upper bound
        flag = TranspTableEntry::UPPERBOUND;
    } else { // otherwise we have an exact score
        flag = TranspTableEntry::EXACT;
    }
    zobristTable.store(b.getZobristKey(), TranspTableEntry(alpha, depth, flag, bestMove));

    return alpha;
}

bool Searcher::checkTime() {
    if (timeCheckRequestCount++ < TIME_CHECK_INTERVAL || timeLimit == 0) {
        return false;
    }
    timeCheckRequestCount = 0;

    if (stopSearch) {
        return true;
    }

    if (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - startTime).count() > timeLimit) {
        stopSearch = true;
        return true;
    }

    return false;
}

void Searcher::setTimeLimit(int t) {
    timeLimit = t;
}

void Searcher::reset(bool continueGame) {
    stopSearch = false;
    timeCheckRequestCount = 0;
    zobristTable.clear();
    if (continueGame) {
        orderingData.nextMove();
    } else {
        orderingData.clear();
    }
}

int Searcher::rootSearch(Board b, int depth, int alpha, int beta) {
    switch(algorithm) {
        case NEGAMAX:
            return negamax(b, depth, alpha, beta);
        case ZOBRIST:
            return zobristNMax(b, depth, alpha, beta);
        case NULLMOVE_PVS:
            return nullMovePVS(b, depth, alpha, beta, true);
        case NL_LM_PVS:
            return lateMovePVS(b, depth, alpha, beta, true);
        default:
            std::cout << "Invalid algorithm" << std::endl;
            return 0;
    }
}

bool Searcher::okToReduce(const Board &b, Move m) {
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
