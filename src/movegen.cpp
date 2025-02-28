#include "movegen.h"
#include <algorithm>    // std::sort

namespace DECODE{
void DecodeMove	(ExtMove *A){
	std::string	from	=	squares[A->move	&	0x3F];
	std::string to		=	squares[(A->move	&	0xFC0) >> 6];
	int flags	=	(A->move	>> 12) & 0xF;
	std::cout	<< std::dec <<	from << "->" << to << "   Flags = " << flags << "   Value = " << A->value << std::endl;
}

void DecodeMove	(Move A){
	std::string	from	=	squares[A	&	0x3F];
	std::string to		=	squares[(A	&	0xFC0) >> 6];
	int flags	=	(A	>> 12) & 0xF;
	std::cout	<< std::dec <<	from << "->" << to << "   Flags = " << flags << std::endl;
}

}//endnamespace

//this need refactor: use vector instead of raw pointer?
ExtMove *MoveEncoding (int isPawn, int Sq, BitBoard Target, ExtMove *moveList, BitBoard Enemy, BOARD C, int *Z){
	uint16_t	from;
	uint16_t	to;
	uint16_t	flags;
	from	=	Sq;
	MoveType	FLG; //Flag
	if (isPawn%6	!=	0){
		while (Target != 0){
			to = BitOp::BitPop(Target);
			if ((BIT1 >> to & Enemy) != 0 )	{	FLG =	CAPTURE;		
				int a	=	EVALUATION::SEEA(to, C, from);
				if (a > -90) a += 200; else a -= 200;
				moveList->value	=	EVALUATION::PieceSquareValue(isPawn, from, to) + a;	
			}
			else {	FLG		=	QUIET_MOVES;	
				int a	=	EVALUATION::SEEA(to, C, from);
				moveList->value	=	EVALUATION::PieceSquareValue(isPawn, from, to) + a;	
			}
			flags	=	FLG;
			moveList->move	=	(from | to << 6 | flags << 12);
			*Z	+=	1;
			moveList++;
		}
	} else {
		while (Target != 0){
			to				=	BitOp::BitPop(Target);
			BitBoard		PawnDes		=	BIT1 >> to;
			if ( ( ( PawnDes & Rank1 ) || ( PawnDes & Rank8 ) ) != 0 ){
				if ( ( PawnDes & Enemy ) != 0 )	{
					for (int i =8; i < 12; i++)	{
						flags	=	i;
						moveList->move	=	(from | to << 6 | flags << 12);
						moveList->value	=	1.5 * i * i	+	EVALUATION::SEEA(to, C, from);
						*Z		+=1;
						moveList++;
					}
				} else {
					for (int i =12; i < 16; i++)	{
						flags	=	i;
						moveList->move	=	(from | to << 6 | flags << 12);
						moveList->value	=	5 * i * i;
						*Z	+=1;
						moveList++;
					}
				}
			
			} else {
				if ((BIT1 >> to & Enemy) != 0 )	{	
					FLG		=	CAPTURE;		
					int a	=	EVALUATION::SEEA(to, C, from);
					if (a > -30) a += 150; else a -= 150;
					moveList->value	=	EVALUATION::PieceSquareValue(isPawn, from, to) + a;	
				}
				else {	
					int DoublePush	=	to - from;
					if (DoublePush == 16 || DoublePush	==	-16)	FLG	=	DOUBLE_PUSH;
					else  											FLG	=	QUIET_MOVES;	
					int a	=	EVALUATION::SEEA(to, C, from);
					moveList->value	=	EVALUATION::PieceSquareValue(isPawn, from, to) + a;
				}
				flags	=	FLG;
				moveList->move	=	(from | to << 6 | flags << 12);
				moveList++;
				*Z	+=	1;
			}
		}
	}
	return moveList;
}

