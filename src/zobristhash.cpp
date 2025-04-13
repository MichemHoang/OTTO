#include "zobristhash.h"
#include "movegen.h"

BitString Random[13][64];
BitString CastKW;
BitString CastQW;
BitString CastKB;
BitString CastQB;
BitString BTurn;

unsigned long long init[4]={0x12345ULL, 0x23456ULL, 0x34567ULL, 0x45678ULL}, length=4;

BitString Xoring (BitString String, BitString Xor){
	return String ^ Xor;
}
 
BitString RandomGenerator(){
	init_by_array64(init, length);
	BitString a = genrand64_int64();
	return a;
}

BitString UpdateKey(BitString oldKey, Move A, BOARD oldBoard){
	uint8_t	from = A & 0x3f;
	uint8_t to = A >> 6 & 0x3f;
	uint8_t flag = A >> 12 & 0x0f;
	
	oldKey ^= Random[oldBoard.Sq[from]][from];	
	oldKey ^= Random[oldBoard.Sq[to]][to];		
	oldKey ^= Random[oldBoard.Sq[from]][to];		
	
	if (flag == KING_CASTLE) {
		if (oldBoard.Side_to_move == WHITE)	{
			oldKey	^=	CastKW;
			oldKey	^=	Random[wR][63];
			oldKey	^=	Random[wR][61];
		} else {
			oldKey	^=	CastKB;
			oldKey	^=	Random[bR][7];
			oldKey	^=	Random[bR][5];
		}
	}
	if (flag == QUEEN_CASTLE) {
		if (oldBoard.Side_to_move	==	WHITE)	{
			oldKey	^=	CastQW;
			oldKey	^=	Random[wR][56];
			oldKey	^=	Random[wR][59];
		} else {
			oldKey	^=	CastQB;
			oldKey	^=	Random[bR][0];
			oldKey	^=	Random[bR][3];
		}
	}
	oldKey ^= BTurn;
	return oldKey;
}

BitString UpdateKey(BitString oldKey, Move newMove, BOARD_C oldBoard){
	uint8_t	from = newMove & 0x3f;
	uint8_t to = newMove >> 6 & 0x3f;
	uint8_t flag = newMove >> 12 & 0x0f;
	
	oldKey ^= Random[oldBoard.Sq[from]][from];	
	oldKey ^= Random[oldBoard.Sq[to]][to];		
	oldKey ^= Random[oldBoard.Sq[from]][to];		
	
	if (flag == KING_CASTLE) {
		if (oldBoard.Side_to_move == WHITE)	{
			oldKey	^=	CastKW;
			oldKey	^=	Random[wR][63];
			oldKey	^=	Random[wR][61];
		} else {
			oldKey	^=	CastKB;
			oldKey	^=	Random[bR][7];
			oldKey	^=	Random[bR][5];
		}
	}
	if (flag == QUEEN_CASTLE) {
		if (oldBoard.Side_to_move	==	WHITE)	{
			oldKey	^=	CastQW;
			oldKey	^=	Random[wR][56];
			oldKey	^=	Random[wR][59];
		} else {
			oldKey	^=	CastQB;
			oldKey	^=	Random[bR][0];
			oldKey	^=	Random[bR][3];
		}
	}
	oldKey ^= BTurn;
	return oldKey;
}

BitString GetKey(BOARD A){
	uint64_t ZKey =	0;
	for (int i	= 0; i < 64; i++)
		if (A.Sq[i]	!= 12)	ZKey ^= Random[A.Sq[i]][i];
	if ((A.Castling_check & CastlingKW)	== CastlingKW ) { ZKey ^= CastKW;}
	if ((A.Castling_check & CastlingQW)	== CastlingQW ) { ZKey ^= CastQW;}
	if ((A.Castling_check & CastlingKB)	== CastlingKB ) { ZKey ^= CastKB;}
	if ((A.Castling_check & CastlingQB)	== CastlingQB ) { ZKey ^= CastQB;}
	if (A.Side_to_move	==	BLACK)	ZKey ^= BTurn;
	return ZKey;
}

BitString GetKeyString(BOARD_C board){
	uint64_t ZKey = 0;
	for (int i = 0; i < 64; i++)
		if (board.Sq[i]	!= 12)	ZKey ^= Random[board.Sq[i]][i];
	if ((board.Castling_check & CastlingKW)	== CastlingKW ) { ZKey ^= CastKW;}
	if ((board.Castling_check & CastlingQW)	== CastlingQW ) { ZKey ^= CastQW;}
	if ((board.Castling_check & CastlingKB)	== CastlingKB ) { ZKey ^= CastKB;}
	if ((board.Castling_check & CastlingQB)	== CastlingQB ) { ZKey ^= CastQB;}
	if (board.Side_to_move	==	BLACK)	ZKey ^= BTurn;
	return ZKey;
}

void InitZoBrist(bool On){
	std::cout << "INIT ZOBRIST RANDOM MATRIX\n";
	if (On) init_by_array64(init, length);
	for (int i = 0; i < 12; i++)
		for (int j = 0; j < 64; j++)	
			Random[i][j] = genrand64_int64();
	for (int j = 0; j < 64; j++)	
		Random[12][j] = 0;
	CastKW	= genrand64_int64();
	CastQW	= genrand64_int64();
	CastKB	= genrand64_int64();
	CastKW	= genrand64_int64();
	BTurn	= genrand64_int64();
}
