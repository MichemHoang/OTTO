cmake_minimum_required(VERSION 3.5)
project(OTTO)

set(srcs
    src/bitboard.cpp
    src/board.cpp
    src/evaluation.cpp
    src/movegen.cpp
    src/mt64.cpp
    src/openbook.cpp
    src/search.cpp
    src/game.cpp
    src/zobristhash.cpp
)

set(INCLUDE_DIRS include )
include_directories(${INCLUDE_DIRS})

add_executable( OTTO src/main.cpp ${srcs})
target_link_libraries( OTTO ${OpenCV_LIBS} pthread)