/*
	add legal moves to a movelist
*/
void MoveEncoding(int isPawn, int from, BitBoard target, std::vector<ExtMove> *moveList, BitBoard enemy, BOARD_C board){
	uint16_t	to;
	uint16_t	flags;
	MoveType	FLG; //Flag
	if (isPawn%6 != 0){ //if piece is not pawn
		while (target != 0){
			ExtMove newMove;
			to = BitOp::BitPop(target);
			if ((BIT1 >> to & enemy) != 0 )	{	
				FLG =	CAPTURE;		
				int a	=	EVALUATION::SEEA(to, board, from);
				if (a > -90) a += 200; else a -= 200;
				newMove.value = EVALUATION::PieceSquareValue(isPawn, from, to) + a;	
			}
			else {	
				FLG	= QUIET_MOVES;	
				int a = EVALUATION::SEEA(to, board, from);
				newMove.value = EVALUATION::PieceSquareValue(isPawn, from, to) + a;	
			}
			flags	=	FLG;
			newMove.move	=	(from | to << 6 | flags << 12);
			moveList->push_back(newMove);
		}
	} else { //if piece is pawn
		while (target != 0){
			ExtMove newMove;
			to = BitOp::BitPop(target);
			BitBoard pawnDes = BIT1 >> to;
			if ( ( ( pawnDes & Rank1 ) || ( pawnDes & Rank8 ) ) != 0 ){
				if ( ( pawnDes & enemy ) != 0 )	{
					for (int i = 8; i < 12; i++)	{
						flags	=	i;
						newMove.move	=	(from | to << 6 | flags << 12);
						newMove.value	=	1.5 * i * i	+	EVALUATION::SEEA(to, board, from);
						moveList->push_back(newMove);
					}
				} else { //promotion foe pawn
					for (int i = 12; i < 16; i++)	{
						flags	=	i;
						newMove.move	=	(from | to << 6 | flags << 12);
						newMove.value	=	5 * i * i;
						moveList->push_back(newMove);
					}
				}
			} else {
				if ((BIT1 >> to & enemy) != 0 )	{	
					FLG		=	CAPTURE;		
					int a	=	EVALUATION::SEEA(to, board, from);
					if (a > -30) a += 150; else a -= 150;
					newMove.value	=	EVALUATION::PieceSquareValue(isPawn, from, to) + a;	
				}
				else {	
					int DoublePush	=	to - from;
					if (DoublePush == 16 || DoublePush	==	-16)	FLG	=	DOUBLE_PUSH;
					else  											FLG	=	QUIET_MOVES;	
					int a	=	EVALUATION::SEEA(to, board, from);
					newMove.value	=	EVALUATION::PieceSquareValue(isPawn, from, to) + a;
				}
				flags	=	FLG;
				newMove.move	=	(from | to << 6 | flags << 12);
				moveList->push_back(newMove);
			}
		}
	}
}

/*
 * "all in all it's just another brick in the wall"
 */
