//
// Created by Tao G on 2/11/2023.
//

#include <iostream>
#include "tests.h"
#include <chrono>
#include <future>
#include <thread>
#include <algorithm>

bool tests::legalMovesTest(const Board& b, int expected) {
    MoveList legal = MoveGen::getLegalMoves(b);
    if (expected == -1) {
        std::cout << "Found " << legal.size() << " legal moves:" << std::endl;
        for (Move m : legal) {
            std::cout << m.getNotation() << std::endl;
        }
        return true;
    } else {
        return legal.size() == expected;
    }
}

bool tests::perfTest(const Board &b, int depth, int expected) {
    std::cout << "Testing " << depth << " ply search..." << std::endl;
    auto start = std::chrono::high_resolution_clock::now();
    int count = nodeCount(b, depth);
    auto end = std::chrono::high_resolution_clock::now();
    if (expected == -1) {
        std::cout << "Found " << count << " nodes at depth " << depth << std::endl;
        std::cout << "Time taken: " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << "ms";
        auto numNodes = float(count);
        auto numSeconds = float(std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count()) / 1000;
        if (numSeconds != 0)
            std::cout << std::fixed << " | " << int(numNodes / numSeconds) << " nodes/s" << std::endl;
        return true;
    } else {
        return count == expected;
    }
}

bool tests::perfTestMultithreaded(const Board &b, int depth, int expected) {
    std::cout << "Testing " << depth << " ply search..." << std::endl;
    auto start = std::chrono::high_resolution_clock::now();
    unsigned long count = 0;
    MoveList legal = MoveGen::getLegalMoves(b);
    std::vector<std::future<U64>> threads;
    for (auto &m : legal) {
        Board copy = b;
        copy.makeMove(m);

         std::future<U64> fp = (std::async(std::launch::async, nodeCount, copy, depth - 1));
        threads.push_back(std::move(fp));
    }
    for (auto &t : threads) {
        count += t.get();
    }
    auto end = std::chrono::high_resolution_clock::now();
    if (expected == -1) {
        std::cout << "Found " << count << " nodes at depth " << depth << std::endl;
        std::cout << "Time taken: " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << "ms";
        auto numNodes = float(count);
        auto numSeconds = float(std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count()) / 1000;
        if (numSeconds != 0)
            std::cout << std::fixed << " | " << int(numNodes / numSeconds) << " nodes/s" << std::endl;
        return true;
    } else {
        return count == expected;
    }
}

U64 tests::nodeCount(const Board &b, int depth) {
    if (depth == 0) {
        return 1;
    } else if (depth == 1) {
        return MoveGen::getLegalMoves(b).size();
    }

    U64 count = 0;
    MoveList legal = MoveGen::getLegalMoves(b);
    for (Move m : legal) {
        Board copy = b;
        copy.makeMove(m);
        count += nodeCount(copy, depth - 1);
    }

    return count;
}


void tests::NodeCountCompare(const Board& b, int depth, const std::string &compareTo) {
    std::unordered_map<std::string, unsigned int> legalMoves;
    std::unordered_map<std::string, unsigned int> compareMoves;
    std::list<std::string> differences;

    std::vector<Move> topLevel = MoveGen::getLegalMoves(b);
    for (Move &move : topLevel) {
        Board copy = b;
        copy.makeMove(move);
        legalMoves[move.getNotation()] = nodeCount(copy, depth - 1);
    }

    // take a string of the form "a2a4: 490" and parse it into a map
    char cols[9] = {'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'z'};
    for (int i = 0; i < compareTo.length(); i ++) {
        char *col = std::find(cols, cols + 8, compareTo[i]);
        if (*col != 'z') {
            std::string move = compareTo.substr(i, 4);
            std::string count = compareTo.substr(i + 5, compareTo.find('\n', i + 5) - i - 5);
            compareMoves[move] = std::stoi(count);
            i += 6;

        }
    }

    // print out any differences
    for (auto &move : legalMoves) {
        if (compareMoves.find(move.first) == compareMoves.end()) {
            std::cout << "Engine has move " << move.first << " that is not in expected moves." << std::endl;
        } else if (move.second != compareMoves[move.first]) {
            std::cout << "Engine has move " << move.first << " with " << move.second << " nodes, expected " << compareMoves[move.first] << std::endl;
        }
    }

}
