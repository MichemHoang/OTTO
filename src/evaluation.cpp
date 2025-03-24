#include "evaluation.h"

short PawnTable[64] =	{
  0,  0,  0,  0,  0,  0,  0,  0,
 50, 50, 50, 50, 50, 50, 50, 50,
 10, 10, 20, 30, 30, 20, 10, 10,
  5,  5, 10, 35, 35, 10,  5,  5,
  0,  0,  0, 33, 33,  0,  0,  0,
  5, -5,-10,  0,  0,-10, -5,  5,
  5, 10, 10,-25,-25, 10, 10,  5,
  0,  0,  0,  0,  0,  0,  0,  0, 
};

short KnightTable[64] =	{
 -50,-40,-30,-30,-30,-30,-40,-50,
 -40,-20,  0,  0,  0,  0,-20,-40,
 -30,  0, 10, 15, 15, 10,  0,-30,
 -30,  5, 15, 20, 20, 15,  5,-30,
 -30,  0, 15, 20, 20, 15,  0,-30,
 -30,  5, 10, 15, 15, 10,  5,-30,
 -40,-20,  0,  5,  5,  0,-20,-40,
 -50,-40,-20,-30,-30,-20,-40,-50, 
};

short BishopTable[64] =	{
 -20,-10,-10,-10,-10,-10,-10,-20,
 -10,  0,  0,  0,  0,  0,  0,-10,
 -10,  0,  5, 10, 10,  5,  0,-10,
 -10, 10,  5, 10, 10,  5,  5,-10,
 -10,  0, 10, 10, 10, 10,  0,-10,
 -10, 10, 10, 10, 10, 10, 10,-10,
 -10, 10,  0,  0,  0,  0, 10,-10,
 -20,-10,-40,-10,-10,-40,-10,-20, 
};

short KingTable[64] =	{
  -30, -40, -40, -50, -50, -40, -40, -30,
  -30, -40, -40, -50, -50, -40, -40, -30,
  -30, -40, -40, -50, -50, -40, -40, -30,
  -30, -40, -40, -50, -50, -40, -40, -30,
  -20, -30, -30, -40, -40, -30, -30, -20,
  -10, -20, -20, -20, -20, -20, -20, -10, 
   20,  20,   0,   0,   0,   0,  20,  20,
   20,  30,  10,   0,   0,  10,  30,  20, 
};

short RookTable[64]	=	{
	0,	0,	0,	0,	0,	0,	0,	0,
	0,	0,	0,	0,	0,	0,	0,	0,
	0,	0,	0,	0,	0,	0,	0,	0,
	0,	0,	0,	0,	0,	0,	0,	0,
	0,	0,	0,	0,	0,	0,	0,	0,
	0,	0,	0,	0,	0,	0,	0,	0,
	0,	0,	0,	0,	0,	0,	0,	0,
  -15,	0,	0,	0,	0,	0,	0,-15,
};

short QueenTable[64]=	{
	0,	0,	0,	0,	0,	0,	0,	0,
	0,	0,	0,	0,	0,	0,	0,	0,
	0,	0,-10,-10,-10,-10,	0,	0,
	0,	0,-10,-15,-15,-10,	0,	0,
	0,	0,-10,-15,-15,-10,	0,	0,
	0,	0,-10,-10,-10,-10,	0,	0,
	0,	0,	0,	0,	0,	0,	0,	0,
	0,	0,	0,	0,	0,	0,	0,	0,
};

BitBoard CenterBonus	=	0x3C3C3C3C0000;

int RayLookUp[8]	=	{1,3,5,7,0,2,4,6};
BitBoard FILES[8]	=	{FileA, FileB, FileC, FileD, FileE, FileF, FileG, FileH};
BitBoard RANKS[8]	=	{Rank1, Rank2, Rank3, Rank4, Rank5, Rank6, Rank7, Rank8};

//return relative score for a move based on destination
//Favors pawn pushing and discourage Queen, King and Rook movement

