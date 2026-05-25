//
// Created by Tao G on 2/13/2023.
//

#include <algorithm>
#include <cctype>
#include <cstring>
#include "UCI.h"

UCI::UCI() {
    // print welcome message, then enter UCI loop
    std::cout << engineName << " version " << version << std::endl;
    UCI_loop();
}

UCI::~UCI() {
    stopSearchThread();
}

void UCI::UCI_loop() {
    std::string command;
    engineRunning = true;
    newGameComing = true;
    while (engineRunning) {
        std::getline(std::cin, command);
        engineRunning = true;
        //newGameComing = true;
        UCI_command(command);
    }
}

void UCI::UCI_command(const std::string& command) {

    if (command == "uci") {
        std::cout << "id name " << engineName << std::endl;
        std::cout << "id author Tao Groves" << std::endl;
        std::cout << "option name Threads type spin default " << s.getThreadCount() << " min 1 max 512" << std::endl;
        std::cout << "option name Ponder type check default false" << std::endl;
        std::cout << "uciok" << std::endl;
    } else if (command == "isready") {
        UCI_isready();
    } else if (command.substr(0, 9) == "setoption") {
        UCI_setoption(command);
    } else if (command.substr(0, 8) == "register") {
        UCI_register(command);
    } else if (command == "ucinewgame") {
        UCI_ucinewgame();
    } else if (command.substr(0, 8) == "position") {
        UCI_position(command);
    } else if (command.substr(0, 2) == "go") {
        UCI_go(command);
    } else if (command == "stop") {
        UCI_stop();
    } else if (command == "ponderhit") {
        UCI_ponderhit();
    } else if (command == "quit") {
        UCI_quit();
    } else if (command == "d") {
        b.printBoard();
    } else {
        std::cout << "Unknown command: " << command << std::endl;
    }
}

void UCI::UCI_isready() {
    std::cout << "readyok" << std::endl;
}

void UCI::UCI_setoption(std::string command) {
    std::string prefix = "setoption name Threads value ";
    if (command.rfind(prefix, 0) == 0) {
        int count = std::stoi(command.substr(prefix.size()));
        s.setThreadCount(std::max(1, count));
        std::cout << "Threads set to " << s.getThreadCount() << std::endl;
        return;
    }

    prefix = "setoption name Ponder value ";
    if (command.rfind(prefix, 0) == 0) {
        std::string value = command.substr(prefix.size());
        std::transform(value.begin(), value.end(), value.begin(), [](unsigned char c) {
            return char(std::tolower(c));
        });
        ponderOption = value == "true";
    }
}

void UCI::UCI_register(std::string command) {

}

void UCI::UCI_ucinewgame() {
    newGameComing = true;
    std::cout << "Engine reset." << std::endl;
}

void UCI::UCI_position(const std::string& command) {

    if (command.substr(9, 8) == "startpos") {
        b = Board();
        if (strlen(command.c_str()) > 18) {
            if (command.substr(18, 5) == "moves") {
                MoveList legal;
                std::string moves = command.substr(23);
                std::string move;
                for (char i : moves) {
                    if (i == ' ') {
                        legal = MoveGen::getLegalMovesFast(b);
                        for (Move m : legal) {
                            if (m.getNotation() == move) {
                                b.makeMove(m);
                            }
                        }
                        move = "";
                    } else {
                        move += i;
                    }
                }
                legal = MoveGen::getLegalMovesFast(b);
                for (Move m : legal) {
                    if (m.getNotation() == move) {
                        b.makeMove(m);
                    }
                }
            }
        }
    }

    else if (command.substr(9, 3) == "fen") {
        std::string fen = command.substr(13);
        b = Board((char *) fen.c_str());
        if (strlen(command.c_str()) > fen.length() + 9) {
            char* token = strtok((char *) command.c_str(), " ");
            while (token != NULL && strcmp(token, "moves") != 0) {
                token = strtok(NULL, " ");
            }
            token = strtok(NULL, " ");
            while (token != NULL) {
                MoveList legal = MoveGen::getLegalMovesFast(b);
                for (Move m : legal) {
                    if (m.getNotation() == token) {
                        b.makeMove(m);
                    }
                }
                token = strtok(NULL, " ");
            }
        }
    }
}

