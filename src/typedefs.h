#ifndef TYPEDEFS_H_
#define TYPEDEFS_H_

#include <stdint.h>
#include <iostream>

typedef uint64_t BitBoard;
typedef uint16_t Move;
typedef uint64_t Key;
typedef uint64_t BitString;
typedef uint64_t HashKey;

//int H_Heuristic[64][64]; //[from][to]

/*
 * 16 bit is needed to encode a move
 * 0-5 	 from
 * 6-10	 to
 * 11-16 ( Move Type )
 */ 

const char 	 	acter [13]	=	{'P', 'N', 'B', 'R', 'Q', 'K', 'p', 'n', 'b', 'r', 'q', 'k', '.'};

const int 		MAX_MOVES = 256;
const int 		MAX_PLY   = 128;

const uint64_t 	UNIVERSE  = 0xffffffffffffffff;
const uint64_t 	BIT1	  = 0x8000000000000000;
const uint64_t 	EMPTY_BRD = 0x0000000000000000;


const int		MAX_VALUE =	65536;
const int 		MIN_VALUE =	-65536;
const uint8_t	BRD_SQR	  =	64;
const std::string	STANDARD	=	"rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 0 ";

#define WHITE	0
#define BLACK	1

#define emptySqr	12

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

enum MoveType {
	QUIET_MOVES		=	0,	DOUBLE_PUSH		=	1,	KING_CASTLE		=	2,
	QUEEN_CASTLE	=	3,	CAPTURE			=	4,	K_PROMO			=	8,
	B_PROMO			=	9,	R_PROMO			=	10,	Q_PROMO			=	11,
	K_PROMO_CAP		=	12,	B_PROMO_CAP		=	13,	R_PROMO_CAP		=	14,
	Q_PROMO_CAP		=	15, ENPASSANT		=	5
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
	const int Q_VALUE	=	970;
	const int R_VALUE	=	510;
}

namespace MID{
	const int P_VALUE	=	105;
	const int N_VALUE	=	305;
	const int B_VALUE	=	320;
	const int K_VALUE	=	15536;
	const int Q_VALUE	=	910;
	const int R_VALUE	=	520;
}

namespace END{
	const int P_VALUE	=	105;
	const int N_VALUE	=	310;
	const int B_VALUE	=	340;
	const int K_VALUE	=	15536;
	const int Q_VALUE	=	910;
	const int R_VALUE	=	510;
}

const int	VALUE[13]	=	{BEGIN::P_VALUE, BEGIN::N_VALUE, BEGIN::B_VALUE, BEGIN::R_VALUE, BEGIN::Q_VALUE, BEGIN::K_VALUE, 
							 BEGIN::P_VALUE, BEGIN::N_VALUE, BEGIN::B_VALUE, BEGIN::R_VALUE, BEGIN::Q_VALUE, BEGIN::K_VALUE, 0};

struct ExtMove {
	Move move;
	int value;
	
	uint16_t getMove() const { return move; }
	
	void operator=(ExtMove m) { move = m.move; value	=	m.value;	}
	
	bool operator>(ExtMove m) { return  value > m.value; }
	
	uint8_t getTo() const {return move >> 6 & 0x3f;}
	
	uint8_t getFrom() const {return (move) & 0x3f;}
	
	uint8_t getFlags() const {return (move >> 12) & 0x0f;}
};

#endif 
