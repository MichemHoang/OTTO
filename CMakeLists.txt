#At least 3.9 for easier integration with openMP
cmake_minimum_required(VERSION 3.9)
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

find_package(OpenMP REQUIRED)

set(INCLUDE_DIRS include )
include_directories(${INCLUDE_DIRS})
include(GoogleTest)

add_library(OTTO SHARED ${srcs})

target_include_directories(OTTO PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)
target_link_libraries( OTTO pthread OpenMP::OpenMP_CXX)

# Add the OTTO_main executable -- makeshift game
add_executable(OTTO_main src/main.cpp)
target_link_libraries(OTTO_main PRIVATE OTTO)

# Add the OTTO_test executable
add_executable(OTTO_test test/unittest.cpp)
target_link_libraries(OTTO_test PRIVATE OTTO GTest::gtest GTest::gtest_main)

# Add the OTTO_benchmark executable
add_executable(OTTO_benchmark test/benchmark.cpp)
target_link_libraries(OTTO_benchmark PRIVATE OTTO OpenMP::OpenMP_CXX)

gtest_discover_tests(OTTO_test)

