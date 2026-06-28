#include "ParallelSearcher.h"

#include <algorithm>
#include <chrono>
#include <iostream>
#include <string>
#include "../core/MoveGen.h"

ParallelSearcher::ParallelSearcher() : threadCount(defaultThreadCount()) {
}

unsigned int ParallelSearcher::defaultThreadCount() {
    unsigned int detected = std::thread::hardware_concurrency();
    if (detected == 0) {
        return 1;
    }
    return std::max(1U, detected / 2);
}

void ParallelSearcher::setThreadCount(unsigned int count) {
    threadCount = std::max(1U, count);
}

unsigned int ParallelSearcher::getThreadCount() const {
    return threadCount;
}

Move ParallelSearcher::getPonderMove(const Board &b, const Move &bestMove) const {
    if (bestMove.getFlags() & Move::NULL_MOVE) {
        return Move();
    }

    Board copy = b;
    copy.makeMove(bestMove);
    const TranspTableEntry *entry = sharedTable.lookup(copy.getZobristKey());
    if (entry == nullptr || entry->getBestMove().getFlags() & Move::NULL_MOVE) {
        return Move();
    }

    MoveList legal = MoveGen::getLegalMovesFast(copy);
    for (Move move : legal) {
        if (move.getPacked() == entry->getBestMove().getPacked()) {
            return move;
        }
    }

    return Move();
}

void ParallelSearcher::reset(bool continueGame) {
    stopSearch.store(false, std::memory_order_relaxed);
    if (!continueGame) {
        sharedTable.clear();
    }
}

void ParallelSearcher::stop() {
    stopSearch.store(true, std::memory_order_relaxed);
}

Move ParallelSearcher::restrictedSearch(const Board &b, const Searcher::searchRestrictions &restrictions) {
    if (threadCount <= 1) {
        Searcher single(&sharedTable, &stopSearch);
        single.setAlgorithm(Searcher::Algorithm::NL_LM_PVS);
        single.setEvaluation(Searcher::Evaluation::QUIESCENT_PST);
        return single.restrictedSearch(b, restrictions);
    }

    if (restrictions.infinite) {
        return depthSearch(b, 100);
    }
    if (restrictions.depth > 0) {
        return depthSearch(b, restrictions.depth);
    }
    if (restrictions.movetime > 0) {
        return timedSearch(b, restrictions.movetime);
    }
    if (restrictions.time[b.isWhiteTurn] != 0) {
        return timedSearch(b, allocateTime(b, restrictions));
    }
    return timedSearch(b, 10000);
}

int ParallelSearcher::allocateTime(const Board &b, const Searcher::searchRestrictions &restrictions) const {
    int ourTime = restrictions.time[b.isWhiteTurn];
    int theirTime = restrictions.time[!b.isWhiteTurn];
    int timeAllocated;

    if (restrictions.movesToGo == 0) {
        double timeRatio = theirTime == 0 ? 1.0 : std::max(double(ourTime) / double(theirTime), 1.0);
        int movesToGo = int(40 * std::min(2.0, timeRatio));
        timeAllocated = ourTime / movesToGo;
    } else {
        timeAllocated = ourTime / (restrictions.movesToGo + 3);
    }

    return timeAllocated + restrictions.increment[b.isWhiteTurn];
}

ParallelSearcher::WorkerResult ParallelSearcher::searchWorker(const Board &b, int maxDepth, int workerIndex) {
    Searcher worker(&sharedTable, &stopSearch);
    worker.setAlgorithm(Searcher::Algorithm::NL_LM_PVS);
    worker.setEvaluation(Searcher::Evaluation::QUIESCENT_PST);

    WorkerResult result;
    if (maxDepth <= 0) {
        return result;
    }

    // Keep worker 0 on the normal depth ladder and let helpers enter nearby
    // ladders so shared-table feedback is less lockstep across threads.
    int depth = workerIndex == 0 ? 1 : 2 + ((workerIndex - 1) % 3);
    depth = std::min(depth, maxDepth);
    while (!stopSearch.load(std::memory_order_relaxed) && depth <= maxDepth) {
        int score = 0;
        Move move = worker.depthSearch(b, depth, -1000000, 1000000, &score);
        if (!stopSearch.load(std::memory_order_relaxed) && !(move.getFlags() & Move::NULL_MOVE)) {
            result = {move, score, depth};
            if (abs(score) > 900000) {
                stopSearch.store(true, std::memory_order_relaxed);
                break;
            }
        }
        depth++;
    }
    return result;
}

bool ParallelSearcher::isValidResult(const WorkerResult &result) {
    return result.depth > 0 && !(result.move.getFlags() & Move::NULL_MOVE);
}

int ParallelSearcher::moveVoteKey(const Move &move) {
    return int(move.getFrom() * 64 + move.getTo());
}

long long ParallelSearcher::threadVoteValue(const WorkerResult &result, int worstScore) {
    return (static_cast<long long>(result.score) - worstScore + 1) * std::max(result.depth, 1);
}

