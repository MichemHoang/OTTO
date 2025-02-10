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

class Game{
private:
	BOARD	INIT;
	time_t 	now;
    int 	Sig;
	bool	UNLOCK;
	Search	GAME;
    Book    book;
public:
    void	AIMove(Search *A, int *TotalTime, int level, std::pair<Move, int> *ANS);
	void	InitBoard()	;
	void 	Init_engine();
	void 	Display_Move(BOARD A, int MType);
	void 	StartGame(void *threadArg);
	void 	Timer();
	void	SignalHandler();
};

#endif 

