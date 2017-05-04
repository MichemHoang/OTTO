#include "MoveGen.h"
/*
 * 
 * 
 */
 
extern int		RECUR_CALL;

namespace DECODE{
void DecodeMove	(ExtMove *A){
	int	from	=	A->move	&	0x3F;
	int to		=	(A->move	&	0xFC0) >> 6;
	int flags	=	(A->move	>> 12) & 0xF;
	std::cout	<< std::dec <<	from << "->" << to << "   Flags = " << flags << "   Value = " << A->value << std::endl;
}

void DecodeMove	(Move A){
	int	from	=	A	&	0x3F;
	int to		=	(A	&	0xFC0) >> 6;
	int flags	=	(A	>> 12) & 0xF;
	std::cout	<< std::dec <<	from << "->" << to << "   Flags = " << flags << std::endl;
}

string DecodeMove	(ExtMove *A, int i){
	int	from	=	A->move	&	0x3F;
	int to		=	(A->move	&	0xFC0) >> 6;
	int flags	=	(A->move	>> 12) & 0xF;
	string answer	=	to_string(from) + "->" + to_string(to) + "   Flags = " + to_string(flags);
	return answer;
}

string DecodeMove	(Move A, int i){
	int	from	=	A	&	0x3F;
	int to		=	(A	&	0xFC0) >> 6;
	int flags	=	(A	>> 12) & 0xF;
	string answer	=	to_string(from) + "->" + to_string(to) + "   Flags = " + to_string(flags);
	return answer;
}
}//endnamespace

ExtMove *MoveEncoding (int isPawn, int Sq, BitBoard Target, ExtMove *MoveList, BitBoard Enemy, BOARD C, int *Z){
	uint16_t	from;
	uint16_t	to;
	uint16_t	flags;
	from	=	Sq;
	MoveType	FLG;
	if (isPawn%6	!=	0){
		while (Target != 0){
			to				=	BitOp::BitPop(Target);
			if ((BIT1 >> to & Enemy) != 0 )	{	FLG		=	CAPTURE;		
				int a	=	EVALUATION::SEEA(to, C, from);
				if (a > -90) a += 200; else a -= 200;
				MoveList->value	=	EVALUATION::PieceSquareValue(isPawn, from, to) + a;	
			}
			else {	FLG		=	QUIET_MOVES;	
				int a	=	EVALUATION::SEEA(to, C, from);
				MoveList->value	=	EVALUATION::PieceSquareValue(isPawn, from, to) + a;	
			}
			flags	=	FLG;
			MoveList->move	=	(from | to << 6 | flags << 12);
			*Z	+=	1;
			MoveList++;
		}
	} else {
		while (Target != 0){
			to				=	BitOp::BitPop(Target);
			BitBoard		PawnDes		=	BIT1 >> to;
			if ( ( ( PawnDes & Rank1 ) || ( PawnDes & Rank8 ) ) != 0 ){
				if ( ( PawnDes & Enemy ) != 0 )	{
					for (int i =8; i < 12; i++)	{
						flags	=	i;
						MoveList->move	=	(from | to << 6 | flags << 12);
						MoveList->value	=	1.5 * i * i	+	EVALUATION::SEEA(to, C, from);
						*Z		+=1;
						MoveList++;
					}
				} else {
					for (int i =12; i < 16; i++)	{
						flags	=	i;
						MoveList->move	=	(from | to << 6 | flags << 12);
						MoveList->value	=	5 * i * i;
						*Z	+=1;
						MoveList++;
					}
				}
			
			} else {
				if ((BIT1 >> to & Enemy) != 0 )	{	
					FLG		=	CAPTURE;		
					int a	=	EVALUATION::SEEA(to, C, from);
					if (a > -30) a += 150; else a -= 150;
					MoveList->value	=	EVALUATION::PieceSquareValue(isPawn, from, to) + a;	
				}
				else {	
					int DoublePush	=	to - from;
					if (DoublePush == 16 || DoublePush	==	-16)	FLG	=	DOUBLE_PUSH;
					else  											FLG	=	QUIET_MOVES;	
					int a	=	EVALUATION::SEEA(to, C, from);
					MoveList->value	=	EVALUATION::PieceSquareValue(isPawn, from, to) + a;
				}
				flags	=	FLG;
				MoveList->move	=	(from | to << 6 | flags << 12);
				MoveList++;
				*Z	+=	1;
			}
		}
	}
	return MoveList;
}