namespace EVALUATION{
int	PieceSquareValue(int isPawn, uint16_t from, uint16_t to){
	switch (isPawn){
		case wP: return (PawnTable[to] 		- 	PawnTable[from])/1.5 + 10;
		case wN: return (KnightTable[to] 	- 	KnightTable[from])/1.5;
		case wB: return (BishopTable[to] 	- 	BishopTable[from])/1.5;
		case wR: return (RookTable[to] 		- 	RookTable[from])/1.5 - 40;
		case wQ: return (QueenTable[to] 	- 	QueenTable[from])/1.5 - 40;		
		case wK: return (KingTable[to] 		- 	KingTable[from])/1.5 -50;
		
		case bP: return (PawnTable[63-to] 		- 	PawnTable[63-from])/1.5 + 10;
		case bN: return (KnightTable[63-to] 	- 	KnightTable[63-from])/1.5;
		case bB: return (BishopTable[63-to] 	- 	BishopTable[63-from])/1.5;
		case bR: return (RookTable[63-to] 		- 	RookTable[63-from])/1.5 - 40;
		case bQ: return (QueenTable[63-to] 		- 	QueenTable[63-from])/1.5 - 40;		
		case bK: return (KingTable[63-to] 		- 	KingTable[63-from])/1.5 -50;
	}
	return 0;
}

/*
 * Evaluation function implements
 * 		+ Material balance
 * 		+ Center control	
 * 		+ Castle receive Bonus
 * 		+ Pawn structure
 * 		+ King safety
 * 		+ BIshop pair
 */

int	Evaluate (BOARD A, int side){
	int MATERIAL_BALANCE	=	0;
	int	PAWN_STRUCTURE		=	0;
	int	CENTER_CONTROL		=	0;
	int	MOBILITY			=	0;
	int CASTLING			=	0;
	int KING_SAFETY			=	0;
	int PSQR_VALUE			=	0;
	int BLK_PWN				=	0;
	int	B_PAIR				=	0;
	int	TOTAL_SCORE			=	0;
	BitBoard	Bishop[2];
	BitBoard	Knight[2];
	BitBoard	Pawn[2];
	//MATERIAL BALANCE
	for (int i = 0; i < 6; i++)	MATERIAL_BALANCE	+=	(BitOp::PopsCount (  A.Pieces[i] ) - BitOp::PopsCount ( A.Pieces[i + 6] )) * VALUE[i];
	
	//PAWN FORMATION
	Pawn[0]	=	A.Pieces[wP]; Pawn[1]	=	A.Pieces[bP];
	for ( int i = 0; i < 2; i++)
		while (Pawn[i] != 0 )	if ((MASK::PMask[i][BitOp::BitPop(Pawn[i])]	& A.Pieces[i*6]) != 0 ) PAWN_STRUCTURE	+=	25*(1 - 2*i);
	CENTER_CONTROL	=	40*(BitOp::PopsCount(A.CurrentBoard[WHITE] & CenterBonus) - BitOp::PopsCount(A.CurrentBoard[BLACK] & CenterBonus));
	
	//CASTLING
	if ((A.Castling_check & ( 0xE << 4  )) == (0xE << 4  ))	CASTLING -= 50 ;
	if ((A.Castling_check &  0xE ) == 0xE ) 				CASTLING += 50 ;
	if (A.No_Ply < 15) CASTLING	*=	2;
	
	//BISHOP_PAIR
	Bishop[0]	=	A.Pieces[wB];	Bishop[1]	=	A.Pieces[bB];
	Knight[0]	=	A.Pieces[wN];	Knight[1]	=	A.Pieces[bN];
	if (BitOp::PopsCount(Bishop[0])	==	2) B_PAIR += 15;
	if (BitOp::PopsCount(Bishop[1])	==	2) B_PAIR -= 15;
	
	//DOUBLE PAWN
	Pawn[0]	=	A.Pieces[0]; Pawn[1]	=	A.Pieces[6];
	for (int file = 0; file < 8; file++){
		if (BitOp::PopsCount(FILES[file]	& Pawn[0]) > 1)	BLK_PWN -= 	15;
		if (BitOp::PopsCount(FILES[file]	& Pawn[1]) > 1)	BLK_PWN	+=	15;
	}
	side	==	BLACK ? BLK_PWN *= -1 : BLK_PWN *= 	1;
	
	//PIECE SQR VALUE 
	Pawn[0]	=	A.Pieces[wP]; Pawn[1]	=	A.Pieces[bP];
	while (Pawn[0] != 0)	PSQR_VALUE	+=	PawnTable[BitOp::BitPop(Pawn[0])];
	while (Pawn[1] != 0)	PSQR_VALUE	-=	PawnTable[63 - BitOp::BitPop(Pawn[1])];
	while (Bishop[0] != 0)	PSQR_VALUE	+=	BishopTable[BitOp::BitPop(Bishop[0])];
	while (Bishop[1] != 0)	PSQR_VALUE	-=	BishopTable[63 - BitOp::BitPop(Bishop[1])];
	while (Knight[0] != 0)	PSQR_VALUE	+=	KnightTable[BitOp::BitPop(Knight[0])];
	while (Knight[1] != 0)	PSQR_VALUE	-=	KnightTable[63 - BitOp::BitPop(Knight[1])];
	
	if (A.No_Ply >= 15) PSQR_VALUE	*= 0.2;
	else   				PSQR_VALUE	*= 0.3;
	
	//KING SAFETY
	Pawn[0]	=	A.Pieces[wP]; Pawn[1]	=	A.Pieces[bP];
	int a = 0, b = 0, d, e;
	int KingPos	=	BitOp::LSBit(A.Pieces[wK]);
	if ((AttackRay[2][KingPos] & Pawn[0]) != 0) a += 90;
	if (KingPos%8 != 7) {if ((AttackRay[2][KingPos + 1] & Pawn[0]) != 0) a += 60;}	else a += 40;
	if (KingPos%8 != 0) {if ((AttackRay[2][KingPos - 1] & Pawn[0]) != 0) a += 60;}	else a += 40;
	if (A.Pieces[bQ]	==	0) a	*= 0.8;
	d =  BitOp::PopsCount(MASK::KMask[KingPos] & A.CurrentBoard[0]) * 30;
	
	KingPos	=	BitOp::LSBit(A.Pieces[bK]);
	if ((AttackRay[6][KingPos] & Pawn[1]) != 0) b -= 90;
	if (KingPos%8 != 7) {if ((AttackRay[6][KingPos + 1] & Pawn[1]) != 0) b -= 60;}	else b -= 40;
	if (KingPos%8 != 0) {if ((AttackRay[6][KingPos - 1] & Pawn[1]) != 0) b -= 60;}	else b -= 40;
	if (A.Pieces[wQ]	==	0) b	*= 0.8;
	e =  BitOp::PopsCount(MASK::KMask[KingPos] & A.CurrentBoard[1]) * -30;
	
	KING_SAFETY	=	a + b;
	if (A.No_Ply > 14) KING_SAFETY+= (d+e);
	if (A.No_Ply < 12) KING_SAFETY*= 0.3;
	if (A.No_Ply > 70) KING_SAFETY*= 0.7;
	
	//ROOK_ON_OPEN_FILE
	if (A.No_Ply > 30){
		int RookPos		=	BitOp::LSBit(A.Pieces[wR]);
		BitBoard Mobi	=	GENERATE::Rook(RookPos, A.CurrentBoard[WHITE], A.CurrentBoard[BLACK]);
		if ((Mobi & FILES[RookPos%8] & A.Pieces[wP] & A.Pieces[bP]) == 0) 	MOBILITY	+= 50;	
		else if ((Mobi & FILES[RookPos%8] & A.Pieces[wP]) == 0)				MOBILITY	+= 25;	
		
		RookPos	=	BitOp::LSBit(A.Pieces[bR]);
		Mobi	=	GENERATE::Rook(RookPos, A.CurrentBoard[BLACK], A.CurrentBoard[WHITE]);
		if ((Mobi & FILES[RookPos%8] & A.Pieces[wP] & A.Pieces[bP]) == 0) 	MOBILITY	-= 50;	
		else if ((Mobi & FILES[RookPos%8] & A.Pieces[bP]) == 0)				MOBILITY	-= 25;	
	}
	TOTAL_SCORE	=	MATERIAL_BALANCE + PAWN_STRUCTURE + CENTER_CONTROL + 
					MOBILITY + CASTLING + KING_SAFETY + PSQR_VALUE + BLK_PWN;
	side	==	BLACK ? TOTAL_SCORE *= -1 : TOTAL_SCORE *= 	1;
	return TOTAL_SCORE;
}

int	Evaluate (BOARD_C A, int side){
	int MATERIAL_BALANCE	=	0;
	int	PAWN_STRUCTURE		=	0;
	int	CENTER_CONTROL		=	0;
	int	MOBILITY			=	0;
	int CASTLING			=	0;
	int KING_SAFETY			=	0;
	int PSQR_VALUE			=	0;
	int BLK_PWN				=	0;
	int	B_PAIR				=	0;
	int	TOTAL_SCORE			=	0;
	BitBoard	Bishop[2];
	BitBoard	Knight[2];
	BitBoard	Pawn[2];
	//MATERIAL BALANCE
	for (int i = 0; i < 6; i++)	MATERIAL_BALANCE	+=	(BitOp::PopsCount (  A.Pieces[i] ) - BitOp::PopsCount ( A.Pieces[i + 6] )) * VALUE[i];
	
	//PAWN FORMATION
	Pawn[0]	=	A.Pieces[wP]; Pawn[1]	=	A.Pieces[bP];
	for ( int i = 0; i < 2; i++)
		while (Pawn[i] != 0 )	if ((MASK::PMask[i][BitOp::BitPop(Pawn[i])]	& A.Pieces[i*6]) != 0 ) PAWN_STRUCTURE	+=	25*(1 - 2*i);
	CENTER_CONTROL	=	40*(BitOp::PopsCount(A.CurrentBoard[WHITE] & CenterBonus) - BitOp::PopsCount(A.CurrentBoard[BLACK] & CenterBonus));
	
	//CASTLING
	if ((A.Castling_check & ( 0xE << 4  )) == (0xE << 4  ))	CASTLING -= 50 ;
	if ((A.Castling_check &  0xE ) == 0xE ) 				CASTLING += 50 ;
	if (A.No_Ply < 15) CASTLING	*=	2;
	
	//BISHOP_PAIR
	Bishop[0]	=	A.Pieces[wB];	Bishop[1]	=	A.Pieces[bB];
	Knight[0]	=	A.Pieces[wN];	Knight[1]	=	A.Pieces[bN];
	if (BitOp::PopsCount(Bishop[0])	==	2) B_PAIR += 15;
	if (BitOp::PopsCount(Bishop[1])	==	2) B_PAIR -= 15;
	
	//DOUBLE PAWN
	Pawn[0]	=	A.Pieces[0]; Pawn[1]	=	A.Pieces[6];
	for (int file = 0; file < 8; file++){
		if (BitOp::PopsCount(FILES[file]	& Pawn[0]) > 1)	BLK_PWN -= 	15;
		if (BitOp::PopsCount(FILES[file]	& Pawn[1]) > 1)	BLK_PWN	+=	15;
	}
	side	==	BLACK ? BLK_PWN *= -1 : BLK_PWN *= 	1;
	
	//PIECE SQR VALUE 
	Pawn[0]	=	A.Pieces[wP]; Pawn[1]	=	A.Pieces[bP];
	while (Pawn[0] != 0)	PSQR_VALUE	+=	PawnTable[BitOp::BitPop(Pawn[0])];
	while (Pawn[1] != 0)	PSQR_VALUE	-=	PawnTable[63 - BitOp::BitPop(Pawn[1])];
	while (Bishop[0] != 0)	PSQR_VALUE	+=	BishopTable[BitOp::BitPop(Bishop[0])];
	while (Bishop[1] != 0)	PSQR_VALUE	-=	BishopTable[63 - BitOp::BitPop(Bishop[1])];
	while (Knight[0] != 0)	PSQR_VALUE	+=	KnightTable[BitOp::BitPop(Knight[0])];
	while (Knight[1] != 0)	PSQR_VALUE	-=	KnightTable[63 - BitOp::BitPop(Knight[1])];
	
	if (A.No_Ply >= 15) PSQR_VALUE	*= 0.2;
	else   				PSQR_VALUE	*= 0.3;
	
	//KING SAFETY
	Pawn[0]	=	A.Pieces[wP]; Pawn[1]	=	A.Pieces[bP];
	int a = 0, b = 0, d, e;
	int KingPos	=	BitOp::LSBit(A.Pieces[wK]);
	if ((AttackRay[2][KingPos] & Pawn[0]) != 0) a += 90;
	if (KingPos%8 != 7) {if ((AttackRay[2][KingPos + 1] & Pawn[0]) != 0) a += 60;}	else a += 40;
	if (KingPos%8 != 0) {if ((AttackRay[2][KingPos - 1] & Pawn[0]) != 0) a += 60;}	else a += 40;
	if (A.Pieces[bQ]	==	0) a	*= 0.8;
	d =  BitOp::PopsCount(MASK::KMask[KingPos] & A.CurrentBoard[0]) * 30;
	
	KingPos	=	BitOp::LSBit(A.Pieces[bK]);
	if ((AttackRay[6][KingPos] & Pawn[1]) != 0) b -= 90;
	if (KingPos%8 != 7) {if ((AttackRay[6][KingPos + 1] & Pawn[1]) != 0) b -= 60;}	else b -= 40;
	if (KingPos%8 != 0) {if ((AttackRay[6][KingPos - 1] & Pawn[1]) != 0) b -= 60;}	else b -= 40;
	if (A.Pieces[wQ]	==	0) b	*= 0.8;
	e =  BitOp::PopsCount(MASK::KMask[KingPos] & A.CurrentBoard[1]) * -30;
	
	KING_SAFETY	=	a + b;
	if (A.No_Ply > 14) KING_SAFETY+= (d+e);
	if (A.No_Ply < 12) KING_SAFETY*= 0.3;
	if (A.No_Ply > 70) KING_SAFETY*= 0.7;
	
	//ROOK_ON_OPEN_FILE
	if (A.No_Ply > 30){
		int RookPos		=	BitOp::LSBit(A.Pieces[wR]);
		BitBoard Mobi	=	GENERATE::Rook(RookPos, A.CurrentBoard[WHITE], A.CurrentBoard[BLACK]);
		if ((Mobi & FILES[RookPos%8] & A.Pieces[wP] & A.Pieces[bP]) == 0) 	MOBILITY	+= 50;	
		else if ((Mobi & FILES[RookPos%8] & A.Pieces[wP]) == 0)				MOBILITY	+= 25;	
		
		RookPos	=	BitOp::LSBit(A.Pieces[bR]);
		Mobi	=	GENERATE::Rook(RookPos, A.CurrentBoard[BLACK], A.CurrentBoard[WHITE]);
		if ((Mobi & FILES[RookPos%8] & A.Pieces[wP] & A.Pieces[bP]) == 0) 	MOBILITY	-= 50;	
		else if ((Mobi & FILES[RookPos%8] & A.Pieces[bP]) == 0)				MOBILITY	-= 25;	
	}
	TOTAL_SCORE	=	MATERIAL_BALANCE + PAWN_STRUCTURE + CENTER_CONTROL + 
					MOBILITY + CASTLING + KING_SAFETY + PSQR_VALUE + BLK_PWN;
	side	==	BLACK ? TOTAL_SCORE *= -1 : TOTAL_SCORE *= 	1;
	return TOTAL_SCORE;
}

