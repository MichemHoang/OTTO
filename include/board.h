#ifndef BOARD_H_
#define BOARD_H_

#include "bitboard.h"

class BOARD_C{
public:
    BitBoard	Pieces[13];
    BitBoard	CurrentBoard[2];
    uint8_t		Sq[64];

    /* 0000 0000 (Q+K)check * (B+W) is possible
    * 1000 0000 BQ check is not possible
    * 0100 0000 BK check is not possible
    * 0010 0000 Already castle 
    */
    uint8_t	Castling_check;
    uint8_t	No_Ply; //number of Ply
    uint8_t	Side_to_move;
    Move	PreviousMove;
    
    std::string	toFENString();  //convert to FENString
    void printBoard();
    BOARD_C makeMove (ExtMove to);
    BOARD_C makeMove (Move to);
    BOARD_C undoMove (ExtMove undo);
    void printAllMove();
};

#endif
