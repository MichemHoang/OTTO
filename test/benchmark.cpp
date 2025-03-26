#include "board.h"
#include "bitboard.h"
#include "movegen.h"

#include <search.h>
#include <chrono>
#include <functional>
#include <fstream>

//ill care about clock resolution later
template <typename Func, typename... Args>
void BENCHMARK(Func func, const std::string& funcName, int loop, Args... args) {
    auto start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < loop; i++){
        //check for callable?? maybe use std::invocable?
        func(std::forward<Args>(args)...);
    }

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration = end - start;
    std::cout << "Time taken by " << funcName << ": " << duration.count() << " seconds\n";
    //return duration.count();
}

int main(){
    //initialize move data
    INITIALIZE::Mask();
    INITIALIZE::MoveData();

    std::vector<BOARD> boardTest;
    std::vector<BOARD_C> boardcTest;

    std::ifstream infile("../test/FenString.txt");
    std::string fenString;
    
    while (std::getline(infile, fenString))
    {
        BOARD newBoard;
        BOARD_C newBoard_C;
        FEN_Op::READ_FEN(fenString, &newBoard);
        newBoard_C.ReadFENString(fenString);
        boardTest.push_back(newBoard);
        boardcTest.push_back(newBoard_C);
    }

    for (int i = 0; i < boardcTest.size(); i++){
        ExtMove MoveList[MAX_MOVES];
        BENCHMARK(GENERATE::AllMoves, "GENERATE::AllMoves", 100000, boardcTest[i], boardcTest[i].Side_to_move);
        BENCHMARK(GENERATE::AllMove, "GENERATE::AllMove", 100000, boardTest[i], MoveList, boardTest[i].Side_to_move);
        BENCHMARK(GENERATE::AllMoves_MP, "GENERATE::AllMoves_MP", 100000, boardcTest[i], boardcTest[i].Side_to_move);
        std::cout << "---------------------------------------------------------\n";
    }
}