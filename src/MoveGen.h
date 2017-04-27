#ifndef MOVEGEN_H_
#define MOVEGEN_H_

#include <stdint.h>
#include <algorithm>    // std::sort
#include "BitBoard.h"
#include "typedefs.h"
#include "evaluation.h"
#include "search.h"
#include <vector>
#include <string>

/*
 * 1		0000			quiet moves
 * 2		0001			double pawn push
 */

using namespace BEGIN_GAME;

inline bool operator<(const ExtMove& f, const ExtMove& s) {
  return f.value < s.value;
}

int GenerateAllMove( struct BOARD A, ExtMove *MoveList, int Color );

int GenerateCaptureMove( struct BOARD A, ExtMove *MoveList, int Color );

bool isCheck	(int Side);

BOARD	MakeMove(BOARD inital, ExtMove to);

BOARD	MakeMove(BOARD initial, Move transformer);

BOARD	UndoMove(BOARD &inital, ExtMove to);

/*
 * Ordering move according to extMove.value
 */
 
void MoveOrdering	(ExtMove *MoveList);

/*
 * return Bitboard resulted from a move;
 */ 

BitBoard Picker(int chooser, int pos, BitBoard OwnPieces, BitBoard EnemyPieces, int Color, BitBoard capture);
 
std::string DecodeMove	(ExtMove *A, int i);

std::string DecodeMove	(Move A, int i);
 
void DecodeMove	(ExtMove *A);

BitBoard generatePawnMove		(int position, BitBoard OwnPieces, BitBoard EnemyPieces, int Color, BitBoard cap);

BitBoard generateKnightMove		(int position, BitBoard OwnPieces, BitBoard EnemyPieces);

BitBoard generateBishopMove		(int position, BitBoard OwnPieces, BitBoard EnemyPieces);

BitBoard generateRookMove		(int position, BitBoard OwnPieces, BitBoard EnemyPieces);

BitBoard generateQueenMove		(int position, BitBoard OwnPieces, BitBoard EnemyPieces);

BitBoard generateKingMove		(int position, BitBoard OwnPieces, BitBoard EnemyPieces);

void DecodeMove	(Move A);

#endif
