//
// Created by Tao Groves on 2/16/23.
//

#include "tests.h"
#include <iostream>

int main() {
    zKey::init();
    MoveGen::init();
    Board b;

    // ask for a fen string
    std::string fen;
    std::cout << "Enter a FEN string to test, or press enter for starting position: ";
    std::getline(std::cin, fen);
    if (fen.empty()) {
        b = Board();
    } else {
        b = Board(fen.c_str());
    }

    threadselect: bool multithreaded = false;
    std::cout << "Run multithreaded? (y/n): ";
    multithreaded = std::cin.get() == 'y';
    std::cin.ignore();
    bool singlethreaded = false;
    std::cout << "Run singlethreaded? (y/n): ";
    singlethreaded = std::cin.get() == 'y';
    std::cin.ignore();
    if (!multithreaded && !singlethreaded) {
        std::cout << "Please select at least one option." << std::endl;
        goto threadselect;
    }

    int minDepth, maxDepth;
    std::cout << "Enter minimum depth: ";
    std::cin >> minDepth;
    std::cout << "Enter maximum depth: ";
    std::cin >> maxDepth;
    std::cin.ignore();

    for (int i = minDepth; i <= maxDepth; i++) {
        if (multithreaded) {
            std::cout << "Multithreaded depth " << i << ": ";
            tests::perfTestMultithreaded(b, i);
        }
        if (singlethreaded) {
            std::cout << "Singlethreaded depth " << i << ": ";
            tests::perfTest(b, i);
        }
    }

    return 0;
}