/*
 * "all in all it's just another brick in the wall"
 */
namespace GENERATE{
int AllMove( struct BOARD A, ExtMove *MoveList, int Color ){
	int 		MOVELIST_TOTAL_ITEMS	=	0;
	BitBoard	Pieces[15];
	BitBoard	Moves[15];
	BitBoard	EnemyPieces, OwnPieces;
	Castling_right	castK, castQ;
	for (int wtf	=	0; 	wtf < 6; wtf++) Moves[wtf]	=	0;
	if (Color	==	WHITE){
		for (int i	=	0; i < 6; i++)	Pieces[i]	=	A.Pieces[i];
		OwnPieces	=	A.CurrentBoard[WHITE];
		EnemyPieces	=	A.CurrentBoard[BLACK];
		castK		=	CastlingKW;
		castQ		=	CastlingQW;
		if (((A.PreviousMove >> 12) & 0x0f)	==	DOUBLE_PUSH){
			int TO	=	((A.PreviousMove >> 6) & 0x3f);
			if (A.Sq[TO + 1]	==	wP){
				if (((TO + 1)%8) != 0) {
					MoveList->move	=	((TO + 1) | ((TO - 8) << 6) | ENPASSANT << 12);
					MoveList->value			=	41;
					MoveList++;
					MOVELIST_TOTAL_ITEMS	+=	1;
				}
			} 
			if (A.Sq[TO - 1]	==	wP){
				if (((TO - 1)%8) != 7) {
					MoveList->move	=	((TO - 1) | ((TO - 8) << 6) | ENPASSANT << 12);
					MoveList->value			=	41;
					MoveList++;
					MOVELIST_TOTAL_ITEMS	+=	1;
				}
			}
		}
		if ((A.Castling_check	&	castK)	!=	castK) {
			if ((EVALUATION::LVA(60, A, BLACK)	!=	-1) || 
				(EVALUATION::LVA(61, A, BLACK)	!=	-1) ||
				(EVALUATION::LVA(62, A, BLACK)	!=	-1)) {}
			else if (A.Sq[62]	==	emptySqr&& A.Sq[61]	==	emptySqr){
				MoveList->move	=	(60 | 62 << 6 | KING_CASTLE << 12);
				Moves[wK]		=	BIT1 >> 62;
				MoveList->value			=	250;
				MoveList++;
				MOVELIST_TOTAL_ITEMS	+=	1;
			}
		}
		if ((A.Castling_check	&	castQ)	!=	castQ){
			if (EVALUATION::LVA(60, A, BLACK)	!=	-1	||
				EVALUATION::LVA(59, A, BLACK)	!=	-1	||
				EVALUATION::LVA(58, A, BLACK)	!=	-1) {}
			else if (A.Sq[57]	==	emptySqr&& A.Sq[58]	==	emptySqr&& A.Sq[59]	==	emptySqr){
				MoveList->move	=	(60 | 58 << 6 | QUEEN_CASTLE << 12);
				Moves[wK]		=	BIT1 >> 58;
				MoveList->value			=	250;
				MoveList++;
				MOVELIST_TOTAL_ITEMS	+=	1;
			}
		}
	}
	else { 
		for (int i	=	0; i < 6; i++)	Pieces[i]	=	A.Pieces[i+6];
		OwnPieces	=	A.CurrentBoard[BLACK];
		EnemyPieces	=	A.CurrentBoard[WHITE];
		castK		=	CastlingKB;
		castQ		=	CastlingQB;
		if (((A.PreviousMove >> 12) & 0x0f)	==	DOUBLE_PUSH){
			int TO	=	((A.PreviousMove >> 6) & 0x3f);
			if (A.Sq[TO + 1]	==	bP){
				if (((TO + 1)%8) != 0) {
					MoveList->move	=	((TO + 1) | ((TO + 8) << 6) | ENPASSANT << 12);
					MoveList->value			=	41;
					MoveList++;
					MOVELIST_TOTAL_ITEMS	+=	1;
				}
			} 
			if (A.Sq[TO - 1]	==	bP){
				if (((TO - 1)%8) != 7) {
					MoveList->move	=	((TO - 1) | ((TO + 8) << 6) | ENPASSANT << 12);
					MoveList->value			=	41;
					MoveList++;
					MOVELIST_TOTAL_ITEMS	+=	1;
				}
			}
		}
		if (((A.Castling_check)	&	castK)	!=	castK) {
			if (EVALUATION::LVA(4, A, WHITE)	!=	-1	||
				EVALUATION::LVA(5, A, WHITE)	!=	-1	||
				EVALUATION::LVA(6, A, WHITE)	!=	-1) {}
			else if (A.Sq[5]	==	emptySqr&& A.Sq[6]	==	emptySqr){
				MoveList->move	=	(4 | 6 << 6 | KING_CASTLE << 12);
				Moves[bK]		=	BIT1 >> 6;
				MoveList->value			=	250;
				MoveList++;
				MOVELIST_TOTAL_ITEMS	+=	1;
			}
		}
		if (((A.Castling_check)	&	castQ)	!=	castQ){
			if (EVALUATION::LVA(4, A, WHITE)	!=	-1	||
				EVALUATION::LVA(3, A, WHITE)	!=	-1	||
				EVALUATION::LVA(2, A, WHITE)	!=	-1) {}
			else if (A.Sq[1]	==	emptySqr&& A.Sq[2]	==	emptySqr&& A.Sq[3]	==	emptySqr){
				MoveList->move	=	(4 | 2 << 6 | QUEEN_CASTLE << 12);
				Moves[bK]		=	BIT1 >> 2;
				MoveList->value			=	250;
				MoveList++;
				MOVELIST_TOTAL_ITEMS	+=	1;
			}
		}
	}
	for (int i	=	0; i < 6; i++){
		while (Pieces[i] != 0){
			BitBoard		MoveHolder;
			int position	=	BitOp::BitPop(Pieces[i]);
			MoveHolder		=	Picker(i, position, OwnPieces, EnemyPieces, Color, EMPTY_BRD);
			MoveList		=	MoveEncoding(i, position, MoveHolder, MoveList, EnemyPieces, A, &MOVELIST_TOTAL_ITEMS);
			Moves[i]		|=	MoveHolder;
		}	
	}	
	return MOVELIST_TOTAL_ITEMS;
}

int CaptureMove( struct BOARD A, ExtMove *MoveList, int Color ){
	int 		MOVELIST_TOTAL_ITEMS	=	0;
	BitBoard	Pieces[12];
	BitBoard	Moves[12];
	BitBoard	EnemyPieces, OwnPieces;
	for (int wtf	=	0; 	wtf < 6; wtf++) Moves[wtf]	=	0;
	if (Color	==	WHITE){
		for (int i	=	0; i < 6; i++)	Pieces[i]	=	A.Pieces[i];
		OwnPieces	=	A.CurrentBoard[WHITE];
		EnemyPieces	=	A.CurrentBoard[BLACK];
	}
	else { 
		for (int i	=	0; i < 6; i++)	Pieces[i]	=	A.Pieces[i+6];
		OwnPieces	=	A.CurrentBoard[BLACK];
		EnemyPieces	=	A.CurrentBoard[WHITE];
	}
	for (int i	=	0; i < 6; i++){
		while (Pieces[i] != 0){
			BitBoard		MoveHolder;
			int position	=	BitOp::BitPop(Pieces[i]);
			MoveHolder		=	Picker(i, position, OwnPieces, EnemyPieces, Color, EMPTY_BRD) & EnemyPieces;
			MoveList		=	MoveEncoding(i, position, MoveHolder, MoveList, EnemyPieces, A, &MOVELIST_TOTAL_ITEMS);
			Moves[i]		|=	MoveHolder;
		}
	}
	return MOVELIST_TOTAL_ITEMS;
}

int QuietMove( struct BOARD A, ExtMove *MoveList, int Color){
	int 		MOVELIST_TOTAL_ITEMS	=	0;
	BitBoard	Pieces[15];
	BitBoard	Moves[15];
	BitBoard	EnemyPieces, OwnPieces;
	Castling_right	castK, castQ;
	for (int wtf	=	0; 	wtf < 6; wtf++) Moves[wtf]	=	0;
	if (Color	==	WHITE){
		for (int i	=	0; i < 6; i++)	Pieces[i]	=	A.Pieces[i];
		OwnPieces	=	A.CurrentBoard[WHITE];
		EnemyPieces	=	A.CurrentBoard[BLACK];
		castK		=	CastlingKW;
		castQ		=	CastlingQW;
		if (((A.PreviousMove >> 12) & 0x0f)	==	DOUBLE_PUSH){
			int TO	=	((A.PreviousMove >> 6) & 0x3f);
			if (A.Sq[TO + 1]	==	wP){
				if (((TO + 1)%8) != 0) {
					MoveList->move	=	((TO + 1) | ((TO - 8) << 6) | ENPASSANT << 12);
					MoveList->value			=	41561;
					MoveList++;
					MOVELIST_TOTAL_ITEMS	+=	1;
				}
			} 
			if (A.Sq[TO - 1]	==	wP){
				if (((TO - 1)%8) != 7) {
					MoveList->move	=	((TO - 1) | ((TO - 8) << 6) | ENPASSANT << 12);
					MoveList->value			=	41;
					MoveList++;
					MOVELIST_TOTAL_ITEMS	+=	1;
				}
			}
		}
		if ((A.Castling_check	&	castK)	!=	castK) {
			if ((EVALUATION::LVA(60, A, BLACK)	!=	-1) || 
				(EVALUATION::LVA(61, A, BLACK)	!=	-1) ||
				(EVALUATION::LVA(62, A, BLACK)	!=	-1)) {}
			else if (A.Sq[62]	==	emptySqr&& A.Sq[61]	==	emptySqr){
				MoveList->move	=	(60 | 62 << 6 | KING_CASTLE << 12);
				Moves[wK]		=	BIT1 >> 62;
				MoveList->value			=	250;
				MoveList++;
				MOVELIST_TOTAL_ITEMS	+=	1;
			}
		}
		if ((A.Castling_check	&	castQ)	!=	castQ){
			if (EVALUATION::LVA(60, A, BLACK)	!=	-1	||
				EVALUATION::LVA(59, A, BLACK)	!=	-1	||
				EVALUATION::LVA(58, A, BLACK)	!=	-1) {}
			else if (A.Sq[57]	==	emptySqr&& A.Sq[58]	==	emptySqr&& A.Sq[59]	==	emptySqr){
				MoveList->move	=	(60 | 58 << 6 | QUEEN_CASTLE << 12);
				Moves[wK]		=	BIT1 >> 58;
				MoveList->value			=	250;
				MoveList++;
				MOVELIST_TOTAL_ITEMS	+=	1;
			}
		}
	}
	else { 
		for (int i	=	0; i < 6; i++)	Pieces[i]	=	A.Pieces[i+6];
		OwnPieces	=	A.CurrentBoard[BLACK];
		EnemyPieces	=	A.CurrentBoard[WHITE];
		castK		=	CastlingKB;
		castQ		=	CastlingQB;
		if (((A.PreviousMove >> 12) & 0x0f)	==	DOUBLE_PUSH){
			int TO	=	((A.PreviousMove >> 6) & 0x3f);
			if (A.Sq[TO + 1]	==	bP){
				if (((TO + 1)%8) != 0) {
					MoveList->move	=	((TO + 1) | ((TO + 8) << 6) | ENPASSANT << 12);
					MoveList->value			=	41;
					MoveList++;
					MOVELIST_TOTAL_ITEMS	+=	1;
				}
			} 
			if (A.Sq[TO - 1]	==	bP){
				if (((TO - 1)%8) != 7) {
					MoveList->move	=	((TO - 1) | ((TO + 8) << 6) | ENPASSANT << 12);
					MoveList->value			=	41561;
					MoveList++;
					MOVELIST_TOTAL_ITEMS	+=	1;
				}
			}
		}
		if (((A.Castling_check)	&	castK)	!=	castK) {
			if (EVALUATION::LVA(4, A, WHITE)	!=	-1	||
				EVALUATION::LVA(5, A, WHITE)	!=	-1	||
				EVALUATION::LVA(6, A, WHITE)	!=	-1) {}
			else if (A.Sq[5]	==	emptySqr&& A.Sq[6]	==	emptySqr){
				MoveList->move	=	(4 | 6 << 6 | KING_CASTLE << 12);
				Moves[bK]		=	BIT1 >> 6;
				MoveList->value			=	250;
				MoveList++;
				MOVELIST_TOTAL_ITEMS	+=	1;
			}
		}
		if (((A.Castling_check)	&	castQ)	!=	castQ){
			if (EVALUATION::LVA(4, A, WHITE)	!=	-1	||
				EVALUATION::LVA(3, A, WHITE)	!=	-1	||
				EVALUATION::LVA(2, A, WHITE)	!=	-1) {}
			else if (A.Sq[1]	==	emptySqr&& A.Sq[2]	==	emptySqr&& A.Sq[3]	==	emptySqr){
				MoveList->move	=	(4 | 2 << 6 | QUEEN_CASTLE << 12);
				Moves[bK]		=	BIT1 >> 2;
				MoveList->value			=	250;
				MoveList++;
				MOVELIST_TOTAL_ITEMS	+=	1;
			}
		}
	}
	for (int i	=	0; i < 6; i++){
		while (Pieces[i] != 0){
			BitBoard		MoveHolder;
			int position	=	BitOp::BitPop(Pieces[i]);
			MoveHolder		=	Picker(i, position, OwnPieces, EnemyPieces, Color, EMPTY_BRD) & (~EnemyPieces);
			MoveList		=	MoveEncoding(i, position, MoveHolder, MoveList, EnemyPieces, A, &MOVELIST_TOTAL_ITEMS);
			Moves[i]		|=	MoveHolder;
		}	
	}	
	return MOVELIST_TOTAL_ITEMS;
}

BitBoard Picker(int chooser, int pos, BitBoard OwnPieces, BitBoard EnemyPieces, int Color, BitBoard capture){
	BitBoard result;
	switch (chooser){
		case wP:	result	=	GENERATE::Pawn	( pos,  OwnPieces,  EnemyPieces,  Color, capture);	break;
		case wB:	result	=	GENERATE::Bishop( pos,  OwnPieces,  EnemyPieces);					break;
		case wR:	result	=	GENERATE::Rook	( pos,  OwnPieces,  EnemyPieces);					break;
		case wQ:	result	=	GENERATE::Queen	( pos,  OwnPieces,  EnemyPieces);					break;
		case wN:	result	=	GENERATE::Knight( pos,  OwnPieces,  EnemyPieces);					break;
		case wK:	result	=	GENERATE::King	( pos,  OwnPieces,  EnemyPieces);					break;
	}
	return result;
}

BitBoard Pawn		(int pos, BitBoard OwnPieces, BitBoard EnemyPieces, int Color, BitBoard cap){
	BitBoard ValidMoves	=	0;
	BitBoard position	=	BIT1 >> pos;
	if (Color	==	WHITE){
		ValidMoves	|=	( position	<< 8 ) & ~( OwnPieces | EnemyPieces ) & ~cap;	
		ValidMoves  |=  (ValidMoves & Rank3) << 8 & ~( OwnPieces | EnemyPieces ) & ~cap;
		ValidMoves	|=	( (	( position	<< 7 ) & FileA )  ^ (position << 7 )	) & ( EnemyPieces | cap );
		ValidMoves	|=	( (	( position	<< 9 ) & FileH )  ^ (position << 9 )	) & ( EnemyPieces | cap );
	} else {
		ValidMoves	=	(position	>> 8 ) & ~( OwnPieces | EnemyPieces ) & ~cap;
		ValidMoves  |=  (ValidMoves & Rank6) >> 8 & ~( OwnPieces | EnemyPieces ) & ~cap;
		ValidMoves	|=	( (	( position	>> 7 ) & FileH )  ^ (position >> 7 )	) & ( EnemyPieces | cap );
		ValidMoves	|=	( (	( position	>> 9 ) & FileA )  ^ (position >> 9 )	) & ( EnemyPieces | cap );
	}
	return ValidMoves;
}

BitBoard Knight		(int pos, BitBoard OwnPieces, BitBoard EnemyPieces){
	BitBoard ValidMoves	=	0;
	ValidMoves	=	MASK::NMask[pos] & ~OwnPieces;
	return ValidMoves;
}

BitBoard Bishop		(int pos, BitBoard OwnPieces, BitBoard EnemyPieces){
	BitBoard ValidMoves	=	0;
	int magicIndex;
	magicIndex	=	(int)( ((OwnPieces | EnemyPieces) & MASK::BMask[pos]) * magicNumberBishop[63 - pos] >> Shift_B[63 - pos]);
	ValidMoves	=	BishopMoveDatabase[pos][magicIndex] & ~OwnPieces;
	return ValidMoves;
}

BitBoard Rook		(int pos, BitBoard OwnPieces, BitBoard EnemyPieces){
	BitBoard ValidMoves	=	0;
	int magicIndex;
	magicIndex	=	(int)( ((OwnPieces | EnemyPieces) & MASK::RMask[pos]) * magicNumberRook[63 - pos] >> Shift_R[63 - pos]);
	ValidMoves	=	RookMoveDatabase[pos][magicIndex] & ~OwnPieces;
	return ValidMoves;
}

BitBoard Queen		(int pos, BitBoard OwnPieces, BitBoard EnemyPieces){
	BitBoard ValidMoves	=	0;
	int magicIndex;
	magicIndex	=	(int)( ((OwnPieces | EnemyPieces) & MASK::BMask[pos]) * magicNumberBishop[63 - pos] >> Shift_B[63 - pos]);
	ValidMoves	=	BishopMoveDatabase[pos][magicIndex] & ~OwnPieces;
	magicIndex	=	(int)( ((OwnPieces | EnemyPieces) & MASK::RMask[pos]) * magicNumberRook[63 - pos] >> Shift_R[63 - pos]);
	ValidMoves	|=	RookMoveDatabase[pos][magicIndex] & ~OwnPieces;
	return ValidMoves;
}

BitBoard King		(int pos, BitBoard OwnPieces, BitBoard EnemyPieces){
	BitBoard ValidMoves	=	0;
	ValidMoves	=	MASK::KMask[pos] & ~OwnPieces;
	return ValidMoves;
}
}//endnamespace;

