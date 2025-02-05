#ifndef EVALUATION_H_
#define EVALUATION_H_

#include  "bitboard.h"
#include  "movegen.h"

namespace EVALUATION{
int	PieceSquareValue(int isPawn, uint16_t from, uint16_t to);
int	Evaluate	(BOARD A, int side);
int	SEEA(int To, BOARD A, int from);
int LVA	(int att_Sqr, BOARD A, int side);
int	DynamicEval	(BOARD A, int Alpha, int Beta);
}
#endif 