void UCI::UCI_go(const std::string& command) {
    Searcher::searchRestrictions restrictions = parseGoRestrictions(command);

    stopSearchThread();
    joinSearchThread();

    newGameComing = true;
    if (newGameComing)
        s.reset(false);
    else
        s.reset(true);

    if (restrictions.ponder) {
        ponderHitRestrictions = restrictions;
        ponderHitRestrictions.ponder = false;

        Searcher::searchRestrictions ponderRestrictions = restrictions;
        ponderRestrictions.infinite = true;
        ponderRestrictions.depth = 0;
        ponderRestrictions.movetime = 0;

        startSearch(b, ponderRestrictions, false);
    } else {
        startSearch(b, restrictions, true);
    }
}

Searcher::searchRestrictions UCI::parseGoRestrictions(const std::string& command) {
    Searcher::searchRestrictions restrictions;
    if (strlen(command.c_str()) > 3) {
        std::string commandCopy = command;
        char *token = strtok(commandCopy.data(), " ");
        while (token != NULL) {
            if (strcmp(token, "movetime") == 0) {
                token = strtok(NULL, " ");
                restrictions.movetime = std::stoi(token);
            } else if (strcmp(token, "wtime") == 0) {
                token = strtok(NULL, " ");
                restrictions.time[1] = std::stoi(token);
            } else if (strcmp(token, "btime") == 0) {
                token = strtok(NULL, " ");
                restrictions.time[0] = std::stoi(token);
            } else if (strcmp(token, "winc") == 0) {
                token = strtok(NULL, " ");
                restrictions.increment[1] = std::stoi(token);
            } else if (strcmp(token, "binc") == 0) {
                token = strtok(NULL, " ");
                restrictions.increment[0] = std::stoi(token);
            } else if (strcmp(token, "movestogo") == 0) {
                token = strtok(NULL, " ");
                restrictions.movesToGo = std::stoi(token);
            } else if (strcmp(token, "depth") == 0) {
                token = strtok(NULL, " ");
                restrictions.depth = std::stoi(token);
            } else if (strcmp(token, "infinite") == 0) {
                restrictions.infinite = true;
            } else if (strcmp(token, "ponder") == 0) {
                restrictions.ponder = true;
            }
            token = strtok(NULL, " ");
        }
    }
    return restrictions;
}

void UCI::startSearch(const Board &position, const Searcher::searchRestrictions &restrictions, bool printBestMove) {
    {
        std::lock_guard<std::mutex> lock(searchMutex);
        ponderSearchInProgress = restrictions.ponder;
        completedPonderMoveAvailable = false;
    }

    searchThread = std::thread([this, position, restrictions, printBestMove] {
        Move best = s.restrictedSearch(position, restrictions);
        if (best.getFlags() & Move::NULL_MOVE) {
            MoveList legal = MoveGen::getLegalMovesFast(position);
            if (!legal.empty()) {
                best = legal[0];
            }
        }

        std::lock_guard<std::mutex> lock(searchMutex);
        if (printBestMove) {
            outputBestMove(position, best);
            newGameComing = false;
        } else {
            completedPonderMove = best;
            completedPonderMoveAvailable = true;
        }
    });
}

void UCI::outputBestMove(const Board &position, const Move &bestMove) {
    std::cout << "bestmove " << bestMove.getNotation();
    if (ponderOption) {
        Move ponderMove = s.getPonderMove(position, bestMove);
        if (!(ponderMove.getFlags() & Move::NULL_MOVE)) {
            std::cout << " ponder " << ponderMove.getNotation();
        }
    }
    std::cout << std::endl;
}

void UCI::joinSearchThread() {
    if (searchThread.joinable()) {
        searchThread.join();
    }
}

void UCI::stopSearchThread() {
    s.stop();
    joinSearchThread();
    std::lock_guard<std::mutex> lock(searchMutex);
    ponderSearchInProgress = false;
}

void UCI::UCI_stop() {
    stopSearchThread();
}

void UCI::UCI_ponderhit() {
    bool hasCompletedPonderMove;
    bool hasPonderSearch;
    Move ponderMove;
    {
        std::lock_guard<std::mutex> lock(searchMutex);
        hasCompletedPonderMove = completedPonderMoveAvailable && ponderSearchInProgress;
        hasPonderSearch = ponderSearchInProgress;
        ponderMove = completedPonderMove;
    }

    if (!hasPonderSearch) {
        return;
    }

    if (hasCompletedPonderMove) {
        stopSearchThread();
        outputBestMove(b, ponderMove);
        newGameComing = false;
        return;
    }

    stopSearchThread();
    s.reset(true);
    startSearch(b, ponderHitRestrictions, true);
}

void UCI::UCI_quit() {
    stopSearchThread();
    engineRunning = false;
    exit(0);
}