 //Least valuable attacker
 //-1 = there is no attacker on the square
int LVA (int att_Sqr, BOARD_C A, int side){
	int	QPosition = -1;
	BitBoard SlidingPiece;
	BitBoard B;
	int position = -1;
	//Check for pawn attack
	B	=	MASK::PMask[side^1][att_Sqr] & A.Pieces[wP + 6*(side)];
	while (B!=0){
		position = BitOp::BitPop(B);
		return position;
	}
	//Check for Knight attack
	B	=	MASK::NMask[att_Sqr] & A.Pieces[wN + 6 * side];
	while (B!=0){
		position = BitOp::BitPop(B);
		return position;
	}
	SlidingPiece = GENERATE::Queen(att_Sqr, A.CurrentBoard[side ^ 1] | BIT1 >> att_Sqr, A.CurrentBoard[side]);
	//Check for rook/Bishop/Queen attack
	for (int i = 0; i < 8; i++){
		B	=	AttackRay[RayLookUp[i]][att_Sqr] & SlidingPiece ;
		while (B!=0){
			if (i < 4)	{
				RayLookUp[i]	<	4? position	=	BitOp::BitPopR(B) : position	=	BitOp::BitPop(B);
				if (A.Sq[position]	==	(wB + 6 * side ))	return position;
				if (A.Sq[position]	==	(wQ + 6 * side )) 	QPosition	=	position;
			}	else {
				RayLookUp[i]	<	4? position	=	BitOp::BitPopR(B) : position	=	BitOp::BitPop(B);
				if (A.Sq[position]	==	(wR + 6 * side ))	return position;
				if (A.Sq[position]	==	(wQ + 6 * side )) 	QPosition	=	position;
			}
		}
	}
	if (QPosition	!=	-1) return QPosition;
	B	=	MASK::KMask[att_Sqr]	&	A.Pieces[wK	+	6 * side];
	position	=	BitOp::BitPop(B);
	return position;
}

//LEAST VALUABLE ATTACKER
int LVA(int att_Sqr, BOARD A, int side){
	int	QPosition	=	-1;
	BitBoard SlidingPiece;
	BitBoard B;
	int position	=	-1;
	//Check for pawn attack
	B = MASK::PMask[side^1][att_Sqr] & A.Pieces[wP + 6*(side)];
	while (B!=0){
		position = BitOp::BitPop(B);
		return position;
	}
	
	//Check for Knight attack
	B	=	MASK::NMask[att_Sqr] & A.Pieces[wN	+	6 * side];
	while (B!=0){
		position = BitOp::BitPop(B);
		return position;
	}
	
	SlidingPiece = GENERATE::Queen(att_Sqr, A.CurrentBoard[side ^ 1] | BIT1 >> att_Sqr, A.CurrentBoard[side]);
	//Check for rook/Bishop/Queen attack
	for (int i = 0; i < 8; i++){
		B	=	AttackRay[RayLookUp[i]][att_Sqr] & SlidingPiece ;
		while (B!=0){
			if (i < 4)	{
				RayLookUp[i]	<	4? position	=	BitOp::BitPopR(B) : position	=	BitOp::BitPop(B);
				if (A.Sq[position]	==	(wB + 6 * side ))	return position;
				if (A.Sq[position]	==	(wQ + 6 * side )) 	QPosition	=	position;
			}	else {
				RayLookUp[i]	<	4? position	=	BitOp::BitPopR(B) : position	=	BitOp::BitPop(B);
				if (A.Sq[position]	==	(wR + 6 * side ))	return position;
				if (A.Sq[position]	==	(wQ + 6 * side )) 	QPosition	=	position;
			}
		}
	}
	if (QPosition	!=	-1) return QPosition;
	B	=	MASK::KMask[att_Sqr]	&	A.Pieces[wK	+	6 * side];
	position = BitOp::BitPop(B);
	return position;
}

//STATIC EXCHANGE EVALUATION
int	SEEA(int To, BOARD A, int from){
	int gain[32]; 
	int	depth	=	0;
	int	Attacker;
	gain[0]		=	VALUE[A.Sq[To]];			//(Initial value)
	A.Pieces[A.Sq[from]]				^=	 ( ( BIT1 >> from ) | (BIT1 >> To) );
	A.Pieces[A.Sq[To]]					^=	 ( BIT1 >> To );
	A.CurrentBoard[A.Side_to_move]		^=	 ( ( BIT1 >> from ) | (BIT1 >> To) );
	A.CurrentBoard[A.Side_to_move ^ 1]	&=	 ~( BIT1 >> To );
	A.Sq[To]		=	A.Sq[from];
	A.Sq[from]	=	12;
	A.Side_to_move	^=	1;
	do {
		Attacker		=	LVA(To, A, A.Side_to_move);
		A.Side_to_move	^=	1;
		if (Attacker == -1) break;
		depth++;
		gain[depth]		=	VALUE[A.Sq[To]]	-	gain[depth-1];
		if (-gain[depth-1] < 0 && gain[depth] < 0)	break;
		A.Pieces[A.Sq[Attacker]]				^=	 ( ( BIT1 >> Attacker ) | (BIT1 >> To) );
		A.Pieces[A.Sq[To]]						^=	 ( BIT1 >> To );
		A.CurrentBoard[A.Side_to_move]			^=	 ( ( BIT1 >> Attacker ) | (BIT1 >> To) );
		A.CurrentBoard[A.Side_to_move ^ 1]		&=	 ~( BIT1 >> To );
		A.Sq[To]		=	A.Sq[Attacker];
		A.Sq[Attacker]	=	12;
	} while (1);
	while (depth > 0)	{
		if (-gain[depth-1] <= gain[depth])  gain[depth-1]	=	-gain[depth];
		depth--;
	}
	return gain[0];
}

//potential for parallel
//SEE(Static exchange evaluation) - The Swap Algorithm 
int	SEEA(int To, BOARD_C A, int from){
	int gain[32]; 
	int	depth = 0;
	int	Attacker;
	BitBoard bboardTo = (BIT1 >> To) ;
	gain[0] = VALUE[A.Sq[To]];			//(Initial value)
	A.Pieces[A.Sq[from]]				^=	 ( ( BIT1 >> from ) | (bboardTo) );
	A.Pieces[A.Sq[To]]					^=	 ( bboardTo );
	A.CurrentBoard[A.Side_to_move]		^=	 ( ( BIT1 >> from ) | (bboardTo) );
	A.CurrentBoard[A.Side_to_move ^ 1]	&=	 ~( bboardTo );
	A.Sq[To] = A.Sq[from];
	A.Sq[from]	=	12;
	A.Side_to_move	^=	1;
	do {
		Attacker = LVA(To, A, A.Side_to_move);
		A.Side_to_move	^=	1;
		//if (VALUE[A.Sq[to]] - VALUE[A.Sq[Attacker]] > 150 ) return -150;
		if (Attacker == -1) break;
		depth++;
		gain[depth]	= VALUE[A.Sq[To]] - gain[depth-1];
		if (-gain[depth-1] < 0 && gain[depth] < 0)	break;
		A.Pieces[A.Sq[Attacker]]				^=	 ( ( BIT1 >> Attacker ) | (bboardTo) );
		A.Pieces[A.Sq[To]]						^=	 ( bboardTo );
		A.CurrentBoard[A.Side_to_move]			^=	 ( ( BIT1 >> Attacker ) | (bboardTo) );
		A.CurrentBoard[A.Side_to_move ^ 1]		&=	 ~( bboardTo );
		A.Sq[To]		=	A.Sq[Attacker];
		A.Sq[Attacker]	=	12; //12 mean empty sqr i think
	} while (1);
	while (depth > 0)	{
		if (-gain[depth-1] <= gain[depth]) gain[depth-1] = -gain[depth];
		depth--;
	}
	return gain[0];
}

int	DynamicEval	(BOARD A, int Alpha, int Beta){
    ExtMove CaptureList[20];
    ExtMove *iter	=	CaptureList;
    int evaluation	=	EVALUATION::Evaluate(A, A.Side_to_move);
    if (evaluation 	>=	Beta) 	{	return Beta;	}
    if (evaluation  >=	Alpha) 	{	Alpha 	=	evaluation;	}
    int CAPTURE_LIST_SIZE	=	GENERATE::CaptureMove(A, iter, A.Side_to_move);
    for (int i = 0; i < CAPTURE_LIST_SIZE; i++){
        int iMin = i;
        for (int j = i+1; j < CAPTURE_LIST_SIZE; j++)	if (CaptureList[j].value > CaptureList[iMin].value)	iMin	=	j;
        if ( i != iMin )	{
            ExtMove	tmp			=	CaptureList[i];
            CaptureList[i]		=	CaptureList[iMin];
            CaptureList[iMin]	=	tmp;
        }
        evaluation	=	-DynamicEval(MOVE::MakeMove(A, CaptureList[i]), -Beta, -Alpha);
        if (evaluation 	>=	Beta) 	{	return Beta;	}
        if (evaluation  >=	Alpha) 	{	Alpha 	=	evaluation;	}
    }
	free (iter);
    return Alpha;
}

int	DynamicEval	(BOARD_C A, int Alpha, int Beta){
	std::vector<ExtMove>  CaptureList;
    int evaluation	=	EVALUATION::Evaluate(A, A.Side_to_move);
    if (evaluation 	>=	Beta) 	{	return Beta;	}
    if (evaluation  >=	Alpha) 	{	Alpha 	=	evaluation;	}
    CaptureList = GENERATE::CaptureMoves(A, A.Side_to_move);
    for (int i = 0; i < CaptureList.size(); i++){
        int iMin = i;
        for (int j = i+1; j < CaptureList.size(); j++)	if (CaptureList[j].value > CaptureList[iMin].value)	iMin	=	j;
        //what did i wrote here? Could be optimized??
		if ( i != iMin )	{
            ExtMove	tmp			=	CaptureList[i];
            CaptureList[i]		=	CaptureList[iMin];
            CaptureList[iMin]	=	tmp;
        }
        evaluation	=	-DynamicEval(A.MakeMove(CaptureList[i]), -Beta, -Alpha);
        if (evaluation 	>=	Beta) 	{	return Beta;	}
        if (evaluation  >=	Alpha) 	{	Alpha 	=	evaluation;	}
    }
    return Alpha;
}
}