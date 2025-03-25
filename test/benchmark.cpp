#include "board.h"
#include "bitboard.h"
#include "movegen.h"

#include <search.h>
#include <chrono>
#include <functional>

#define BENCHMARK_F

//ill care about clock resolution later
template <typename Func, typename... Args>
void BENCHMARK(Func func, const std::string& funcName, int loop, Args... args) {
    auto start = std::chrono::high_resolution_clock::now();
    for (int i=0; i < loop; i++){
        //check for callable?? maybe use std::invocable?
        //what if i put in some bs? what happen at run time?
        func(std::forward<Args>(args)...);
    }

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration = end - start;
    std::cout << "Time taken by " << funcName << ": " << duration.count() << " seconds\n";
}

int main(){
    BOARD test1;
    BOARD_C test;
    std::string FENFEN = "4rb2/3qrk2/1p1p1n2/7p/P2P4/4R2P/1BQN1P2/1K4R1 w - - 3 39";
    ExtMove MoveList[MAX_MOVES];
    //std::string FENFEN = STANDARD;
    FEN_Op::READ_FEN(FENFEN, &test1);
    test.ReadFENString(FENFEN);
    INITIALIZE::Mask();
	INITIALIZE::MoveData();
    //std::vector<ExtMove> AAA = GENERATE::AllMoves(test, test.Side_to_move);
    test.PrintAllMove();
    BENCHMARK(GENERATE::AllMoves, "GENERATE::AllMoves", 200000, test, test.Side_to_move);
    BENCHMARK(GENERATE::AllMove, "GENERATE::AllMove", 200000, test1, MoveList, test1.Side_to_move);
    BENCHMARK(GENERATE::AllMoves_MP, "GENERATE::AllMoves_MP", 200000, test, test.Side_to_move);
}