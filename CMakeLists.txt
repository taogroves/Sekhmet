cmake_minimum_required(VERSION 3.19)
project(Sekhmet)

set(CMAKE_CXX_STANDARD 20)

set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O2")

# enable this to use the utility functions in /external/
#add_executable(HelperFunctions external/genPawnMasks.cpp)

add_executable(Sekhmet core/LookupTables.h core/main.cpp transpositions/zKey.cpp transpositions/zKey.h models/Board.h models/Board.cpp models/Move.h models/Move.cpp core/MoveGen.cpp core/MoveGen.h
        search/Search.cpp search/Search.h transpositions/zTable.cpp transpositions/zTable.h search/eval.cpp search/eval.h transpositions/TranspTableEntry.h interface/UCI.cpp interface/UCI.h search/OrderingData.cpp search/OrderingData.h search/MovePicker.cpp search/MovePicker.h)

add_executable(PerfTests tests/perftest.cpp tests/tests.h tests/tests.cpp models/Board.h models/Board.cpp models/Move.h models/Move.cpp transpositions/zKey.cpp transpositions/zKey.h core/MoveGen.cpp core/MoveGen.h transpositions/zTable.cpp transpositions/zTable.h)
add_executable(PlayVsEngine tests/gameVsEngine.cpp models/Board.h models/Board.cpp models/Move.h models/Move.cpp transpositions/zKey.cpp transpositions/zKey.h core/MoveGen.cpp core/MoveGen.h transpositions/zTable.cpp transpositions/zTable.h search/Search.cpp search/Search.h search/eval.cpp search/eval.h transpositions/TranspTableEntry.h interface/UCI.cpp interface/UCI.h search/OrderingData.cpp search/OrderingData.h search/MovePicker.cpp search/MovePicker.h)