std::vector<ParallelSearcher::WorkerResult>::const_iterator
ParallelSearcher::selectBestResult(const std::vector<WorkerResult> &results) const {
    constexpr int MATE_SCORE_BOUND = 900000;
    constexpr int MAX_MOVE_KEYS = 64 * 64;

    auto best = results.end();
    int worstScore = 1000000;
    long long voteMap[MAX_MOVE_KEYS] = {};

    for (auto result = results.begin(); result != results.end(); ++result) {
        if (!isValidResult(*result)) {
            continue;
        }

        worstScore = std::min(worstScore, result->score);
        if (best == results.end()) {
            best = result;
        }
    }

    if (best == results.end()) {
        return best;
    }

    for (const WorkerResult &result : results) {
        if (!isValidResult(result)) {
            continue;
        }

        voteMap[moveVoteKey(result.move)] += threadVoteValue(result, worstScore);
    }

    int bestScore = best->score;
    long long bestVoteScore = voteMap[moveVoteKey(best->move)];
    long long bestThreadValue = threadVoteValue(*best, worstScore);

    for (auto result = best + 1; result != results.end(); ++result) {
        if (!isValidResult(*result)) {
            continue;
        }

        int currScore = result->score;
        long long currVoteScore = voteMap[moveVoteKey(result->move)];
        long long currThreadValue = threadVoteValue(*result, worstScore);

        if (abs(bestScore) >= MATE_SCORE_BOUND || abs(currScore) >= MATE_SCORE_BOUND) {
            if (currScore > bestScore) {
                best = result;
                bestScore = currScore;
                bestVoteScore = currVoteScore;
                bestThreadValue = currThreadValue;
            }
        } else if (currScore > -MATE_SCORE_BOUND &&
                   (currVoteScore > bestVoteScore ||
                    (currVoteScore == bestVoteScore &&
                     (currThreadValue > bestThreadValue ||
                      (currThreadValue == bestThreadValue && result->depth > best->depth))))) {
            best = result;
            bestScore = currScore;
            bestVoteScore = currVoteScore;
            bestThreadValue = currThreadValue;
        }
    }

    return best;
}

Move ParallelSearcher::depthSearch(const Board &b, int depth) {
    stopSearch.store(false, std::memory_order_relaxed);

    std::vector<std::thread> threads;
    std::vector<WorkerResult> results(threadCount);
    threads.reserve(threadCount);

    auto start = std::chrono::high_resolution_clock::now();
    for (unsigned int i = 0; i < threadCount; i++) {
        threads.emplace_back([&, i] {
            results[i] = searchWorker(b, depth, int(i));
        });
    }

    for (std::thread &thread : threads) {
        thread.join();
    }

    auto best = selectBestResult(results);

    long elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::high_resolution_clock::now() - start).count();
    if (best != results.end()) {
        sharedTable.store(b.getZobristKey(), TranspTableEntry(best->score, best->depth, TranspTableEntry::EXACT, best->move));
        printSearchInfo(b, *best, elapsed, false);
    } else {
        std::cout << "Time taken: " << elapsed << "ms | Threads: " << threadCount
                  << " | Transpositions: " << sharedTable.size() << std::endl;
    }

    return best != results.end() ? best->move : Move();
}

Move ParallelSearcher::timedSearch(const Board &b, int milliseconds) {
    stopSearch.store(false, std::memory_order_relaxed);

    MoveList moves = MoveGen::getLegalMovesFast(b);
    if (moves.empty()) {
        return Move();
    }
    if (moves.size() == 1) {
        return moves[0];
    }

    std::vector<std::thread> threads;
    std::vector<WorkerResult> results(threadCount);
    threads.reserve(threadCount);

    auto start = std::chrono::high_resolution_clock::now();
    for (unsigned int i = 0; i < threadCount; i++) {
        threads.emplace_back([&, i] {
            results[i] = searchWorker(b, 100, int(i));
        });
    }

    while (!stopSearch.load(std::memory_order_relaxed)) {
        long elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::high_resolution_clock::now() - start).count();
        if (elapsed >= milliseconds) {
            stopSearch.store(true, std::memory_order_relaxed);
            break;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }

    for (std::thread &thread : threads) {
        thread.join();
    }

    auto best = selectBestResult(results);

    long elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::high_resolution_clock::now() - start).count();
    if (best != results.end()) {
        sharedTable.store(b.getZobristKey(), TranspTableEntry(best->score, best->depth, TranspTableEntry::EXACT, best->move));
        printSearchInfo(b, *best, elapsed, elapsed >= milliseconds && abs(best->score) <= 900000);
    } else {
        std::cout << elapsed << "ms | Threads: " << threadCount
                  << " | Depth: 0 | Transpositions: " << sharedTable.size() << std::endl;
    }

    return best != results.end() ? best->move : moves[0];
}

void ParallelSearcher::printSearchInfo(const Board &b, const WorkerResult &result, long elapsedMs, bool stopped) {
    std::cout << elapsedMs << "ms | Threads: " << threadCount
              << " | Depth: " << result.depth << (stopped ? "*" : "")
              << " | Transpositions: " << sharedTable.size() << " | ";

    int score = result.score;
    if (!b.isWhiteTurn) {
        score *= -1;
    }

    std::string evalPrint;
    if (abs(score) >= 9999) {
        evalPrint = "Mate in " + std::to_string((1000000 - abs(score) - b.fullMoveNumber)) + " ply";
    } else {
        evalPrint = std::to_string(float(score) / 100);
        evalPrint = evalPrint.substr(0, evalPrint.find('.') + 3);
    }

    std::cout << "Evaluation: " << evalPrint << std::endl;
    printPV(b, result.depth);
}

void ParallelSearcher::printPV(const Board &b, int depth) {
    Board copy = b;
    std::cout << "PV: ";
    for (int i = 0; i < depth; i++) {
        const TranspTableEntry *entry = sharedTable.lookup(copy.getZobristKey());
        if (entry == nullptr || entry->getBestMove().getFlags() == Move::NULL_MOVE) {
            break;
        }
        std::cout << entry->getBestMove().getNotation() << " ";
        copy.makeMove(entry->getBestMove());
    }
    std::cout << std::endl;
}
