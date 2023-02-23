//
// Created by Tao G on 2/7/2023.
//



#include "../interface/UCI.h"
#include "../search/eval.h"

int main() {
    zKey::init();
    MoveGen::init();
    eval::init();

    UCI uci = UCI();

    return 0;
}