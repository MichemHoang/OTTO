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
    InitZoBrist(true);

    std::vector<BOARD> boardTest;
    std::vector<BOARD_C> boardcTest;

    std::ifstream infile("../test/FenString.txt");
    std::string fenString;
    Search alphaBeta, alphaBeta1;
    BOARD_C puzzle1;
    BOARD puzzle11;
    ExtMove result;
    
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
        BENCHMARK(EVALUATION::EvaluateBoard, "EVALUATION::EvaluateBoard", 100000, boardcTest[i], boardcTest[i].Side_to_move);
        BENCHMARK(GetKeyString, "BITSTRING::GetKey", 100000, boardcTest[i]);
        std::cout << "---------------------------------------------------------\n";
    }

    puzzle1.ReadFENString("rnb1k2r/pp2b1pp/2p2pn1/q3P2Q/5p2/PB6/1BPP2PP/RN2K1NR w KQkq - 0 1");
    alphaBeta.SetTableSize(30000000);
    alphaBeta.SetPosition(puzzle1);
    FEN_Op::READ_FEN("rnb1k2r/pp2b1pp/2p2pn1/q3P2Q/5p2/PB6/1BPP2PP/RN2K1NR w KQkq - 0 1",&puzzle11);
    alphaBeta1.SetTableSize(30000000);
    alphaBeta1.SetPosition(puzzle11);
    std::pair<Move, int> RES;

    auto start = std::chrono::high_resolution_clock::now();
    result = alphaBeta.IterativeDeepening(10, true);
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration = end - start;
    std::cout << "Time taken : " << duration.count() << " seconds\n";

    //-----------------------------------------------------------------------------------------
    start = std::chrono::high_resolution_clock::now();
    RES = alphaBeta1.IterativeDeepening(12);
    end = std::chrono::high_resolution_clock::now();
    duration = end - start;
    std::cout << "Time taken : " << duration.count() << " seconds\n";
}