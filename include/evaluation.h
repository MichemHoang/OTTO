#ifndef EVALUATION_H_
#define EVALUATION_H_

#include "bitboard.h"
#include "movegen.h"
#include "board.h"

namespace EVALUATION{
int LVA(int att_Sqr, BOARD A, int side);
int	PieceSquareValue(int isPawn, uint16_t from, uint16_t to);
int	Evaluate (BOARD A, int side);
int	EvaluateBoard (BOARD_C A, int side);
int	SEEA(int To, BOARD A, int from); // static exhange equaivalent
int	SEEA(int To, BOARD_C A, int from); // static exhange equaivalent
int LVA	(int att_Sqr, BOARD A, int side);
int LVA (int att_Sqr, BOARD_C board, int side); //Least valuable attacker
int	DynamicEval	(BOARD A, int Alpha, int Beta);
int	DynamicEval	(BOARD_C A, int Alpha, int Beta); //Dynamic Evaluation
}
#endif 
