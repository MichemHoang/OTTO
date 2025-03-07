#ifndef BITBOARD_H_
#define BITBOARD_H_

#include "typedefs.h"
#include <stdint.h>

/*
 * BitBoard manipulation
 * Bit level manipulation
 * Important BitBoard value
 */
 
namespace MASK{
extern BitBoard RMask[64];
extern BitBoard NMask[64];
extern BitBoard KMask[64];
extern BitBoard BMask[64];
extern BitBoard	PMask[2][64];
}

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

const std::string squares[64] = {
	"A8", "B8", "C8", "D8", "E8", "F8", "G8", "H8",
	"A7", "B7", "C7", "D7", "E7", "F7", "G7", "H7",
	"A6", "B6", "C6", "D6", "E6", "F6", "G6", "H6",
	"A5", "B5", "C5", "D5", "E5", "F5", "G5", "H5",
	"A4", "B4", "C4", "D4", "E4", "F4", "G4", "H4",
	"A3", "B3", "C3", "D3", "E3", "F3", "G3", "H3",
	"A2", "B2", "C2", "D2", "E2", "F2", "G2", "H2",
	"A1", "B1", "C1", "D1", "E1", "F1", "G1", "H1"
};

struct BOARD{
	BitBoard	Pieces[13];
	BitBoard	CurrentBoard[2];
    uint8_t		Sq[64];

	/* 0000 0000 (Q+K)check * (B+W) is possible
	 * 1000 0000 BQ check is not possible
	 * 0100 0000 BK check is not possible
	 * 0010 0000 Already castle 
	 */
	uint8_t		Castling_check;
    uint8_t		No_Ply; //number of Ply
	uint8_t		Side_to_move;
	Move		PreviousMove;
};

//Bit Operation
namespace BitOp{
bool 	isSubsetOf		( BitBoard a, BitBoard b );
int		BitPop			( BitBoard &A );
int 	LSBit			( BitBoard A );
int 	MSBit			( BitBoard A ); 
int		BitPopR			( BitBoard &A );
int 	PopsCount		( BitBoard C );
void 	PrintBitBoard 	( BitBoard A );
void	PrintSQ			( uint8_t SQ[] );
void	getBoardInfo	( BOARD A );
}

namespace INITIALIZE{
void 	MoveData ();
void 	Mask	();
}

namespace FEN_Op{
void	READ_FEN(std::string FEN_STRING, BOARD *A);
std::string		toFEN(BOARD A);
}

#endif
