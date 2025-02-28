cmake_minimum_required(VERSION 3.5)
project(OTTO)

# GoogleTest requires at least C++14
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)
FetchContent_MakeAvailable(googletest)

enable_testing()

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
include(GoogleTest)

add_executable( OTTO src/main.cpp ${srcs})
target_link_libraries( OTTO ${OpenCV_LIBS} pthread)

add_executable( OTTO_test test/unittest.cpp ${srcs})
target_link_libraries( OTTO_test gtest gtest_main)

gtest_discover_tests(OTTO_test)

