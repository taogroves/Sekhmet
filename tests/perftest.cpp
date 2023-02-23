//
// Created by Tao Groves on 2/16/23.
//

#include "tests.h"
#include <iostream>

int main() {
    zKey::init();
    MoveGen::init();
    std::cout << "Running standard tests..." << std::endl;
    Board b = Board();
    for (int i = 4; i < 20; i++) {
        std::cout << "Depth " << i << " multithreaded" << std::endl;
        tests::perfTestMultithreaded(b, i);
        std::cout << "Depth " << i << " singlethreaded" << std::endl;
        tests::perfTest(b, i);
    }
    return 0;
}