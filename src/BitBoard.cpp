#include "BitBoard.h"

using namespace std;

const int index64[64] = {
    0,  1, 48,  2, 57, 49, 28,  3,
   61, 58, 50, 42, 38, 29, 17,  4,
   62, 55, 59, 36, 53, 51, 43, 22,
   45, 39, 33, 30, 24, 18, 12,  5,
   63, 47, 56, 27, 60, 41, 37, 16,
   54, 35, 52, 21, 44, 32, 23, 11,
   46, 26, 40, 15, 34, 20, 31, 10,
   25, 14, 19,  9, 13,  8,  7,  6
};

const int indexR64[64] = {
    0, 47,  1, 56, 48, 27,  2, 60,
   57, 49, 41, 37, 28, 16,  3, 61,
   54, 58, 35, 52, 50, 42, 21, 44,
   38, 32, 29, 23, 17, 11,  4, 62,
   46, 55, 26, 59, 40, 36, 15, 53,
   34, 51, 20, 43, 31, 22, 10, 45,
   25, 39, 14, 33, 19, 30,  9, 24,
   13, 18,  8, 12,  7,  6,  5, 63
};

string		Digit[10]	=	{"0", "1", "2", "3", "4", "5", "6", "7", "8", "9"};
char		Castling_Condition[5]	=	{'K', 'Q', 'k', 'q', '-'};
uint8_t		Castling_Value[5]		=	{CastlingKW, CastlingQW, CastlingKB, CastlingQB, 0};

uint8_t		BitCount[65536];
BitBoard	Bishop_AttackMask[64];
BitBoard 	Rook_AttackMask[64];
BitBoard	Knight_AttackMask[64];
BitBoard	King_AttackMask[64];
BitBoard	Pawn_AttackMask[2][64];

/*
 *Copyright (C) 2007 Pradyumna Kannan. 
 */
 
BitBoard magicNumberRook[] = {
	(0x0080001020400080), (0x0040001000200040), (0x0080081000200080), (0x0080040800100080),
	(0x0080020400080080), (0x0080010200040080), (0x0080008001000200), (0x0080002040800100),
	(0x0000800020400080), (0x0000400020005000), (0x0000801000200080), (0x0000800800100080),
	(0x0000800400080080), (0x0000800200040080), (0x0000800100020080), (0x0000800040800100),
	(0x0000208000400080), (0x0000404000201000), (0x0000808010002000), (0x0000808008001000),
	(0x0000808004000800), (0x0000808002000400), (0x0000010100020004), (0x0000020000408104),
	(0x0000208080004000), (0x0000200040005000), (0x0000100080200080), (0x0000080080100080),
	(0x0000040080080080), (0x0000020080040080), (0x0000010080800200), (0x0000800080004100),
	(0x0000204000800080), (0x0000200040401000), (0x0000100080802000), (0x0000080080801000),
	(0x0000040080800800), (0x0000020080800400), (0x0000020001010004), (0x0000800040800100),
	(0x0000204000808000), (0x0000200040008080), (0x0000100020008080), (0x0000080010008080),
	(0x0000040008008080), (0x0000020004008080), (0x0000010002008080), (0x0000004081020004),
	(0x0000204000800080), (0x0000200040008080), (0x0000100020008080), (0x0000080010008080),
	(0x0000040008008080), (0x0000020004008080), (0x0000800100020080), (0x0000800041000080),
	(0x00FFFCDDFCED714A), (0x007FFCDDFCED714A), (0x003FFFCDFFD88096), (0x0000040810002101),
	(0x0001000204080011), (0x0001000204000801), (0x0001000082000401), (0x0001FFFAABFAD1A2)
};

