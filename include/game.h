#ifndef GAME_H_
#define GAME_H_

#include <stdio.h>
#include <iostream>
#include <pthread.h>
#include <unistd.h>

#include "evaluation.h"
#include "search.h" 
#include "openbook.h"

/* Managing thread
 * Timer, main engine, IO
 */


enum gameMode {AIvsAI, HumanVsAi};

class Game{
private:
	BOARD	initialPosition;
	BOARD_C	initialBoard;
	time_t 	now;
    int 	stopSig;
	bool	unlock;
	Search	game;
	Search  alphaBeta;
    Book    openingBook;
	std::vector<ExtMove> gameMove;
public:
    void	AIMove(Search *A, int *TotalTime, int level, std::pair<Move, int> *ANS);
	ExtMove	AIMove(Search *A, int *TotalTime, int level);
	void	InitBoard();
	void	CustomPosition(BOARD_C board);
	void	CustomPositionFromFenString(std::string fenString);
	void 	Init_engine();
	void	DisplayMove(BOARD_C board, int MType);
	void 	Display_Move(BOARD A, int MType);
	void 	Start(void *threadArg);
	void 	Timer();
	void	SignalHandler();
};

#endif 

