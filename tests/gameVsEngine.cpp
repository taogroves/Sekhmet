//
// Created by Tao G on 2/12/2023.
//


#include <iostream>

#include "../models/Board.h"
#include "../search/Search.h"
#include "../core/MoveGen.h"
#include "../search/eval.h"

Move playerInput(const Board &b) {
    // get all legal moves
    std::vector<Move> legal = MoveGen::getLegalMoves(b);

    // if there are no legal moves, return a move that will end the game
    if (legal.empty())
        return Move();

    // print the board, putting an asterisk next to each piece that can move
    b.printBoard(false, !b.isWhiteTurn);

    // get and parse the move from the user
    get: std::string move;
    std::cout << "Enter a move: ";
    std::cin >> move;
    int from = (move[0] - 'a') + (7 - (move[1] - '1')) * 8;
    int to = (move[2] - 'a') + (7 - (move[3] - '1')) * 8;
    Move m = Move(from, to, b.getPieceOnSquare(from, b.isWhiteTurn));
    // promotion
    if (move.length() == 5) {
        m.setFlag(Move::PROMOTION);
        switch (move[4]) {
            case 'q':
                m.setPromotionPieceType(QUEEN);
                break;
            case 'r':
                m.setPromotionPieceType(ROOK);
                break;
            case 'b':
                m.setPromotionPieceType(BISHOP);
                break;
            case 'n':
                m.setPromotionPieceType(KNIGHT);
                break;
        }
    }
    // capture
    if (b.getPieceOnSquare(to, !b.isWhiteTurn) != NULL_PIECE) {
        m.setFlag(Move::CAPTURE);
        m.setCapturedPieceType(b.getPieceOnSquare(to, !b.isWhiteTurn));
    }
        // en passant
    else if (b.getPieceOnSquare(from, b.isWhiteTurn) == (PAWN) && (abs(from - to) == 7 || abs(from - to) == 9)) {
        m.setFlag(Move::EN_PASSANT);
    }

    // castle
    else if (b.getPieceOnSquare(from, b.isWhiteTurn) == (KING) && (abs(from - to) == 2)) {
        if (to == 62 || to == 6) {
            m.setFlag(Move::KSIDE_CASTLE);
        } else {
            m.setFlag(Move::QSIDE_CASTLE);
        }
    }

    std::cout << m.getNotation() << std::endl;

    // check if the move is legal
    for (Move check : legal) {
        if (check.getNotation() == m.getNotation()) {
            return m;
        }
    }
    std::cout << "Illegal move - ";
    if (b.getPieceOnSquare(from, b.isWhiteTurn) == NULL_PIECE) {
        if (b.getPieceOnSquare(from, !b.isWhiteTurn) == NULL_PIECE)
            std::cout << "there is no piece on that square." << std::endl;
        else
            std::cout << "that piece is not yours." << std::endl;
    } else {
        std::cout << "that piece cannot move to that square." << std::endl;
    }
    goto get;
}

Move engineMove(const Board &b, int time, Searcher::Algorithm alg, Searcher::Evaluation eval) {
    auto s = Searcher();
    s.setEvaluation(eval);
    s.setAlgorithm(alg);
    Move m = s.timedSearch(b, time * 1000);
    std::cout << (b.isWhiteTurn ? "White " : "Black ") << "plays " << m.getNotation() << std::endl;
    return m;
}

int main() {
    zKey::init();
    MoveGen::init();
    eval::init();

    auto b = Board();
    bool w_is_human = true;
    bool b_is_human = true;

    // ask the user if white is a human or an engine
    std::cout << "Is white a human? (y/n): ";
    std::string input;
    std::cin >> input;
    if (input == "n")
        w_is_human = false;

    // if white is an engine, ask for the algorithm, evaluation, and time
    int w_time = 0;
    Searcher::Algorithm w_alg;
    Searcher::Evaluation w_eval;
    if (!w_is_human) {
        std::cout << "Select a search algorithm for white (1: NegaMax, 2: PVS, 3: PVS w/ Null-Move Pruning, 4: PVS w/ NMP & Late Move Reduction): ";
        std::cin >> input;
        switch (input[0]) {
            case '1':
                w_alg = Searcher::NEGAMAX;
                break;
            case '2':
                w_alg = Searcher::ZOBRIST;
                break;
            case '3':
                w_alg = Searcher::NULLMOVE_PVS;
                break;
            case '4':
                w_alg = Searcher::NL_LM_PVS;
                break;
        }
        std::cout << "Select an evaluation for white (1: Material, 2: Material + Quiescence, 3: Position, 4: Position + Quiescence): ";
        std::cin >> input;
        switch (input[0]) {
            case '1':
                w_eval = Searcher::MATERIAL;
                break;
            case '2':
                w_eval = Searcher::QUIESCENT_MATERIAL;
                break;
            case '3':
                w_eval = Searcher::PST;
                break;
            case '4':
                w_eval = Searcher::QUIESCENT_PST;
                break;
        }
        std::cout << "How long should white think (in seconds)? ";
        std::cin >> w_time;
    }

    // ask the user if black is a human or an engine
    std::cout << "Is black a human? (y/n): ";
    std::cin >> input;
    if (input == "n")
        b_is_human = false;

    // if black is an engine, ask for the algorithm, evaluation, and time
    int b_time = 0;
    Searcher::Algorithm b_alg;
    Searcher::Evaluation b_eval;
    if (!b_is_human) {
        std::cout << "Select a search algorithm for black (1: NegaMax, 2: PVS, 3: PVS w/ Null-Move Pruning, 4: PVS w/ NMP & Late Move Reduction): ";
        std::cin >> input;
        switch (input[0]) {
            case '1':
                b_alg = Searcher::NEGAMAX;
                break;
            case '2':
                b_alg = Searcher::ZOBRIST;
                break;
            case '3':
                b_alg = Searcher::NULLMOVE_PVS;
                break;
            case '4':
                b_alg = Searcher::NL_LM_PVS;
                break;
        }
        std::cout << "Select an evaluation for black (1: Material, 2: Material + Quiescence, 3: Position, 4: Position + Quiescence): ";
        std::cin >> input;
        switch (input[0]) {
            case '1':
                b_eval = Searcher::MATERIAL;
                break;
            case '2':
                b_eval = Searcher::QUIESCENT_MATERIAL;
                break;
            case '3':
                b_eval = Searcher::PST;
                break;
            case '4':
                b_eval = Searcher::QUIESCENT_PST;
                break;
        }
        std::cout << "How long should black think (in seconds)? ";
        std::cin >> b_time;
    }

    // main game loop
    while (true) {
        //b.printBoard();
        // if there are no legal moves, the game is over
        if (MoveGen::getLegalMoves(b).empty())
            break;

        // if it's white's turn, get a move from the user or the engine
        if (b.isWhiteTurn) {
            Move m;
            if (w_is_human)
                m = playerInput(b);
            else
                m = engineMove(b, w_time, w_alg, w_eval);
            b.makeMove(m);
        }

        // if it's black's turn, get a move from the user or the engine
        else {
            Move m;
            if (b_is_human)
                m = playerInput(b);
            else
                m = engineMove(b, b_time, b_alg, b_eval);
            b.makeMove(m);
        }
    }

    // print the final board
    b.printBoard();
    std::cout << (b.isInCheck(b.isWhiteTurn) ? "Checkmate!" : "Stalemate") << " | ";
    std::cout << (b.isWhiteTurn ? "Black" : "White") << " wins!" << std::endl;
}