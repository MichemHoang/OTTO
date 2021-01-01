#include "zobristhash.h"
#include "movegen.h"

BitString	Random[13][64];
BitString	CastKW;
BitString	CastQW;
BitString	CastKB;
BitString	CastQB;
BitString	BTurn;

unsigned long long init[4]={0x12345ULL, 0x23456ULL, 0x34567ULL, 0x45678ULL}, length=4;

BitString Xoring	(BitString String, BitString Xor){
	return String ^ Xor;
}
 
BitString	RandomGenerator(){
	init_by_array64(init, length);
	BitString	a	=	genrand64_int64();
	return a;
}

BitString	UpdateKey(BitString OldKey, Move A, BOARD OldBoard){
	uint8_t	from	=	A & 0x3f;
	uint8_t to		=	A >> 6 & 0x3f;
	uint8_t flag	=	A >> 12 & 0x0f;
	
	OldKey		^=	Random[OldBoard.Sq[from]][from];	
	OldKey		^=	Random[OldBoard.Sq[to]][to];		
	OldKey		^=	Random[OldBoard.Sq[from]][to];		
	
	if (flag	==	KING_CASTLE) {
		if (OldBoard.Side_to_move	==	WHITE)	{
			OldKey	^=	CastKW;
			OldKey	^=	Random[wR][63];
			OldKey	^=	Random[wR][61];
		} else {
			OldKey	^=	CastKB;
			OldKey	^=	Random[bR][7];
			OldKey	^=	Random[bR][5];
		}
	}
	if (flag	==	QUEEN_CASTLE) {
		if (OldBoard.Side_to_move	==	WHITE)	{
			OldKey	^=	CastQW;
			OldKey	^=	Random[wR][56];
			OldKey	^=	Random[wR][59];
		} else {
			OldKey	^=	CastQB;
			OldKey	^=	Random[bR][0];
			OldKey	^=	Random[bR][3];
		}
	}
	OldKey	^=	BTurn;
	return OldKey;
}

BitString	GetKey(BOARD A){
	uint64_t	ZKey	=	0;
	for (int i	=	0; i < 64; i++)
		if (A.Sq[i]	!= 12)	ZKey	^=	Random[A.Sq[i]][i];
	if ((A.Castling_check	&	CastlingKW)	== CastlingKW ) {	ZKey	^=	CastKW; std::cout	<< ""	;}
	if ((A.Castling_check	&	CastlingQW)	== CastlingQW ) {	ZKey	^=	CastQW;	std::cout	<< ""	;}
	if ((A.Castling_check	&	CastlingKB)	== CastlingKB ) {	ZKey	^=	CastKB;	std::cout	<< ""	;}
	if ((A.Castling_check	&	CastlingQB)	== CastlingQB ) {	ZKey	^=	CastQB;	std::cout	<< ""	;}
	if (A.Side_to_move	==	BLACK)	ZKey	^= BTurn;
	return ZKey;
}

void		InitZoBrist(bool On){
	std::cout	<<	"INIT ZOBRIST RANDOM MATRIX\n";
	if (On) init_by_array64(init, length);
	for (int i = 0; i < 12; i++)
		for (int j = 0; j < 64; j++)	
			Random[i][j]		=	genrand64_int64();
	for (int j = 0; j < 64; j++)	
			Random[12][j]		=	0;
	CastKW	=	genrand64_int64();
	CastQW	=	genrand64_int64();
	CastKB	=	genrand64_int64();
	CastKW	=	genrand64_int64();
	BTurn	=	genrand64_int64();
}
