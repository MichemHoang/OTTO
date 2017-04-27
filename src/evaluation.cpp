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
	-5,	0,	0,	0,	0,	0,	0, -5,
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

int	VALUE[13]		=	{P_VALUE, N_VALUE, B_VALUE, R_VALUE, Q_VALUE, K_VALUE, P_VALUE, N_VALUE, B_VALUE, R_VALUE, Q_VALUE, K_VALUE, 0};
int RayLookUp[8]	=	{1,3,5,7,0,2,4,6};
BitBoard FILES[8]	=	{FileA, FileB, FileC, FileD, FileE, FileF, FileG, FileH};
BitBoard RANKS[8]	=	{Rank1, Rank2, Rank3, Rank4, Rank5, Rank6, Rank7, Rank8};
//return relative score for a move based on destination
//Favors pawn pushing and discourage Queen, King and Rook movement

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
 */

int	EvaluateBOARD	(BOARD A, int side){
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
	for (int i = 0; i < 6; i++)	MATERIAL_BALANCE	+=	(PopsCount (  A.Pieces[i] ) - PopsCount ( A.Pieces[i + 6] )) * VALUE[i];
	
	//PAWN FORMATION
	Pawn[0]	=	A.Pieces[wP]; Pawn[1]	=	A.Pieces[bP];
	for ( int i = 0; i < 2; i++)
		while (Pawn[i] != 0 )	if ((Pawn_AttackMask[i][BitPop(Pawn[i])]	& A.Pieces[i*6]) != 0 ) PAWN_STRUCTURE	+=	25*(1 - 2*i);
	CENTER_CONTROL	=	40*(PopsCount(A.CurrentBoard[WHITE] & CenterBonus) - PopsCount(A.CurrentBoard[BLACK] & CenterBonus));
	
	//CASTLING
	if ((A.Castling_check & ( 0xE << 4  )) == (0xE << 4  ))	CASTLING -= 50 ;
	if ((A.Castling_check &  0xE ) == 0xE ) 				CASTLING += 50 ;
	
	//BISHOP_PAIR
	Bishop[0]	=	A.Pieces[wB];	Bishop[1]	=	A.Pieces[bB];
	Knight[0]	=	A.Pieces[wN];	Knight[1]	=	A.Pieces[bN];
	if (PopsCount(Bishop[0])	==	2) B_PAIR += 15;
	if (PopsCount(Bishop[1])	==	2) B_PAIR -= 15;
	
	//DOUBLE PAWN
	Pawn[0]	=	A.Pieces[0]; Pawn[1]	=	A.Pieces[6];
	for (int file = 0; file < 8; file++){
		if (PopsCount(FILES[file]	& Pawn[0]) > 1)	BLK_PWN -= 	15;
		if (PopsCount(FILES[file]	& Pawn[1]) > 1)	BLK_PWN	+=	15;
	}
	side	==	BLACK ? BLK_PWN *= -1 : BLK_PWN *= 	1;
	
	//PIECE SQR VALUE 
	Pawn[0]	=	A.Pieces[wP]; Pawn[1]	=	A.Pieces[bP];
	while (Pawn[0] != 0)	PSQR_VALUE	+=	PawnTable[BitPop(Pawn[0])];
	while (Pawn[1] != 0)	PSQR_VALUE	-=	PawnTable[63 - BitPop(Pawn[1])];
	while (Bishop[0] != 0)	PSQR_VALUE	+=	BishopTable[BitPop(Bishop[0])];
	while (Bishop[1] != 0)	PSQR_VALUE	-=	BishopTable[63 - BitPop(Bishop[1])];
	while (Knight[0] != 0)	PSQR_VALUE	+=	KnightTable[BitPop(Knight[0])];
	while (Knight[1] != 0)	PSQR_VALUE	-=	KnightTable[63 - BitPop(Knight[1])];
	
	if (A.No_Ply >= 15) PSQR_VALUE	*= 0.2;
	else   				PSQR_VALUE	*= 0.3;
	
	//KING SAFETY
	Pawn[0]	=	A.Pieces[wP]; Pawn[1]	=	A.Pieces[bP];
	int a = 0, b = 0;
	int KingPos	=	LSBit(A.Pieces[wK]);
	if ((AttackRay[2][KingPos] & Pawn[0]) != 0) a += 90;
	if (KingPos%8 != 7) if ((AttackRay[2][KingPos + 1] & Pawn[0]) != 0) a += 60;
	if (KingPos%8 != 0) if ((AttackRay[2][KingPos - 1] & Pawn[0]) != 0) a += 60;
	if (A.Pieces[bQ]	==	0) a	*= 0.8;
	
	int d =  PopsCount(King_AttackMask[KingPos] & A.CurrentBoard[0]);
	d *= 30;
	
	KingPos	=	LSBit(A.Pieces[bK]);
	if ((AttackRay[6][KingPos] & Pawn[1]) != 0) b -= 90;
	if (KingPos%8 != 7) if ((AttackRay[6][KingPos + 1] & Pawn[1]) != 0) b -= 60;
	if (KingPos%8 != 0) if ((AttackRay[6][KingPos - 1] & Pawn[1]) != 0) b -= 60;
	if (A.Pieces[wQ]	==	0) b	*= 0.8;
	int e =  PopsCount(King_AttackMask[KingPos] & A.CurrentBoard[1]);
	e *= -30;
	
	KING_SAFETY	=	a + b + d + e;
	
	if (A.No_Ply < 12) KING_SAFETY*= 0.3;
	if (A.No_Ply > 70) KING_SAFETY*= 0.7;
	
	
	TOTAL_SCORE	=	MATERIAL_BALANCE + PAWN_STRUCTURE + CENTER_CONTROL + MOBILITY + CASTLING + KING_SAFETY + PSQR_VALUE + BLK_PWN;
	side	==	BLACK ? TOTAL_SCORE *= -1 : TOTAL_SCORE *= 	1;
	return TOTAL_SCORE;
}


