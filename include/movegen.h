#ifndef MOVEGEN_H_
#define MOVEGEN_H_

#include <stdint.h>
#include "bitboard.h"
#include "typedefs.h"
#include "evaluation.h"
#include "search.h"
#include <vector>
#include <string>
#include <board.h>
#include <immintrin.h>


/*
 * 1	0000    quiet moves
 * 2	0001	double pawn push
 */

namespace GENERATE{
//using pointer and arrays
int AllMove		( struct BOARD A, ExtMove *MoveList, int Color );
int CaptureMove	( struct BOARD A, ExtMove *MoveList, int Color);
int QuietMove	( struct BOARD A, ExtMove *MoveList, int Color);

//using vector::std
std::vector<ExtMove> AllMoves(BOARD_C board, int side);
std::vector<ExtMove> CaptureMoves(BOARD_C board, int side);
std::vector<ExtMove> QuietMoves(BOARD_C board, int side);
std::vector<ExtMove> SpecialMoves(BOARD_C board, int side);

BitBoard Picker	(int chooser, int pos, BitBoard OwnPieces, BitBoard EnemyPieces, int Color, BitBoard capture);
BitBoard Pawn	(int position, BitBoard OwnPieces, BitBoard EnemyPieces, int Color, BitBoard cap);
BitBoard Knight	(int position, BitBoard OwnPieces, BitBoard EnemyPieces);
BitBoard Bishop	(int position, BitBoard OwnPieces, BitBoard EnemyPieces);
BitBoard Rook	(int position, BitBoard OwnPieces, BitBoard EnemyPieces);
BitBoard Queen	(int position, BitBoard OwnPieces, BitBoard EnemyPieces);
BitBoard King	(int position, BitBoard OwnPieces, BitBoard EnemyPieces);
}

namespace MOVE{
BOARD MakeMove(BOARD inital, ExtMove to);
BOARD MakeMove(BOARD initial, Move transformer);
BOARD UndoMove(BOARD &inital, ExtMove to);
}

#endif
