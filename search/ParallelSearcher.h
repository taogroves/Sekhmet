#ifndef SEKHMET_PARALLELSEARCHER_H
#define SEKHMET_PARALLELSEARCHER_H

#include <atomic>
#include <thread>
#include <vector>
#include "Search.h"

class ParallelSearcher {
public:
    ParallelSearcher();

    Move restrictedSearch(const Board &b, const Searcher::searchRestrictions &restrictions);

    void setThreadCount(unsigned int count);
    unsigned int getThreadCount() const;
    void reset(bool continueGame = false);

private:
    struct WorkerResult {
        Move move;
        int score = 0;
        int depth = 0;
    };

    unsigned int threadCount;
    zTable sharedTable;
    std::atomic<bool> stopSearch = false;

    static unsigned int defaultThreadCount();
    int allocateTime(const Board &b, const Searcher::searchRestrictions &restrictions) const;
    WorkerResult searchWorker(const Board &b, int maxDepth, int workerIndex);
    Move timedSearch(const Board &b, int milliseconds);
    Move depthSearch(const Board &b, int depth);
    void printSearchInfo(const Board &b, const WorkerResult &result, long elapsedMs, bool stopped);
    void printPV(const Board &b, int depth);
};

#endif
