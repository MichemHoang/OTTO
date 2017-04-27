#ifndef EVALUATION_H_
#define EVALUATION_H_

#include  "BitBoard.h"
#include  "MoveGen.h"
#include  "evaluation.h"

int	PieceSquareValue(int isPawn, uint16_t from, uint16_t to);

int	EvaluateBOARD	(BOARD A, int side);

int	SEE	(int Sqr, BOARD A, int attacker, int turn);

int	SEEA(int To, BOARD A, int from);

int LeastValuableAttacker	(int att_Sqr, BOARD A, int side);

#endif 
