#include "board.h"

char 	 	Pieces [13]	=	{'P', 'N', 'B', 'R', 'Q', 'K', 'p', 'n', 'b', 'r', 'q', 'k', '.'};
char		Castling[5]	=	{'K', 'Q', 'k', 'q', '-'};
uint8_t		Castling_Val[5]		=	{CastlingKW, CastlingQW, CastlingKB, CastlingQB, 0};

std::string BOARD_C::toFENString(){
    std::string	FEN_STRING	= "";
	int	counter;
	for (int i = 0; i < 8; i++){
		counter	=	0;
		for (int j = 0; j < 8; j++){
			if (Sq[i*8 + j]	==	emptySqr)	counter++;
			else {
				if (counter != 0) FEN_STRING += std::to_string(counter);
				FEN_STRING	+=	Pieces[Sq[i*8 + j]];
				counter = 0;
			}
		}
		if (counter != 0)	FEN_STRING	+=	std::to_string(counter);
		if (i < 7) FEN_STRING	+=	"/";
	}
	FEN_STRING  +=	" ";
	int a	=	0;
	Side_to_move	== WHITE? FEN_STRING	+= "w " : FEN_STRING	+= "b ";
	if ((Castling_check & CastlingKW) != CastlingKW) {FEN_STRING	+=	"K"; a++;}
	if ((Castling_check & CastlingQW) != CastlingQW) {FEN_STRING	+=	"Q"; a++;}
	if ((Castling_check & CastlingKB) != CastlingKB) {FEN_STRING	+=	"k"; a++;}
	if ((Castling_check & CastlingQB) != CastlingQB) {FEN_STRING	+=	"q"; a++;}
	if (a == 0) FEN_STRING	+= "-";
	FEN_STRING	+= " - ";
	int HalfMoveClock	=	0;
	FEN_STRING	+= std::to_string(HalfMoveClock) + " ";
	a	=	(int)No_Ply/2;
	FEN_STRING	+= std::to_string(a) + " ";
	return FEN_STRING;
}

void BOARD_C::printBoard(){
    BitOp::PrintSQ(Sq);
    if (Side_to_move	==	WHITE)	std::cout	<< "WHITE TURN\n";
    else 							std::cout	<< "BLACK TURN\n";
}

BOARD_C BOARD_C::makeMove (ExtMove transformer){
    Move newMove = transformer.move;
    return makeMove(newMove);
}

BOARD_C BOARD_C::makeMove (Move transformer){
    BOARD_C newboard = *this;
    uint8_t	from	=	transformer & 0x3f;
	uint8_t to		=	transformer >> 6 & 0x3f;
	uint8_t flag	=	transformer >> 12 & 0x0f;;
	if (newboard.Sq[from] == (wR + 6*newboard.Side_to_move)){
		if 		(from	==	(63 - (56*newboard.Side_to_move)))	newboard.Castling_check |=	0x08 << (4*newboard.Side_to_move);
		else if (from	==	(56 - (56*newboard.Side_to_move)))	newboard.Castling_check |=	0x04 << (4*newboard.Side_to_move);
	}
	if (newboard.Sq[from] == (wK + 6*newboard.Side_to_move)){
		newboard.Castling_check |=	(0xC << (4*newboard.Side_to_move));
	}
	newboard.Pieces[newboard.Sq[from] ]	=	(newboard.Pieces[newboard.Sq[from] ]	& ~(BIT1 >> from)) | (BIT1 >> to);
	newboard.Pieces[newboard.Sq[to] ]	=	newboard.Pieces[newboard.Sq[to]	]	& ~(BIT1 >> to);
	newboard.Sq[to]		=	newboard.Sq[from];
	newboard.Sq[from]	=	emptySqr; 
	if (flag	==	KING_CASTLE){
		newboard.Pieces[newboard.Side_to_move*6 +	wR]	=	(newboard.Pieces[newboard.Side_to_move*6 +	wR]	& ~(BIT1 >> (from + 3))) | (BIT1 >> (from + 1));
		newboard.Sq[from + 1]	=	newboard.Sq[from + 3];
		newboard.Sq[from + 3]	=	emptySqr;
		newboard.Castling_check	|=	( 0xE << (newboard.Side_to_move*4));
	}
	if (flag	==	QUEEN_CASTLE){
		newboard.Pieces[newboard.Side_to_move*6 +	wR]	=	(newboard.Pieces[newboard.Side_to_move*6 +	wR]	& ~BIT1 >> (from -4)) | (BIT1 >> (from -1));
		newboard.Sq[from - 1]	=	newboard.Sq[from - 4];
		newboard.Sq[from - 4]	=	emptySqr;
		newboard.Castling_check	|=	( 0xE << (newboard.Side_to_move*4));
	}
	if (flag > 7) {
		newboard.Pieces[newboard.Side_to_move]	^=	(BIT1 >> to);
		newboard.Pieces[(flag - 8) % 4 + 1 + 6*newboard.Side_to_move]		|=	(BIT1 >> to);
		newboard.Sq[to]		=	(flag - 8) % 4 + 1 + 6*newboard.Side_to_move;
	}
	if (flag == ENPASSANT){
		if (newboard.Side_to_move == WHITE){
			newboard.Pieces[bP]	&=	~(BIT1 >> (to + 8));
			newboard.Sq[to + 8]	=	emptySqr;
		} else {
			newboard.Pieces[wP]	&=	~(BIT1 >> (to - 8));
			newboard.Sq[to - 8]	=	emptySqr;
		}
	}
	newboard.CurrentBoard[0]	=	newboard.Pieces[wP] | newboard.Pieces[wN] | newboard.Pieces[wR] | newboard.Pieces[wB] | newboard.Pieces[wK] | newboard.Pieces[wQ];
	newboard.CurrentBoard[1]	=	newboard.Pieces[bP] | newboard.Pieces[bN] | newboard.Pieces[bR] | newboard.Pieces[bB] | newboard.Pieces[bK] | newboard.Pieces[bQ];
	newboard.Side_to_move	^=	0x01;
	newboard.PreviousMove	=	transformer;//
	newboard.No_Ply++;
	return newboard;
}

BOARD_C BOARD_C::undoMove (ExtMove to){

}