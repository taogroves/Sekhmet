//
// Created by Tao Groves on 2/24/23.
//

#include "ParallelSearch.h"

#include "../core/MoveGen.h"
#include "MovePicker.h"
#include <iostream>
#include <thread>

ParallelSearcher::ParallelSearcher() {

    // initialize the search threads
    /*for (int i = 0; i < 2; i++) {
        threads[i] = SearchThread(&tTable, Board(), i, 0, -1000000, 1000000, &startTime, &stopSearch, &timeCheckRequestCount);
    }*/
}

Move ParallelSearcher::restrictedSearch(const Board &b, const searchRestrictions &restrictions) {
    if (restrictions.infinite) {
        timeLimit = 999999999;
        //return depthSearch(b, 100);
    } else if (restrictions.depth > 0) {
        timeLimit = 0;
        auto start = std::chrono::high_resolution_clock::now();
        //Move m = depthSearch(b, restrictions.depth);
        auto end = std::chrono::high_resolution_clock::now();
        std::cout << "Time taken: " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << "ms | ";
        std::cout << "Transpositions: " << tTable.size() << std::endl;
        //return m;
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
        return timedSearch(b, 10000);
    }
}

Move ParallelSearcher::timedSearch(const Board &b, int time) {

    // if we only have one legal move, return it
    MoveList moves = MoveGen::getLegalMoves(b);
    if (moves.size() == 1) {
        std::cout << "Move " << moves[0].getNotation() << " is forced." << std::endl;
        return moves[0];
    }

    startTime = std::chrono::high_resolution_clock::now();
    timeLimit = time;
    Move bestMove = Move();
    int depth = 6;
    int alpha = -1000000;
    int beta = 1000000;
    int score = 0;

    // full window search at depth 1
    // start one thread with a full window search
    SearchThread initialThread(&tTable, std::ref(b), 1, 1, -1000000, 1000000, &startTime, timeLimit, &stopSearch, &timeCheckRequestCount);
    std::thread t1(&SearchThread::run, &initialThread);

    // wait for the thread to finish
    t1.join();

    // get the score and best move
    score = initialThread.getBestScore();
    bestMove = initialThread.getBestMove();

    int numthreads = 4;
    stopSearch = false;

    // break if we've found a checkmate
    if (score > 900000 || score < -900000) {
        goto output;
    }

    // establish bounds for all threads
    alpha = score - 25;
    beta = score + 25;
    while (depth < 13) {

        // start 4 threads with a reduced window search
        for (int i = 0; i < numthreads; i++) {
            threads[i] = SearchThread(&tTable, b, i, depth, alpha, beta, &startTime, timeLimit, &stopSearch, &timeCheckRequestCount);

        }
        std::thread temp[numthreads];
        // wait for all threads to finish
        for (int i = 0; i < numthreads; i++) {
            temp[i] = std::thread(&SearchThread::run, &threads[i]);
        }
        for (auto & i : temp) {
            i.join();
        }

        /*SearchThread t(&tTable, b, 0, depth, alpha, beta);
        std::thread thr(&SearchThread::run, &t);
        thr.join();
        int bestScore = t.getBestScore();
        Move found = t.getBestMove();*/

        // get the best score and best move
        int bestScore = -1000000;
        Move found = Move();
        for (int i = 0; i < numthreads; i++) {
            std::cout << "Thread " << i << " score: " << threads[i].getBestScore() << std::endl;
            if (threads[i].getBestScore() > bestScore) {
                bestScore = threads[i].getBestScore();
                found = threads[i].getBestMove();
            }
        }

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

        std::cout << "Depth: " << depth << " | " << "Score: " << score << " | " << "Time: " << elapsed << "ms" << std::endl;
        depth++;
    }
    output: std::cout << std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - startTime).count() << "ms | ";
    std::cout << "Depth: " << depth << (stopSearch ? "*" : "") << " | " << "Transpositions: " << tTable.size() << " | ";
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

void ParallelSearcher::printPV(const Board &b, int depth) {
    Board copy = b;
    std::cout << "PV: ";
    for (int i = 0; i < depth; i++) {
        TranspTableEntry currEntry = tTable.lookup(copy.getZobristKey());
        if (currEntry.getDepth() == 0 || currEntry.getBestMove().getFlags() == Move::NULL_MOVE) {
            break;
        }
        std::cout << currEntry.getBestMove().getNotation() << " ";
        copy.makeMove(currEntry.getBestMove());
    }
    std::cout << std::endl;
}


void ParallelSearcher::setTimeLimit(int t) {
    timeLimit = t;
}

void ParallelSearcher::reset(bool continueGame) {
    stopSearch = false;
    timeCheckRequestCount = 0;
    tTable.clear();
}
