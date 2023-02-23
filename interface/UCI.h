//
// Created by Tao G on 2/13/2023.
//

#ifndef SEKHMET_UCI_H
#define SEKHMET_UCI_H

#include <iostream>
#include <string>
#include "../models/Board.h"
#include "../search/Search.h"
#include "../core/MoveGen.h"

class UCI {
public:
    constexpr static const char* const engineName = "Sekhmet";
    constexpr static const char* const version = "3.2";

    bool engineRunning;
    bool newGameComing;

    int searchDepth = 4;
    int searchTime = 1000;

    Board b;
    Searcher s = Searcher();

    UCI();

private:
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
};


#endif //SEKHMET_UCI_H
