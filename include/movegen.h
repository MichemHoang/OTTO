#ifndef MOVEGEN_H_
#define MOVEGEN_H_

#include <stdint.h>
#include "bitboard.h"
#include "typedefs.h"
#include "evaluation.h"
#include "search.h"
#include <vector>
#include <string>

/*
 * 1		0000			quiet moves
 * 2		0001			double pawn push
 */

namespace GENERATE{
int AllMove		( struct BOARD A, ExtMove *MoveList, int Color );
int CaptureMove	( struct BOARD A, ExtMove *MoveList, int Color);
int QuietMove	( struct BOARD A, ExtMove *MoveList, int Color);
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

void MoveOrdering (ExtMove *MoveList);

namespace DECODE{ 
void DecodeMove	(ExtMove *A);
void DecodeMove	(Move A);
}
#endif