namespace MOVE{
BOARD	MakeMove(BOARD initial, ExtMove transformer){
	uint8_t	from	=	transformer.getFrom();
	uint8_t to		=	transformer.getTo();
	uint8_t flag	=	transformer.getFlags();
	
	if (initial.Sq[from] == (wR + 6*initial.Side_to_move)){
		if 		(from	==	(63 - (56*initial.Side_to_move)))	initial.Castling_check |=	0x08 << (4*initial.Side_to_move);
		else if (from	==	(56 - (56*initial.Side_to_move)))	initial.Castling_check |=	0x04 << (4*initial.Side_to_move);
	}
	if (initial.Sq[from] == (wK + 6*initial.Side_to_move)){
		initial.Castling_check |=	(0xC << (4*initial.Side_to_move));
	}
	initial.Pieces[initial.Sq[from] ]	=	(initial.Pieces[initial.Sq[from] ]	& ~(BIT1 >> from)) | (BIT1 >> to);
	initial.Pieces[initial.Sq[to] ]		=	initial.Pieces[initial.Sq[to]	]	& ~(BIT1 >> to);
	initial.Sq[to]		=	initial.Sq[from];
	initial.Sq[from]	=	emptySqr;
	if (flag	==	KING_CASTLE){
		initial.Pieces[initial.Side_to_move*6 +	wR]	=	(initial.Pieces[initial.Side_to_move*6 +	wR]	& ~(BIT1 >> (from + 3))) | (BIT1 >> (from + 1));
		initial.Sq[from + 1]	=	initial.Sq[from + 3];
		initial.Sq[from + 3]	=	emptySqr;
		initial.Castling_check	|=	( 0xE << (initial.Side_to_move*4));
	}                                                                                                                                                                   
	if (flag	==	QUEEN_CASTLE){
		initial.Pieces[initial.Side_to_move*6 +	wR]	=	(initial.Pieces[initial.Side_to_move*6 +	wR]	& ~(BIT1 >> (from -4))) | (BIT1 >> (from -1));
		initial.Sq[from - 1]	=	initial.Sq[from - 4];
		initial.Sq[from - 4]	=	emptySqr;
		initial.Castling_check	|=	( 0xE << (initial.Side_to_move*4));
	}
	if (flag > 7) {
		initial.Pieces[initial.Side_to_move]	^=	(BIT1 >> to);
		initial.Pieces[(flag - 8) % 4 + 1 + 6*initial.Side_to_move]		|=	(BIT1 >> to);
		initial.Sq[to]		=	(flag - 8) % 4 + 1 + 6*initial.Side_to_move;
	}
	if (flag == ENPASSANT){
		if (initial.Side_to_move == WHITE){
			initial.Pieces[bP]	&=	~(BIT1 >> (to + 8));
			initial.Sq[to + 8]	=	emptySqr;
		} else {
			initial.Pieces[wP]	&=	~(BIT1 >> (to - 8));
			initial.Sq[to - 8]	=	emptySqr;
		}
	}
	initial.CurrentBoard[0]	=	initial.Pieces[wP] | initial.Pieces[wN] | initial.Pieces[wR] | initial.Pieces[wB] | initial.Pieces[wK] | initial.Pieces[wQ];
	initial.CurrentBoard[1]	=	initial.Pieces[bP] | initial.Pieces[bN] | initial.Pieces[bR] | initial.Pieces[bB] | initial.Pieces[bK] | initial.Pieces[bQ];
	initial.Side_to_move	^=	0x01;
	initial.PreviousMove	=	transformer.move;
	initial.No_Ply++;
	return initial;
}


BOARD	MakeMove(BOARD initial, Move transformer){
	uint8_t	from	=	transformer & 0x3f;
	uint8_t to		=	transformer >> 6 & 0x3f;
	uint8_t flag	=	transformer >> 12 & 0x0f;;
	if (initial.Sq[from] == (wR + 6*initial.Side_to_move)){
		if 		(from	==	(63 - (56*initial.Side_to_move)))	initial.Castling_check |=	0x08 << (4*initial.Side_to_move);
		else if (from	==	(56 - (56*initial.Side_to_move)))	initial.Castling_check |=	0x04 << (4*initial.Side_to_move);
	}
	if (initial.Sq[from] == (wK + 6*initial.Side_to_move)){
		initial.Castling_check |=	(0xC << (4*initial.Side_to_move));
	}
	initial.Pieces[initial.Sq[from] ]	=	(initial.Pieces[initial.Sq[from] ]	& ~(BIT1 >> from)) | (BIT1 >> to);
	initial.Pieces[initial.Sq[to] ]	=	initial.Pieces[initial.Sq[to]	]	& ~(BIT1 >> to);
	initial.Sq[to]		=	initial.Sq[from];
	initial.Sq[from]	=	emptySqr; 
	if (flag	==	KING_CASTLE){
		initial.Pieces[initial.Side_to_move*6 +	wR]	=	(initial.Pieces[initial.Side_to_move*6 +	wR]	& ~(BIT1 >> (from + 3))) | (BIT1 >> (from + 1));
		initial.Sq[from + 1]	=	initial.Sq[from + 3];
		initial.Sq[from + 3]	=	emptySqr;
		initial.Castling_check	|=	( 0xE << (initial.Side_to_move*4));
	}
	if (flag	==	QUEEN_CASTLE){
		initial.Pieces[initial.Side_to_move*6 +	wR]	=	(initial.Pieces[initial.Side_to_move*6 +	wR]	& ~BIT1 >> (from -4)) | (BIT1 >> (from -1));
		initial.Sq[from - 1]	=	initial.Sq[from - 4];
		initial.Sq[from - 4]	=	emptySqr;
		initial.Castling_check	|=	( 0xE << (initial.Side_to_move*4));
	}
	if (flag > 7) {
		initial.Pieces[initial.Side_to_move]	^=	(BIT1 >> to);
		initial.Pieces[(flag - 8) % 4 + 1 + 6*initial.Side_to_move]		|=	(BIT1 >> to);
		initial.Sq[to]		=	(flag - 8) % 4 + 1 + 6*initial.Side_to_move;
	}
	if (flag == ENPASSANT){
		if (initial.Side_to_move == WHITE){
			initial.Pieces[bP]	&=	~(BIT1 >> (to + 8));
			initial.Sq[to + 8]	=	emptySqr;
		} else {
			initial.Pieces[wP]	&=	~(BIT1 >> (to - 8));
			initial.Sq[to - 8]	=	emptySqr;
		}
	}
	initial.CurrentBoard[0]	=	initial.Pieces[wP] | initial.Pieces[wN] | initial.Pieces[wR] | initial.Pieces[wB] | initial.Pieces[wK] | initial.Pieces[wQ];
	initial.CurrentBoard[1]	=	initial.Pieces[bP] | initial.Pieces[bN] | initial.Pieces[bR] | initial.Pieces[bB] | initial.Pieces[bK] | initial.Pieces[bQ];
	initial.Side_to_move	^=	0x01;
	initial.PreviousMove	=	transformer;//
	initial.No_Ply++;
	return initial;
}
}


//Unfinished Undo Move