BitBoard magicNumberBishop[] = {
	(0x0002020202020200), (0x0002020202020000), (0x0004010202000000), (0x0004040080000000),
	(0x0001104000000000), (0x0000821040000000), (0x0000410410400000), (0x0000104104104000),
	(0x0000040404040400), (0x0000020202020200), (0x0000040102020000), (0x0000040400800000),
	(0x0000011040000000), (0x0000008210400000), (0x0000004104104000), (0x0000002082082000),
	(0x0004000808080800), (0x0002000404040400), (0x0001000202020200), (0x0000800802004000),
	(0x0000800400A00000), (0x0000200100884000), (0x0000400082082000), (0x0000200041041000),
	(0x0002080010101000), (0x0001040008080800), (0x0000208004010400), (0x0000404004010200),
	(0x0000840000802000), (0x0000404002011000), (0x0000808001041000), (0x0000404000820800),
	(0x0001041000202000), (0x0000820800101000), (0x0000104400080800), (0x0000020080080080),
	(0x0000404040040100), (0x0000808100020100), (0x0001010100020800), (0x0000808080010400),
	(0x0000820820004000), (0x0000410410002000), (0x0000082088001000), (0x0000002011000800),
	(0x0000080100400400), (0x0001010101000200), (0x0002020202000400), (0x0001010101000200),
	(0x0000410410400000), (0x0000208208200000), (0x0000002084100000), (0x0000000020880000),
	(0x0000001002020000), (0x0000040408020000), (0x0004040404040000), (0x0002020202020000),
	(0x0000104104104000), (0x0000002082082000), (0x0000000020841000), (0x0000000000208800),
	(0x0000000010020200), (0x0000000404080200), (0x0000040404040400), (0x0002020202020200)
};

int Shift_R[] = {
	52, 53, 53, 53, 53, 53, 53, 52,
	53, 54, 54, 54, 54, 54, 54, 53,
	53, 54, 54, 54, 54, 54, 54, 53,
	53, 54, 54, 54, 54, 54, 54, 53,
	53, 54, 54, 54, 54, 54, 54, 53,
	53, 54, 54, 54, 54, 54, 54, 53,
	53, 54, 54, 54, 54, 54, 54, 53,
	52, 53, 53, 53, 53, 53, 53, 52
}; 

int Shift_B[] = {
    58, 59, 59, 59, 59, 59, 59, 58,
	59, 59, 59, 59, 59, 59, 59, 59,
	59, 59, 57, 57, 57, 57, 59, 59,
	59, 59, 57, 55, 55, 57, 59, 59,
	59, 59, 57, 55, 55, 57, 59, 59,
	59, 59, 57, 57, 57, 57, 59, 59,
	59, 59, 59, 59, 59, 59, 59, 59,
	58, 59, 59, 59, 59, 59, 59, 58
};

//End of Copyright

const BitBoard FileA = 0x0101010101010101ULL;			
const BitBoard FileB = FileA << 1;
const BitBoard FileC = FileA << 2;
const BitBoard FileD = FileA << 3;
const BitBoard FileE = FileA << 4;
const BitBoard FileF = FileA << 5;
const BitBoard FileG = FileA << 6;
const BitBoard FileH = FileA << 7;

const BitBoard Rank1 = 0xFF;
const BitBoard Rank2 = Rank1 << (8 * 1);
const BitBoard Rank3 = Rank1 << (8 * 2);
const BitBoard Rank4 = Rank1 << (8 * 3);
const BitBoard Rank5 = Rank1 << (8 * 4);
const BitBoard Rank6 = Rank1 << (8 * 5);
const BitBoard Rank7 = Rank1 << (8 * 6);
const BitBoard Rank8 = Rank1 << (8 * 7);

const BitBoard Edges   	= FileA | FileH | Rank1 | Rank8;		// Edge of the Board
char 	Character [13]	=	{'P', 'N', 'B', 'R', 'Q', 'K', 'p', 'n', 'b', 'r', 'q', 'k', '.'};
BitBoard *RookMoveDatabase[64];
BitBoard *BishopMoveDatabase[64];
BitBoard AttackRay[8][64];

const uint64_t debruijn641  = 0x03f79d71b4cb0a89;
const uint64_t debruijn64 	= 0x03f79d71b4cb0a89;

//Thanks Debrujins

const BitBoard DarkSquares 	= 0xAA55AA55AA55AA55ULL;

bool isSubsetOf	( BitBoard a, BitBoard b) {return (a & b) == a;}

int LSBit		( BitBoard bb ){
	if ( bb == 0x00 ) return 64;
	return 63 - index64[((bb & -bb) * debruijn64) >> 58];
}

int MSBit		(BitBoard bb) {
   bb |= bb >> 1; 
   bb |= bb >> 2;
   bb |= bb >> 4;
   bb |= bb >> 8;
   bb |= bb >> 16;
   bb |= bb >> 32;
   return indexR64[(bb * debruijn641) >> 58];
}

void PrintBitBoard	( BitBoard A ){
	BitBoard	temp;
	for (int i	=	0;	i < 64; i++){
		temp	=	BIT1 >> i;
		if ((temp & A) == temp )
			cout	<< "1  ";
		else cout	<< ".  ";
		if ( (i+1) % 8 == 0 )	cout << endl;
	}
	cout	<< "-------------------------------\n";
}

