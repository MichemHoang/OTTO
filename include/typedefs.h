#ifndef TYPEDEFS_H_
#define TYPEDEFS_H_

#include <stdint.h>
#include <iostream>

#define WHITE	0
#define BLACK	1

#define emptySqr 12

#define	wP		0
#define wN		1
#define wB		2
#define	wR		3
#define	wQ		4
#define	wK		5

#define bP		6
#define bN		7
#define bB		8
#define bR		9
#define	bQ		10
#define	bK		11

typedef uint64_t BitBoard;
typedef uint16_t Move;
typedef uint64_t Key;
typedef uint64_t BitString;
typedef uint64_t HashKey;

/*
 * 16 bit is needed to encode a move (2^6 = 64)
 * Bit 0-5 	 from
 * Bit 6-10	 to
 * 11-16 ( Move Type ) 2^4 = 16
 */ 

const int 			MAX_MOVES = 120;	//Total number of moves (1 move = 2 plys)
const int 			MAX_PLY   = 128;    //Total number of ply (or half move)
const BitBoard 		UNIVERSE  = 0xffffffffffffffff;
const BitBoard 		BIT1	  = 0x8000000000000000;
const BitBoard 		EMPTY_BRD = 0x0000000000000000;
const int			MAX_VALUE =	65536;
const int 			MIN_VALUE =	-65536;
const std::string	STANDARD  =	"rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 0";
const char 			Character [13] =	{'P', 'N', 'B', 'R', 'Q', 'K', 'p', 'n', 'b', 'r', 'q', 'k', '.'};

const std::string 	squares[64] = {
	"A8", "B8", "C8", "D8", "E8", "F8", "G8", "H8",
	"A7", "B7", "C7", "D7", "E7", "F7", "G7", "H7",
	"A6", "B6", "C6", "D6", "E6", "F6", "G6", "H6",
	"A5", "B5", "C5", "D5", "E5", "F5", "G5", "H5",
	"A4", "B4", "C4", "D4", "E4", "F4", "G4", "H4",
	"A3", "B3", "C3", "D3", "E3", "F3", "G3", "H3",
	"A2", "B2", "C2", "D2", "E2", "F2", "G2", "H2",
	"A1", "B1", "C1", "D1", "E1", "F1", "G1", "H1"
};

enum Signal {
	TIMEOUT			=	-2,
	BEGIN_SEARCH 	= 	2,
	SHOWTIME		=	-1,
	SEARCHING		=	0,
	WAITING			=	1
};

enum MoveType {
	QUIET_MOVES		=	0,	DOUBLE_PUSH		=	1,	KING_CASTLE		=	2,
	QUEEN_CASTLE	=	3,	CAPTURE			=	4,	ENPASSANT		=	5,
	K_PROMO			=	8,  B_PROMO			=	9,	R_PROMO			=	10,	
	Q_PROMO			=	11, K_PROMO_CAP		=	12,	B_PROMO_CAP		=	13,	
	R_PROMO_CAP		=	14, Q_PROMO_CAP		=	15, 
};

enum Castling_right{
	CastlingKW		=	0x8 ,		CastlingQW		=	0x4,
	CastlingKB		=	0x8 << 4,	CastlingQB		=	0x4 << 4
};

namespace BEGIN{
	const int P_VALUE	=	105;
	const int N_VALUE	=	320;
	const int B_VALUE	=	340;
	const int K_VALUE	=	15536;
	const int Q_VALUE	=	910;
	const int R_VALUE	=	510;
}

//This seems useless? 
const int VALUE[13]	= {BEGIN::P_VALUE, BEGIN::N_VALUE, BEGIN::B_VALUE, BEGIN::R_VALUE, BEGIN::Q_VALUE, BEGIN::K_VALUE, 
					   BEGIN::P_VALUE, BEGIN::N_VALUE, BEGIN::B_VALUE, BEGIN::R_VALUE, BEGIN::Q_VALUE, BEGIN::K_VALUE, 0};

//Move with value and its functions
struct ExtMove {
	Move move;
	int value;		//initial value
	int searchValue; //value after search

	void operator=(ExtMove m) { move = m.move; value = m.value;	}
	bool operator>(ExtMove m) { return  value > m.value; }
	bool operator<(ExtMove m) { return  value < m.value; }

    Move getMove() const { return move; }
	uint8_t getTo() const {return move >> 6 & 0x3f;}  //value of bit 6 to 10 
	uint8_t getFrom() const {return (move) & 0x3f;}  // value of first 6 bit
    uint8_t getFlags() const {return (move >> 12) & 0x0f;} // value of bit 12 - 15
	std::string toString(){
		std::string	from = squares[(move & 0x3F) + 1];
		std::string to = squares[(move >> 6 & 0x3f) + 1];
		std::string flags = std::to_string((move >> 12) & 0xF);
		std::string quickValue = std::to_string(value);
		std::string final = from + "->" + to + " flags = " + flags + " Initial value = " + quickValue + "\n";
		return final;
	}
};

#endif 
