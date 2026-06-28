//
// Created by Tao G on 2/13/2023.
//

#ifndef SEKHMET_UCI_H
#define SEKHMET_UCI_H

#include <iostream>
#include <mutex>
#include <string>
#include <thread>
#include "../models/Board.h"
#include "../search/ParallelSearcher.h"
#include "../core/MoveGen.h"

class UCI {
public:
    constexpr static const char* const engineName = "Sekhmet";
    constexpr static const char* const version = "3.2";

    bool engineRunning;
    bool newGameComing;
    bool ponderOption = false;

    int searchDepth = 4;
    int searchTime = 1000;

    Board b;
    ParallelSearcher s = ParallelSearcher();

    UCI();
    ~UCI();

private:
    std::thread searchThread;
    std::mutex searchMutex;
    bool ponderSearchInProgress = false;
    bool completedPonderMoveAvailable = false;
    Move completedPonderMove;
    Searcher::searchRestrictions ponderHitRestrictions;

    void UCI_loop();
    void UCI_command(const std::string& command);

    static void UCI_isready();
    void UCI_setoption(std::string command);
    void UCI_register(std::string command);
    void UCI_ucinewgame();
    void UCI_position(const std::string& command);
    void UCI_go(const std::string& command);
    void UCI_stop();
    void UCI_ponderhit();
    void UCI_quit();

    Searcher::searchRestrictions parseGoRestrictions(const std::string& command);
    void startSearch(const Board &position, const Searcher::searchRestrictions &restrictions, bool printBestMove);
    void outputBestMove(const Board &position, const Move &bestMove);
    void joinSearchThread();
    void stopSearchThread();
};


#endif //SEKHMET_UCI_H
