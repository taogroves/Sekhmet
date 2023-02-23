# Sekhmet - Fast, Optimized UCI Chess Engine
Sekhmet is a UCI chess engine written in C++, named after the Egyptian goddess of war. It 
plays at a ~2000 level in rapid, and becomes significantly stronger relative to a human player
as time control shortens. 
Recognizes most UCI commands/parameters; uses time appropriately in multi- and single- phase games.

Supports all rules of standard chess, including 50 move rule, draw by repetition, and
stalemate. 
## Engine Structure
### Boards
- Position Encoding: **Bitboards**
- Move Encoding: **int32**
### Search
- Alpha-Beta Negamax
- Zobrist Hashing
- Iterative Deepening
### Evaluation
- Quiescence Search
- Static Evaluation

## Features
- Full, stateless UCI support.
- Fast Move Generation: ~42M nodes/sec singlethreaded, ~250 nodes/sec multithreaded with O2 compiler optimizations on a 5.3GHz Intel i7-12700K.
- Extensive Search Optimization
  - Killer Move Heuristic
  - History Heuristic
  - Principal Variation Search
  - Verified Null-Move Pruning
  - Adaptive Late Move Reduction
  - Aspiration Windows
- Tapered Evaulation
  - Piece Square Tables
  - Pawn Structure
    - Passed Pawns
    - Doubled/Isolated Pawns
    - Defended Pawns
  - Bishop/Queen Mobility
  - Rook/Queen File Control
  - King Safety

Includes three executables; Sekhmet.exe is the main engine, and playVsEngine.exe allows you to play against the engine/battle different engine parameters using a command line interface.