int BitPop			( BitBoard &A ){
	if ( A	==	0 ) return -1;
	int pos		=	LSBit(A);
	A	^=	(BIT1 >> pos);
	return pos;
}

int BitPopR			( BitBoard &A ){
	if ( A	==	0 ) return -1;
	int pos		=	MSBit(A);
	A	^=	(BIT1 >> ( 63 - pos ) );
	return ( 63 - pos );
}

void	PrintSQ			( uint8_t SQ[]){
	cout	<< "===========Print SQR==========\n\n";
	for (int i = 0; i < 64; i++){
		cout	<< Character[SQ[i]] << "  ";
		if ((i+1) % 8 == 0 ) cout << endl;
	}
	cout	<< "\n==============END=============\n";
}

void GenerateMask	(){
	//for (int i = 0; i < 64; i++)
		//for (int j = 0; j < 64; j++)
			//H_Heuristic[i][j] = 0;
	BitBoard	o;
	for (int i	= 0; i< 65536; i++){
		o	=	i;
		BitCount[i]	=	population(o);
	}
	BitBoard	temp;		
	//GenerateAttackMovefor Pawn
	for (int i	= 8; i< 56; i ++){
		BitBoard	position	=	BIT1 >> i;
		Pawn_AttackMask[0][i]	=	0;
		Pawn_AttackMask[0][i]	=	( (	( position	<< 7 ) & FileH )  ^ (position << 7 )	) ;
		Pawn_AttackMask[0][i]	|=	( (	( position	<< 9 ) & FileA )  ^ (position << 9 )	) ;
		
		Pawn_AttackMask[1][i]	=	0;
		Pawn_AttackMask[1][i]	=	( (	( position	>> 7 ) & FileA )  ^ (position >> 7 )	) ;
		Pawn_AttackMask[1][i]	|=	( (	( position	>> 9 ) & FileH )  ^ (position >> 9 )	) ;
	}
	
	//Generate moves for king
	for (int i = 0; i < 64; i++){
		temp	=	BIT1	>> i;
		if ( i % 8 != 7 )			King_AttackMask[i]	|=	( temp >> 1 | temp << 7 | temp >> 9);
		if ( i % 8 != 0 )			King_AttackMask[i]	|=	( temp << 1 | temp << 9 | temp >> 7);
		King_AttackMask[i]	|=	temp >> 8;
		King_AttackMask[i]	|=	temp << 8;
	}
	//Generate moves for Knight
	for (int i = 0; i < 64; i++){
		temp	=	BIT1	>> i;
		if ( i % 8 < 7 )			Knight_AttackMask[i] |=	( temp << 15 | temp >> 17 );
		if ( i % 8 < 6 ) 			Knight_AttackMask[i] |=	( temp << 6  | temp >> 10 );
		if ( i % 8 > 0 )			Knight_AttackMask[i] |= ( temp << 17 | temp >> 15 );
		if ( i % 8 > 1 )			Knight_AttackMask[i] |= ( temp << 10 | temp >> 6  );
	}
	//Generate moves for Rook { 6 = S, 2 = N, 4 = E, 0 = W } 
	for (int i = 0; i < 64; i++){
		for (int j	=	i+8; j < 64; j+=8)	{	AttackRay[6][i]	|=	( BIT1 >> j );	if (j >= 56			) break;	Rook_AttackMask[i]	|=	( BIT1 >> j );	}
		for (int j 	=	i-8; j >=0 ; j-=8)	{	AttackRay[2][i]	|=	( BIT1 >> j );	if (j < 8 			) break;	Rook_AttackMask[i]	|=	( BIT1 >> j );	}
		for (int j	=	i+1; j < 64; j++ )	{	AttackRay[4][i]	|=	( BIT1 >> j );	if (j%8==0 || j%8==7) break;	Rook_AttackMask[i]	|=	( BIT1 >> j );	}
		for (int j	=	i-1; j >=0 ; j-- )	{	AttackRay[0][i]	|=	( BIT1 >> j );	if (j%8==0 || j%8==7) break;	Rook_AttackMask[i]	|=	( BIT1 >> j );	}	
	}
	//Generate moves for Bishop
	for (int i = 0; i < 64; i++){
		for (int j	=	i+9; j < 64; j+=9)	{	AttackRay[5][i]	|=	( BIT1 >> j );	if (j%8==0 || j%8==7) break; Bishop_AttackMask[i]	|=	( BIT1 >> j ); }
		for (int j 	=	i-9; j >=0 ; j-=9)	{	AttackRay[1][i]	|=	( BIT1 >> j );	if (j%8==0 || j%8==7) break; Bishop_AttackMask[i]	|=	( BIT1 >> j ); }
		for (int j	=	i+7; j < 64; j+=7)	{	AttackRay[7][i]	|=	( BIT1 >> j );	if (j%8==0 || j%8==7) break; Bishop_AttackMask[i]	|=	( BIT1 >> j ); }
		for (int j	=	i-7; j >=0 ; j-=7 )	{	AttackRay[3][i]	|=	( BIT1 >> j );	if (j%8==0 || j%8==7) break; Bishop_AttackMask[i]	|=	( BIT1 >> j ); }
		Bishop_AttackMask[i]	&=		~Edges;		
		
	}
}

