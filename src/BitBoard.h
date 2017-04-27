#ifndef BITBOARD_H_
#define BITBOARD_H_

#include <stdint.h>

#include "typedefs.h"

/*
 * BitBoard manipulation
 * Bit level manipulation
 * Important BitBoard value
 */
 
extern uint8_t	BitCount[65536];
extern BitBoard Rook_AttackMask[64];
extern BitBoard Knight_AttackMask[64];
extern BitBoard King_AttackMask[64];
extern BitBoard Bishop_AttackMask[64];
extern BitBoard	Pawn_AttackMask[2][64];

extern BitBoard magicNumberRook[];
extern BitBoard magicNumberBishop[];

extern int Shift_R[];
extern int Shift_B[];

extern const BitBoard FileA;
extern const BitBoard FileB;
extern const BitBoard FileC;
extern const BitBoard FileD;
extern const BitBoard FileE;
extern const BitBoard FileF;
extern const BitBoard FileG;
extern const BitBoard FileH;

extern const BitBoard Rank1;
extern const BitBoard Rank2;
extern const BitBoard Rank3;
extern const BitBoard Rank4;
extern const BitBoard Rank5;
extern const BitBoard Rank6;
extern const BitBoard Rank7;
extern const BitBoard Rank8;

extern const BitBoard Edges;

extern BitBoard *RookMoveDatabase[64];
extern BitBoard *BishopMoveDatabase[64];
extern BitBoard AttackRay[8][64];

struct	BOARD{
	BitBoard	Pieces[13];
	BitBoard	CurrentBoard[2];
	uint8_t		Sq[BRD_SQR];
	uint8_t		Castling_check;
	/* 0000 0000 (Q+K)check * (B+W) is possible
	 * 1000 0000 BQ check is not possible
	 * 0100 0000 BK check is not possible
	 * 0010 0000 Already castle 
	 */
	uint8_t		No_Ply;
	uint8_t		Side_to_move;
	Move		PreviousMove;
	Move		PrevMove[80];
};

int		population	( BitBoard A );

bool 	isSubsetOf	( BitBoard a, BitBoard b);

int		BitPop		( BitBoard &A );

int 	LSBit		( BitBoard A );

int 	MSBit		( BitBoard A ); 

int		BitPopR		( BitBoard &A );

int 	PopsCount	( BitBoard C );

void 	PrintBitBoard 	( BitBoard A );

void	PrintSQ			( uint8_t SQ[]);

void 	GenerateMoveData ();

void 	GenerateMask	();

void	READ_FEN	(std::string FEN_STRING, BOARD *A);

std::string		toFEN		(BOARD A);

void	getBoardInfo(BOARD A);

#endif
