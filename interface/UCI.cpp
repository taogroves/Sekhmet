//
// Created by Tao G on 2/13/2023.
//

#include <cstring>
#include "UCI.h"

UCI::UCI() {
    // print welcome message, then enter UCI loop
    std::cout << engineName << " version " << version << std::endl;
    s.setAlgorithm(Searcher::Algorithm::NL_LM_PVS);
    //s.setAlgorithm(Searcher::Algorithm::NEGAMAX);
    s.setEvaluation(Searcher::Evaluation::QUIESCENT_PST);
    UCI_loop();
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
        std::cout << "uciok" << std::endl;
    } else if (command == "isready") {
        UCI_isready();
    } else if (command.substr(0, 8) == "setoption") {
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
                        legal = MoveGen::getLegalMoves(b);
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
                legal = MoveGen::getLegalMoves(b);
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
                MoveList legal = MoveGen::getLegalMoves(b);
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
    Searcher::searchRestrictions restrictions;
    if (strlen(command.c_str()) > 3) {
        char *token = strtok((char *) command.c_str(), " ");
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
            }
            token = strtok(NULL, " ");
        }
    }
    newGameComing = true;
    if (newGameComing)
        s.reset(false);
    else
        s.reset(true);

    Move best = s.restrictedSearch(b, restrictions);
    std::cout << "bestmove " << best.getNotation() << std::endl;
    newGameComing = false;
}

void UCI::UCI_stop() {

}

void UCI::UCI_ponderhit() {

}

void UCI::UCI_quit() {
    engineRunning = false;
    exit(0);
}