/* 
 * Planet earth is blue and there's nothing i can do.
 */

void GenerateMoveData(){
	int variation[12];
	BitBoard	mask, Var, validMoves;
	int length, magicIndex;
	for (int i = 0; i < 64; i++){
		mask	=	Rook_AttackMask[i];	
		for (int jj	=	0; jj < 12; jj++){
			variation [jj]	=	BitPop(mask);
			if (variation[jj]	==	-1) break;
		}
		length	=	1L << (64 - Shift_R[63-i]);
		RookMoveDatabase[i]	=	new BitBoard[length];
		for (int k	=	1; k < length; k++){
			validMoves	=	0;
			BitBoard tmp	=	k;
			Var		=	Rook_AttackMask[i];
			while (tmp != 0){
				int a	=	BitPop(tmp);
				if (a == -1) break;
				Var		^=	(BIT1 >> 	variation[63 - a]);
			}	
			magicIndex		=	(int)( (Var * magicNumberRook[63 - i]) >> Shift_R[63 - i] );
			for (int j=i+8; j<=63; j+=8) 			{ validMoves |= (BIT1 >> j); if ((Var & (BIT1 >> j)) != 0) break; }
			for (int j=i-8; j>=0 ; j-=8) 			{ validMoves |= (BIT1 >> j); if ((Var & (BIT1 >> j)) != 0) break; }
			for (int j=i+1; j%8!=0; j++) 			{ validMoves |= (BIT1 >> j); if ((Var & (BIT1 >> j)) != 0) break; }
            for (int j=i-1; j%8!=7 && j>=0; j--) 	{ validMoves |= (BIT1 >> j); if ((Var & (BIT1 >> j)) != 0) break; }
			RookMoveDatabase[i][magicIndex]		=	validMoves;
		}	
		//===============================================================================
		mask	=	Bishop_AttackMask[i];	
		for (int jj	=	0; jj < 12; jj++){
			variation [jj]	=	BitPop(mask);
			if (variation[jj]	==	-1) break;
		}
		length	=	1L << (64 - Shift_B[63-i]);
		BishopMoveDatabase[i]	=	new BitBoard[length];
		for (int k	=	0; k < length; k++){
			validMoves	=	0;
			BitBoard tmp	=	k;
			Var		=	Bishop_AttackMask[i];
			while (tmp != 0){
				int a	=	BitPop(tmp);
				if (a == -1) break;
				Var		^=	(BIT1 >> 	variation[63 - a]);
			}	
			magicIndex		=	(int)( (Var * magicNumberBishop[63 - i]) >> Shift_B[63 - i] );
			for (int j=i+9; j%8!=0 && j<=63; j+=9) 		{ validMoves |= (BIT1 >> j); if ((Var & (BIT1 >> j)) != 0) break; }
			for (int j=i-9; j%8!=7 && j>=0 ; j-=9) 		{ validMoves |= (BIT1 >> j); if ((Var & (BIT1 >> j)) != 0) break; }
			for (int j=i+7; j%8!=7 && j<=63; j+=7)		{ validMoves |= (BIT1 >> j); if ((Var & (BIT1 >> j)) != 0) break; }
            for (int j=i-7; j%8!=0 && j>=0 ; j-=7) 		{ validMoves |= (BIT1 >> j); if ((Var & (BIT1 >> j)) != 0) break; }
			BishopMoveDatabase[i][magicIndex]		=	validMoves;
		}	
	}
}