namespace GENERATE{
int AllMove( struct BOARD A, ExtMove *moveList, int Color ){
	int 		moveList_TOTAL_ITEMS	=	0;
	BitBoard	Pieces[15]; 
	BitBoard	Moves[15];
	BitBoard	enemyPieces, ownPieces;
	Castling_right	castK, castQ;
	for (int wtf	=	0; 	wtf < 6; wtf++) Moves[wtf]	=	0;
	if (Color	==	WHITE){
		for (int i	=	0; i < 6; i++)	Pieces[i]	=	A.Pieces[i];
		ownPieces	=	A.CurrentBoard[WHITE];
		enemyPieces	=	A.CurrentBoard[BLACK];
		castK		=	CastlingKW;
		castQ		=	CastlingQW;
		if (((A.PreviousMove >> 12) & 0x0f)	==	DOUBLE_PUSH){
			int TO	=	((A.PreviousMove >> 6) & 0x3f);
			if (A.Sq[TO + 1]	==	wP){
				if (((TO + 1)%8) != 0) {
					moveList->move	=	((TO + 1) | ((TO - 8) << 6) | ENPASSANT << 12);
					moveList->value			=	41;
					moveList++;
					moveList_TOTAL_ITEMS	+=	1;
				}
            }
			if (A.Sq[TO - 1]	==	wP){
				if (((TO - 1)%8) != 7) {
					moveList->move	=	((TO - 1) | ((TO - 8) << 6) | ENPASSANT << 12);
					moveList->value			=	41;
					moveList++;
					moveList_TOTAL_ITEMS	+=	1;
				}
			}
		}
		if ((A.Castling_check	&	castK)	!=	castK) {
			if ((EVALUATION::LVA(60, A, BLACK)	!=	-1) || 
				(EVALUATION::LVA(61, A, BLACK)	!=	-1) ||
				(EVALUATION::LVA(62, A, BLACK)	!=	-1)) {}
			else if (A.Sq[62]	==	emptySqr&& A.Sq[61]	==	emptySqr){
				moveList->move	=	(60 | 62 << 6 | KING_CASTLE << 12);
				Moves[wK]		=	BIT1 >> 62;
				moveList->value			=	250;
				moveList++;
				moveList_TOTAL_ITEMS	+=	1;
			}
		}
		if ((A.Castling_check	&	castQ)	!=	castQ){
			if (EVALUATION::LVA(60, A, BLACK)	!=	-1	||
				EVALUATION::LVA(59, A, BLACK)	!=	-1	||
				EVALUATION::LVA(58, A, BLACK)	!=	-1) {}
			else if (A.Sq[57]	==	emptySqr&& A.Sq[58]	==	emptySqr&& A.Sq[59]	==	emptySqr){
				moveList->move	=	(60 | 58 << 6 | QUEEN_CASTLE << 12);
				Moves[wK]		=	BIT1 >> 58;
				moveList->value			=	250;
				moveList++;
				moveList_TOTAL_ITEMS	+=	1;
			}
		}
	}
	else { 
		for (int i	=	0; i < 6; i++)	Pieces[i]	=	A.Pieces[i+6];
		ownPieces	=	A.CurrentBoard[BLACK];
		enemyPieces	=	A.CurrentBoard[WHITE];
		castK		=	CastlingKB;
		castQ		=	CastlingQB;
		if (((A.PreviousMove >> 12) & 0x0f)	==	DOUBLE_PUSH){
			int TO	=	((A.PreviousMove >> 6) & 0x3f);
			if (A.Sq[TO + 1]	==	bP){
				if (((TO + 1)%8) != 0) {
					moveList->move	=	((TO + 1) | ((TO + 8) << 6) | ENPASSANT << 12);
					moveList->value			=	41;
					moveList++;
					moveList_TOTAL_ITEMS	+=	1;
				}
			} 
			if (A.Sq[TO - 1]	==	bP){
				if (((TO - 1)%8) != 7) {
					moveList->move	=	((TO - 1) | ((TO + 8) << 6) | ENPASSANT << 12);
					moveList->value			=	41;
					moveList++;
					moveList_TOTAL_ITEMS	+=	1;
				}
			}
		}
		if (((A.Castling_check)	&	castK)	!=	castK) {
			if (EVALUATION::LVA(4, A, WHITE)	!=	-1	||
				EVALUATION::LVA(5, A, WHITE)	!=	-1	||
				EVALUATION::LVA(6, A, WHITE)	!=	-1) {}
			else if (A.Sq[5]	==	emptySqr&& A.Sq[6]	==	emptySqr){
				moveList->move	=	(4 | 6 << 6 | KING_CASTLE << 12);
				Moves[bK]		=	BIT1 >> 6;
				moveList->value	=	250;
				moveList++;
				moveList_TOTAL_ITEMS	+=	1;
			}
		}
		if (((A.Castling_check)	&	castQ)	!=	castQ){
			if (EVALUATION::LVA(4, A, WHITE)	!=	-1	||
				EVALUATION::LVA(3, A, WHITE)	!=	-1	||
				EVALUATION::LVA(2, A, WHITE)	!=	-1) {}
			else if (A.Sq[1]	==	emptySqr&& A.Sq[2]	==	emptySqr&& A.Sq[3]	==	emptySqr){
				moveList->move	=	(4 | 2 << 6 | QUEEN_CASTLE << 12);
				Moves[bK]		=	BIT1 >> 2;
				moveList->value			=	250;
				moveList++;
				moveList_TOTAL_ITEMS	+=	1;
			}
		}
	}
	for (int i	=	0; i < 6; i++){
		while (Pieces[i] != 0){
			BitBoard		MoveHolder;
			int position	=	BitOp::BitPop(Pieces[i]);
			MoveHolder		=	Picker(i, position, ownPieces, enemyPieces, Color, EMPTY_BRD);
			moveList		=	MoveEncoding(i, position, MoveHolder, moveList, enemyPieces, A, &moveList_TOTAL_ITEMS);
			Moves[i]		|=	MoveHolder;
		}	
	}	
	return moveList_TOTAL_ITEMS;
}

int CaptureMove( struct BOARD A, ExtMove *moveList, int Color ){
	int 		moveList_TOTAL_ITEMS	=	0;
	BitBoard	Pieces[12];
	BitBoard	Moves[12];
	BitBoard	enemyPieces, ownPieces;
	for (int wtf	=	0; 	wtf < 6; wtf++) Moves[wtf]	=	0;
	if (Color	==	WHITE){
		for (int i	=	0; i < 6; i++)	Pieces[i]	=	A.Pieces[i];
		ownPieces	=	A.CurrentBoard[WHITE];
		enemyPieces	=	A.CurrentBoard[BLACK];
	}
	else { 
		for (int i	=	0; i < 6; i++)	Pieces[i]	=	A.Pieces[i+6];
		ownPieces	=	A.CurrentBoard[BLACK];
		enemyPieces	=	A.CurrentBoard[WHITE];
	}
	for (int i	=	0; i < 6; i++){
		while (Pieces[i] != 0){
			BitBoard		MoveHolder;
			int position	=	BitOp::BitPop(Pieces[i]);
			MoveHolder		=	Picker(i, position, ownPieces, enemyPieces, Color, EMPTY_BRD) & enemyPieces;
			moveList		=	MoveEncoding(i, position, MoveHolder, moveList, enemyPieces, A, &moveList_TOTAL_ITEMS);
			Moves[i]		|=	MoveHolder;
		}
	}
	return moveList_TOTAL_ITEMS;
}

int QuietMove( struct BOARD A, ExtMove *moveList, int Color){
	int 		moveList_TOTAL_ITEMS	=	0;
	BitBoard	Pieces[15];
	BitBoard	Moves[15];
	BitBoard	enemyPieces, ownPieces;
	Castling_right	castK, castQ;
	for (int wtf	=	0; 	wtf < 6; wtf++) Moves[wtf]	=	0;
	if (Color	==	WHITE){
		for (int i	=	0; i < 6; i++)	Pieces[i]	=	A.Pieces[i];
		ownPieces	=	A.CurrentBoard[WHITE];
		enemyPieces	=	A.CurrentBoard[BLACK];
		castK		=	CastlingKW;
		castQ		=	CastlingQW;
		if (((A.PreviousMove >> 12) & 0x0f)	==	DOUBLE_PUSH){
			int TO	=	((A.PreviousMove >> 6) & 0x3f);
			if (A.Sq[TO + 1]	==	wP){
				if (((TO + 1)%8) != 0) {
					moveList->move	=	((TO + 1) | ((TO - 8) << 6) | ENPASSANT << 12);
					moveList->value			=	41561;
					moveList++;
					moveList_TOTAL_ITEMS	+=	1;
				}
			} 
			if (A.Sq[TO - 1]	==	wP){
				if (((TO - 1)%8) != 7) {
					moveList->move	=	((TO - 1) | ((TO - 8) << 6) | ENPASSANT << 12);
					moveList->value			=	41;
					moveList++;
					moveList_TOTAL_ITEMS	+=	1;
				}
			}
		}
		if ((A.Castling_check	&	castK)	!=	castK) {
			if ((EVALUATION::LVA(60, A, BLACK)	!=	-1) || 
				(EVALUATION::LVA(61, A, BLACK)	!=	-1) ||
				(EVALUATION::LVA(62, A, BLACK)	!=	-1)) {}
			else if (A.Sq[62]	==	emptySqr&& A.Sq[61]	==	emptySqr){
				moveList->move	=	(60 | 62 << 6 | KING_CASTLE << 12);
				Moves[wK]		=	BIT1 >> 62;
				moveList->value			=	250;
				moveList++;
				moveList_TOTAL_ITEMS	+=	1;
			}
		}
		if ((A.Castling_check	&	castQ)	!=	castQ){
			if (EVALUATION::LVA(60, A, BLACK)	!=	-1	||
				EVALUATION::LVA(59, A, BLACK)	!=	-1	||
				EVALUATION::LVA(58, A, BLACK)	!=	-1) {}
			else if (A.Sq[57]	==	emptySqr&& A.Sq[58]	==	emptySqr&& A.Sq[59]	==	emptySqr){
				moveList->move	=	(60 | 58 << 6 | QUEEN_CASTLE << 12);
				Moves[wK]		=	BIT1 >> 58;
				moveList->value			=	250;
				moveList++;
				moveList_TOTAL_ITEMS	+=	1;
			}
		}
	}
	else { 
		for (int i	=	0; i < 6; i++)	Pieces[i]	=	A.Pieces[i+6];
		ownPieces	=	A.CurrentBoard[BLACK];
		enemyPieces	=	A.CurrentBoard[WHITE];
		castK		=	CastlingKB;
		castQ		=	CastlingQB;
		if (((A.PreviousMove >> 12) & 0x0f)	==	DOUBLE_PUSH){
			int TO	=	((A.PreviousMove >> 6) & 0x3f);
			if (A.Sq[TO + 1]	==	bP){
				if (((TO + 1)%8) != 0) {
					moveList->move	=	((TO + 1) | ((TO + 8) << 6) | ENPASSANT << 12);
					moveList->value			=	41;
					moveList++;
					moveList_TOTAL_ITEMS	+=	1;
				}
			} 
			if (A.Sq[TO - 1]	==	bP){
				if (((TO - 1)%8) != 7) {
					moveList->move	=	((TO - 1) | ((TO + 8) << 6) | ENPASSANT << 12);
					moveList->value			=	41561;
					moveList++;
					moveList_TOTAL_ITEMS	+=	1;
				}
			}
		}
		if (((A.Castling_check)	&	castK)	!=	castK) {
			if (EVALUATION::LVA(4, A, WHITE)	!=	-1	||
				EVALUATION::LVA(5, A, WHITE)	!=	-1	||
				EVALUATION::LVA(6, A, WHITE)	!=	-1) {}
			else if (A.Sq[5]	==	emptySqr&& A.Sq[6]	==	emptySqr){
				moveList->move	=	(4 | 6 << 6 | KING_CASTLE << 12);
				Moves[bK]		=	BIT1 >> 6;
				moveList->value			=	250;
				moveList++;
				moveList_TOTAL_ITEMS	+=	1;
			}
		}
		if (((A.Castling_check)	&	castQ)	!=	castQ){
			if (EVALUATION::LVA(4, A, WHITE)	!=	-1	||
				EVALUATION::LVA(3, A, WHITE)	!=	-1	||
				EVALUATION::LVA(2, A, WHITE)	!=	-1) {}
			else if (A.Sq[1]	==	emptySqr&& A.Sq[2]	==	emptySqr&& A.Sq[3]	==	emptySqr){
				moveList->move	=	(4 | 2 << 6 | QUEEN_CASTLE << 12);
				Moves[bK]		=	BIT1 >> 2;
				moveList->value			=	250;
				moveList++;
				moveList_TOTAL_ITEMS	+=	1;
			}
		}
	}
	for (int i	=	0; i < 6; i++){
		while (Pieces[i] != 0){
			BitBoard		MoveHolder;
			int position	=	BitOp::BitPop(Pieces[i]);
			MoveHolder		=	Picker(i, position, ownPieces, enemyPieces, Color, EMPTY_BRD) & (~enemyPieces);
			moveList		=	MoveEncoding(i, position, MoveHolder, moveList, enemyPieces, A, &moveList_TOTAL_ITEMS);
			Moves[i]		|=	MoveHolder;
		}	
	}	
	return moveList_TOTAL_ITEMS;
}

//return all possible moves from a board configuration
std::vector<ExtMove> AllMoves(BOARD_C board, int side){
	std::vector<ExtMove> moveList;
	BitBoard Pieces[15];
	BitBoard Moves[15];
	BitBoard enemyPieces, ownPieces;
	Castling_right	castK, castQ;
	for (int i = 0; i < 6; i++) Moves[i] = 0;
	std::cout << "so far so good\n";
	if (side == WHITE){
		for (int i = 0; i < 6; i++)	Pieces[i]	=	board.Pieces[i];
		ownPieces	=	board.CurrentBoard[WHITE];
		enemyPieces	=	board.CurrentBoard[BLACK];
		castK		=	CastlingKW;
		castQ		=	CastlingQW;
		if (((board.PreviousMove >> 12) & 0x0f)	==	DOUBLE_PUSH){ //Check for enpassant, enpassant only work when there is a double pawn push by opp
			int TO	=	((board.PreviousMove >> 6) & 0x3f);
			if (board.Sq[TO + 1]	==	wP){
				ExtMove newMove;
				if (((TO + 1)%8) != 0) {
					newMove.move	=	((TO + 1) | ((TO - 8) << 6) | ENPASSANT << 12);
					newMove.value	=	41;
					moveList.push_back(newMove);
				}
            } 
			if (board.Sq[TO - 1]	==	wP){
				ExtMove newMove;
				if (((TO - 1)%8) != 7) {
					newMove.move	=	((TO - 1) | ((TO - 8) << 6) | ENPASSANT << 12);
					newMove.value	=	41;
					moveList.push_back(newMove);
				}
			}
		}
		std::cout << "so far so good\n";
		if ((board.Castling_check &	castK) != castK) {
			if ((EVALUATION::LVA(60, board, BLACK)	!=	-1) || 
				(EVALUATION::LVA(61, board, BLACK)	!=	-1) ||
				(EVALUATION::LVA(62, board, BLACK)	!=	-1)) {} //check if castling kingside right into a check;
			else if (board.Sq[62] == emptySqr && board.Sq[61] == emptySqr){
				ExtMove newMove;
				newMove.move	=	(60 | 62 << 6 | KING_CASTLE << 12);
				Moves[wK]		=	BIT1 >> 62;
				newMove.value	=	250;
				moveList.push_back(newMove);
			}
		}
		if ((board.Castling_check & castQ) != castQ){	
			if (EVALUATION::LVA(60, board, BLACK)	!=	-1	||
				EVALUATION::LVA(59, board, BLACK)	!=	-1	||
				EVALUATION::LVA(58, board, BLACK)	!=	-1) {} //check if castling queenside right into a check;
			else if (board.Sq[57] == emptySqr&& board.Sq[58] == emptySqr && board.Sq[59] == emptySqr){
				ExtMove newMove;
				newMove.move	=	(60 | 58 << 6 | QUEEN_CASTLE << 12);
				Moves[wK]		=	BIT1 >> 58;
				newMove.value	=	250;
				moveList.push_back(newMove);
			}
		}
		std::cout << "so far so good3\n";
	} else { 
		for (int i	=	0; i < 6; i++)	Pieces[i] = board.Pieces[i+6];
		ownPieces	=	board.CurrentBoard[BLACK];
		enemyPieces	=	board.CurrentBoard[WHITE];
		castK		=	CastlingKB;
		castQ		=	CastlingQB;
		if (((board.PreviousMove >> 12) & 0x0f)	== DOUBLE_PUSH){
			int TO	=	((board.PreviousMove >> 6) & 0x3f);
			if (board.Sq[TO + 1] == bP){
				ExtMove newMove;
				if (((TO + 1)%8) != 0) {
					newMove.move	=	((TO + 1) | ((TO + 8) << 6) | ENPASSANT << 12);
					newMove.value	=	41;
					moveList.push_back(newMove);
				}
			} 
			if (board.Sq[TO - 1] == bP){
				ExtMove newMove;
				if (((TO - 1)%8) != 7) {
					newMove.move	=	((TO - 1) | ((TO + 8) << 6) | ENPASSANT << 12);
					newMove.value	=	41;
					moveList.push_back(newMove);
				}
			}
		}
		if (((board.Castling_check)	& castK)	!=	castK) {
			ExtMove newMove;
			if (EVALUATION::LVA(4, board, WHITE)	!=	-1	||
				EVALUATION::LVA(5, board, WHITE)	!=	-1	||
				EVALUATION::LVA(6, board, WHITE)	!=	-1) {}
			else if (board.Sq[5] == emptySqr && board.Sq[6] == emptySqr){
				newMove.move	=	(4 | 6 << 6 | KING_CASTLE << 12);
				Moves[bK]		=	BIT1 >> 6;
				newMove.value	=	250;
				moveList.push_back(newMove);
			}
		}
		if (((board.Castling_check)	& castQ)	!=	castQ){
			ExtMove newMove;
			if (EVALUATION::LVA(4, board, WHITE)	!=	-1	||
				EVALUATION::LVA(3, board, WHITE)	!=	-1	||
				EVALUATION::LVA(2, board, WHITE)	!=	-1) {}
			else if (board.Sq[1]	==	emptySqr && board.Sq[2]	==	emptySqr && board.Sq[3]	==	emptySqr){
				newMove.move	=	(4 | 2 << 6 | QUEEN_CASTLE << 12);
				Moves[bK]		=	BIT1 >> 2;
				newMove.value			=	250;
				moveList.push_back(newMove);
			}
		}
	}

	std::cout << "so far so good\n";

	for (int i = 0; i < 6; i++){
		while (Pieces[i] != 0){
			BitBoard		moveHolder;
			int position	=	BitOp::BitPop(Pieces[i]);
			moveHolder		=	Picker(i, position, ownPieces, enemyPieces, side, EMPTY_BRD);
			MoveEncoding(i, position, moveHolder, &moveList, enemyPieces, board);
			Moves[i]		|=	moveHolder;
		}	
	}	
	return moveList;
}

//return enpassant + castling moves
std::vector<ExtMove> SpecialMoves(BOARD_C board, int side){
	std::vector<ExtMove> moveList;
	BitBoard Pieces[15];
	BitBoard Moves[15];
	BitBoard enemyPieces, ownPieces;
	Castling_right	castK, castQ;
	for (int i = 0; i < 6; i++) Moves[i] = 0;
	if (side == WHITE){
		for (int i = 0; i < 6; i++)	Pieces[i]	=	board.Pieces[i];
		ownPieces	=	board.CurrentBoard[WHITE];
		enemyPieces	=	board.CurrentBoard[BLACK];
		castK		=	CastlingKW;
		castQ		=	CastlingQW;
		if (((board.PreviousMove >> 12) & 0x0f)	==	DOUBLE_PUSH){ //Check for enpassant, enpassant only work when there is a double pawn push by opp
			int TO	=	((board.PreviousMove >> 6) & 0x3f);
			if (board.Sq[TO + 1]	==	wP){
				ExtMove newMove;
				if (((TO + 1)%8) != 0) {
					newMove.move	=	((TO + 1) | ((TO - 8) << 6) | ENPASSANT << 12);
					newMove.value	=	41;
					moveList.push_back(newMove);
				}
            } 
			if (board.Sq[TO - 1]	==	wP){
				ExtMove newMove;
				if (((TO - 1)%8) != 7) {
					newMove.move	=	((TO - 1) | ((TO - 8) << 6) | ENPASSANT << 12);
					newMove.value	=	41;
					moveList.push_back(newMove);
				}
			}
		}
		if ((board.Castling_check &	castK) != castK) {
			if ((EVALUATION::LVA(60, board, BLACK)	!=	-1) || 
				(EVALUATION::LVA(61, board, BLACK)	!=	-1) ||
				(EVALUATION::LVA(62, board, BLACK)	!=	-1)) {} //check if castling kingside right into a check;
			else if (board.Sq[62] == emptySqr && board.Sq[61] == emptySqr){
				ExtMove newMove;
				newMove.move	=	(60 | 62 << 6 | KING_CASTLE << 12);
				Moves[wK]		=	BIT1 >> 62;
				newMove.value	=	250;
				moveList.push_back(newMove);
			}
		}
		if ((board.Castling_check & castQ) != castQ){	
			if (EVALUATION::LVA(60, board, BLACK)	!=	-1	||
				EVALUATION::LVA(59, board, BLACK)	!=	-1	||
				EVALUATION::LVA(58, board, BLACK)	!=	-1) {} //check if castling queenside right into a check;
			else if (board.Sq[57] == emptySqr&& board.Sq[58] == emptySqr && board.Sq[59] == emptySqr){
				ExtMove newMove;
				newMove.move	=	(60 | 58 << 6 | QUEEN_CASTLE << 12);
				Moves[wK]		=	BIT1 >> 58;
				newMove.value			=	250;
				moveList.push_back(newMove);
			}
		}
	} 
	return moveList;
}

//return all possible capture moves from a board configuration
std::vector<ExtMove> CaptureMoves(BOARD_C board, int side){
	BitBoard Pieces[12];
	BitBoard Moves[12];
	BitBoard enemyPieces, ownPieces;
	std::vector<ExtMove> moveList;
	for (int i = 0; i < 6; i++) Moves[i] = 0;
	if (side	==	WHITE){
		for (int i	=	0; i < 6; i++)	Pieces[i]	=	board.Pieces[i];
		ownPieces	=	board.CurrentBoard[WHITE];
		enemyPieces	=	board.CurrentBoard[BLACK];
	}
	else { 
		for (int i	=	0; i < 6; i++)	Pieces[i]	=	board.Pieces[i+6];
		ownPieces	=	board.CurrentBoard[BLACK];
		enemyPieces	=	board.CurrentBoard[WHITE];
	}
	for (int i = 0; i < 6; i++){
		while (Pieces[i] != 0){
			BitBoard		moveHolder;
			int position	=	BitOp::BitPop(Pieces[i]);
			moveHolder		=	Picker(i, position, ownPieces, enemyPieces, side, EMPTY_BRD) & enemyPieces;
			MoveEncoding(i, position, moveHolder, &moveList, enemyPieces, board);
			Moves[i]		|=	moveHolder;
		}
	}
	return moveList;
}

//return all quiet moves from a board configuration
std::vector<ExtMove> QuietMoves(BOARD_C board, int side){
	std::vector<ExtMove> moveList;
	BitBoard Pieces[15];
	BitBoard Moves[15];
	BitBoard enemyPieces, ownPieces;
	Castling_right	castK, castQ;
	for (int i = 0; i < 6; i++) Moves[i] = 0;
	if (side == WHITE){
		for (int i = 0; i < 6; i++)	Pieces[i]	=	board.Pieces[i];
		ownPieces	=	board.CurrentBoard[WHITE];
		enemyPieces	=	board.CurrentBoard[BLACK];
		castK		=	CastlingKW;
		castQ		=	CastlingQW;
		if (((board.PreviousMove >> 12) & 0x0f)	==	DOUBLE_PUSH){ //Check for enpassant, enpassant only work when there is a double pawn push by opp
			int TO	=	((board.PreviousMove >> 6) & 0x3f);
			if (board.Sq[TO + 1]	==	wP){
				ExtMove newMove;
				if (((TO + 1)%8) != 0) {
					newMove.move	=	((TO + 1) | ((TO - 8) << 6) | ENPASSANT << 12);
					newMove.value	=	41;
					moveList.push_back(newMove);
				}
            } 
			if (board.Sq[TO - 1]	==	wP){
				ExtMove newMove;
				if (((TO - 1)%8) != 7) {
					newMove.move	=	((TO - 1) | ((TO - 8) << 6) | ENPASSANT << 12);
					newMove.value	=	41;
					moveList.push_back(newMove);
				}
			}
		}
		if ((board.Castling_check &	castK) != castK) {
			if ((EVALUATION::LVA(60, board, BLACK)	!=	-1) || 
				(EVALUATION::LVA(61, board, BLACK)	!=	-1) ||
				(EVALUATION::LVA(62, board, BLACK)	!=	-1)) {} //check if castling kingside right into a check;
			else if (board.Sq[62] == emptySqr && board.Sq[61] == emptySqr){
				ExtMove newMove;
				newMove.move	=	(60 | 62 << 6 | KING_CASTLE << 12);
				Moves[wK]		=	BIT1 >> 62;
				newMove.value	=	250;
				moveList.push_back(newMove);
			}
		}
		if ((board.Castling_check & castQ) != castQ){	
			if (EVALUATION::LVA(60, board, BLACK)	!=	-1	||
				EVALUATION::LVA(59, board, BLACK)	!=	-1	||
				EVALUATION::LVA(58, board, BLACK)	!=	-1) {} //check if castling queenside right into a check;
			else if (board.Sq[57] == emptySqr&& board.Sq[58] == emptySqr && board.Sq[59] == emptySqr){
				ExtMove newMove;
				newMove.move	=	(60 | 58 << 6 | QUEEN_CASTLE << 12);
				Moves[wK]		=	BIT1 >> 58;
				newMove.value			=	250;
				moveList.push_back(newMove);
			}
		}
	} else { 
		for (int i	=	0; i < 6; i++)	Pieces[i] = board.Pieces[i+6];
		ownPieces	=	board.CurrentBoard[BLACK];
		enemyPieces	=	board.CurrentBoard[WHITE];
		castK		=	CastlingKB;
		castQ		=	CastlingQB;
		if (((board.PreviousMove >> 12) & 0x0f)	== DOUBLE_PUSH){
			int TO	=	((board.PreviousMove >> 6) & 0x3f);
			if (board.Sq[TO + 1] == bP){
				ExtMove newMove;
				if (((TO + 1)%8) != 0) {
					newMove.move	=	((TO + 1) | ((TO + 8) << 6) | ENPASSANT << 12);
					newMove.value	=	41;
					moveList.push_back(newMove);
				}
			} 
			if (board.Sq[TO - 1] == bP){
				ExtMove newMove;
				if (((TO - 1)%8) != 7) {
					newMove.move	=	((TO - 1) | ((TO + 8) << 6) | ENPASSANT << 12);
					newMove.value	=	41;
					moveList.push_back(newMove);
				}
			}
		}
		if (((board.Castling_check)	& castK)	!=	castK) {
			ExtMove newMove;
			if (EVALUATION::LVA(4, board, WHITE)	!=	-1	||
				EVALUATION::LVA(5, board, WHITE)	!=	-1	||
				EVALUATION::LVA(6, board, WHITE)	!=	-1) {}
			else if (board.Sq[5] == emptySqr && board.Sq[6] == emptySqr){
				newMove.move	=	(4 | 6 << 6 | KING_CASTLE << 12);
				Moves[bK]		=	BIT1 >> 6;
				newMove.value	=	250;
				moveList.push_back(newMove);
			}
		}
		if (((board.Castling_check)	& castQ)	!=	castQ){
			ExtMove newMove;
			if (EVALUATION::LVA(4, board, WHITE)	!=	-1	||
				EVALUATION::LVA(3, board, WHITE)	!=	-1	||
				EVALUATION::LVA(2, board, WHITE)	!=	-1) {}
			else if (board.Sq[1]	==	emptySqr && board.Sq[2]	==	emptySqr && board.Sq[3]	==	emptySqr){
				newMove.move	=	(4 | 2 << 6 | QUEEN_CASTLE << 12);
				Moves[bK]		=	BIT1 >> 2;
				newMove.value			=	250;
				moveList.push_back(newMove);
			}
		}
	}

	for (int i = 0; i < 6; i++){
		while (Pieces[i] != 0){
			BitBoard		moveHolder;
			int position	=	BitOp::BitPop(Pieces[i]);
			moveHolder		=	Picker(i, position, ownPieces, enemyPieces, side, EMPTY_BRD) & (~enemyPieces);;
			MoveEncoding(i, position, moveHolder, &moveList, enemyPieces, board);
			Moves[i]		|=	moveHolder;
		}	
	}	
	return moveList;
}

//return a bitboard with legal move squares based on a a piece's position and its type
BitBoard Picker(int chooser, int position, BitBoard ownPieces, BitBoard enemyPieces, int Color, BitBoard capture){
	BitBoard result;
	switch (chooser){
		case wP:	result	=	GENERATE::Pawn	( position,  ownPieces,  enemyPieces,  Color, capture);	break;
		case wB:	result	=	GENERATE::Bishop( position,  ownPieces,  enemyPieces);					break;
		case wR:	result	=	GENERATE::Rook	( position,  ownPieces,  enemyPieces);					break;
		case wQ:	result	=	GENERATE::Queen	( position,  ownPieces,  enemyPieces);					break;
		case wN:	result	=	GENERATE::Knight( position,  ownPieces,  enemyPieces);					break;
		case wK:	result	=	GENERATE::King	( position,  ownPieces,  enemyPieces);					break;
	}
	return result;
}

BitBoard Pawn(int pos, BitBoard ownPieces, BitBoard enemyPieces, int Color, BitBoard cap){
	BitBoard ValidMoves	=	0;
	BitBoard position	=	BIT1 >> pos;
	if (Color	==	WHITE){
		ValidMoves	|=	( position	<< 8 ) & ~( ownPieces | enemyPieces ) & ~cap;	
		ValidMoves  |=  (ValidMoves & Rank3) << 8 & ~( ownPieces | enemyPieces ) & ~cap;
		ValidMoves	|=	( (	( position	<< 7 ) & FileA )  ^ (position << 7 )	) & ( enemyPieces | cap );
		ValidMoves	|=	( (	( position	<< 9 ) & FileH )  ^ (position << 9 )	) & ( enemyPieces | cap );
	} else {
		ValidMoves	=	(position	>> 8 ) & ~( ownPieces | enemyPieces ) & ~cap;
		ValidMoves  |=  (ValidMoves & Rank6) >> 8 & ~( ownPieces | enemyPieces ) & ~cap;
		ValidMoves	|=	( (	( position	>> 7 ) & FileH )  ^ (position >> 7 )	) & ( enemyPieces | cap );
		ValidMoves	|=	( (	( position	>> 9 ) & FileA )  ^ (position >> 9 )	) & ( enemyPieces | cap );
	}
	return ValidMoves;
}

BitBoard Knight(int pos, BitBoard ownPieces, BitBoard enemyPieces){
	BitBoard ValidMoves	=	0;
	ValidMoves	=	MASK::NMask[pos] & ~ownPieces;
	return ValidMoves;
}

BitBoard Bishop(int pos, BitBoard ownPieces, BitBoard enemyPieces){
	BitBoard ValidMoves	=	0;
	int magicIndex;
	magicIndex	=	(int)( ((ownPieces | enemyPieces) & MASK::BMask[pos]) * magicNumberBishop[63 - pos] >> Shift_B[63 - pos]);
	ValidMoves	=	BishopMoveDatabase[pos][magicIndex] & ~ownPieces;
	return ValidMoves;
}

BitBoard Rook(int pos, BitBoard ownPieces, BitBoard enemyPieces){
	BitBoard ValidMoves	=	0;
	int magicIndex;
	magicIndex	=	(int)( ((ownPieces | enemyPieces) & MASK::RMask[pos]) * magicNumberRook[63 - pos] >> Shift_R[63 - pos]);
	ValidMoves	=	RookMoveDatabase[pos][magicIndex] & ~ownPieces;
	return ValidMoves;
}

BitBoard Queen(int pos, BitBoard ownPieces, BitBoard enemyPieces){
	BitBoard ValidMoves	=	0;
	int magicIndex;
	magicIndex	=	(int)( ((ownPieces | enemyPieces) & MASK::BMask[pos]) * magicNumberBishop[63 - pos] >> Shift_B[63 - pos]);
	ValidMoves	=	BishopMoveDatabase[pos][magicIndex] & ~ownPieces;
	magicIndex	=	(int)( ((ownPieces | enemyPieces) & MASK::RMask[pos]) * magicNumberRook[63 - pos] >> Shift_R[63 - pos]);
	ValidMoves	|=	RookMoveDatabase[pos][magicIndex] & ~ownPieces;
	return ValidMoves;
}

BitBoard King(int pos, BitBoard ownPieces, BitBoard enemyPieces){
	BitBoard ValidMoves	=	0;
	ValidMoves	=	MASK::KMask[pos] & ~ownPieces;
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

//It just take some times
//Little girl you in the middle of the ride
//Unfinished Undo Move