cmake_minimum_required(VERSION 3.25)
project(Thud)

set(CMAKE_CXX_STANDARD 20)

add_executable(Thud main.cpp Board.h
        Utils.h
        Board.cpp
        Utils.cpp
        MoveGenerator.cpp
        MoveGenerator.h
        Tablebase_test.h
        Search.cpp
        Search.h
        Tablebase_test_128bit.h
        Perft.cpp
        Perft.h
        perft_tt.h
        Indexer.cpp
        Indexer.h
        Timer.h
        TranspositionTable.h
        Evaluation.cpp
        Evaluation.h
        Parameters.h)