int	population	( BitBoard C ){
	int population	=	0;
	BitBoard	A	=	C;
	while ( A != 0 ){
		BitPop(A);
		population++;
	}
	return population;
}

int PopsCount	( BitBoard C ){
	int Res	=	0;
	uint16_t index;
	for (int scalar	=	0; scalar < 4; scalar++){
		index	=	C >> (16*scalar);
		Res		+=	BitCount[index];
	}
	return Res;
	
}

void	getBoardInfo	(BOARD A){
	PrintSQ(A.Sq);
	if (A.Side_to_move	==	WHITE)	cout	<< "WHITE TURN\n";
	else 							cout	<< "BLACK TURN\n";
}

int		Char_To_int	(char Mx, int ii){
	if (ii == 1)	for (int i = 0; i < 12; i++){if (Mx	==	acter[i]) return i;}
	else 			for (int i = 0; i < 5; i++)	{if (Mx	==	Castling_Condition[i]) return i;}
	return -1;
}

void	READ_FEN(string FEN_STRING, BOARD *A) {
	int BrdIter	=	0;
	A	-> CurrentBoard[0]	=	0;
	A	-> CurrentBoard[1]	=	0;
	for (int j	=	0; j < 12; j++)	A->Pieces[j]	=	EMPTY_BRD;
	for (int t 	=	0; t < 64; t++)	A->Sq[t]		=	emptySqr;
	int Length	=	FEN_STRING.length();
	int iter = 0;
	for (iter = 0; iter < Length; iter++){
		char Mon	=	FEN_STRING[iter];
		if ((int)Mon < 58) {
			if (Mon != '/') BrdIter	+=	(int)Mon - 48;}
		else {
			int Pcs			=	Char_To_int(Mon, 1);
			A->Sq[BrdIter]	= 	Pcs;
			A->Pieces[Pcs]	|=	BIT1 >> BrdIter;
			if (Pcs < 6) 	A->CurrentBoard[0]	|=	(BIT1 >> BrdIter);	
			else  			A->CurrentBoard[1]	|=	(BIT1 >> BrdIter);	
			BrdIter++;		
		}
		if (BrdIter > 63) break;
	}
	iter+=2;
	FEN_STRING[iter] == 'w'? A->Side_to_move = WHITE: A->Side_to_move = BLACK;
	iter+=2;
	A->Castling_check	=	0xEE;
	while (FEN_STRING[iter]!= ' '){
		int Px	=	Char_To_int(FEN_STRING[iter], 15);
		A->Castling_check	&=	~Castling_Value[Px];
		iter++;
	}
	iter+=5;
	uint8_t	Moves	=	((int)FEN_STRING[iter] - 48);
	if (FEN_STRING[++iter]!= ' ') {
		Moves	*=	10;
		Moves	+=	(int)FEN_STRING[iter] - 48;
	}
	A->No_Ply	=	Moves*2;
}

string	toFEN	(BOARD A){
	string	FEN_STRING	=	"";
	int		counter;
	for (int i = 0; i < 8; i++){
		counter	=	0;
		for (int j = 0; j < 8; j++){
			if (A.Sq[i*8 + j]	==	emptySqr)	counter++;
			else {
				if (counter != 0) FEN_STRING +=	Digit[counter];
				FEN_STRING	+=	acter[A.Sq[i*8 + j]];
				counter = 0;
			}
		}
		if (counter != 0)	FEN_STRING	+=	Digit[counter];
		if (i < 7) FEN_STRING	+=	"/";
	}
	FEN_STRING	+=	" ";
	int a	=	0;
	A.Side_to_move	== WHITE? FEN_STRING	+= "w " : FEN_STRING	+= "b ";
	if ((A.Castling_check & CastlingKW) != CastlingKW) {FEN_STRING	+=	"K"; a++;}
	if ((A.Castling_check & CastlingQW) != CastlingQW) {FEN_STRING	+=	"Q"; a++;}
	if ((A.Castling_check & CastlingKB) != CastlingKB) {FEN_STRING	+=	"k"; a++;}
	if ((A.Castling_check & CastlingQB) != CastlingQB) {FEN_STRING	+=	"q"; a++;}
	if (a == 0) FEN_STRING	+= "-";
	FEN_STRING	+= " - ";
	int HalfMoveClock	=	0;
	FEN_STRING	+= std::to_string(HalfMoveClock) + " ";
	a	=	(int)A.No_Ply/2;
	FEN_STRING	+= std::to_string(a) + " ";
	return FEN_STRING;
}