int LeastValuableAttacker	(int att_Sqr, BOARD A, int side){
	int	QPosition	=	-1;
	BitBoard SlidingPiece;
	BitBoard B;
	int position	=	-1;
	SlidingPiece	=	generateQueenMove(att_Sqr, A.CurrentBoard[side ^ 1] | BIT1 >> att_Sqr, A.CurrentBoard[side]);
	B	=	(Pawn_AttackMask[side^1][att_Sqr]	&	A.Pieces[wP + 6*(side)]);
	while (B!=0){
		position	=	BitPop(B);
		return position;
	}
	B	=	Knight_AttackMask[att_Sqr]	&	A.Pieces[wN	+	6 * side];
	while (B!=0){
		position	=	BitPop(B);
		return position;
	}
	for (int i = 0; i < 8; i++){
		B	=	AttackRay[RayLookUp[i]][att_Sqr] & SlidingPiece ;
		while (B!=0){
			if (i < 4)	{
				RayLookUp[i]	<	4? position	=	BitPopR(B) : position	=	BitPop(B);
				if (A.Sq[position]	==	(wB + 6 * side ))	return position;
				if (A.Sq[position]	==	(wQ + 6 * side )) 	QPosition	=	position;
			}	else {
				RayLookUp[i]	<	4? position	=	BitPopR(B) : position	=	BitPop(B);
				if (A.Sq[position]	==	(wR + 6 * side ))	return position;
				if (A.Sq[position]	==	(wQ + 6 * side )) 	QPosition	=	position;
			}
		}
	}
	B	=	King_AttackMask[att_Sqr]	&	A.Pieces[wK	+	6 * side];
	position	=	BitPop(B);
	if (A.Sq[position]	==	(wN + 6 * side ))	if (QPosition	==	-1) return position;
	return QPosition;
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
		Attacker		=	LeastValuableAttacker(To, A, A.Side_to_move);
		A.Side_to_move	^=	1;
		if (Attacker == -1) break;
		depth++;
		gain[depth]		=	VALUE[A.Sq[To]]	-	gain[depth-1];
		if (gain[depth-1] > 0 || gain[depth] < 0) {
			break;
		}
		A.Pieces[A.Sq[Attacker]]				^=	 ( ( BIT1 >> Attacker ) | (BIT1 >> To) );
		A.Pieces[A.Sq[To]]						^=	 ( BIT1 >> To );
		A.CurrentBoard[A.Side_to_move]			^=	 ( ( BIT1 >> Attacker ) | (BIT1 >> To) );
		A.CurrentBoard[A.Side_to_move ^ 1]		&=	 ~( BIT1 >> To );
		A.Sq[To]		=	A.Sq[Attacker];
		A.Sq[Attacker]	=	12;
	} while (1);
	while (depth >0)	{
		if (-gain[depth-1] <= gain[depth])  gain[depth-1]	=	-gain[depth];
		depth--;
	}
	return gain[0];
};


