cmake_minimum_required(VERSION 3.10.0)
project(Chess VERSION 0.1.0 LANGUAGES C CXX)

add_executable(Chess src/main.cpp
    src/ChessBoard.c
    include/ChessBoard.h)

target_include_directories(Chess PRIVATE
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/src
)

add_executable(ChessTests
    tests/ChessBoardTests.cpp
    src/ChessBoard.c
    include/ChessBoard.h)

target_include_directories(ChessTests PRIVATE
    ${CMAKE_SOURCE_DIR}/include
)

include(CTest)
enable_testing()

add_test(NAME ChessBoardTests COMMAND ChessTests)

