#ifndef THREAD_H_
#define THREAD_H_

#include <stdio.h>
#include <iostream>
#include <pthread.h>
#include <unistd.h>

#include "BitBoard.h"
#include "MoveGen.h"
#include "evaluation.h"
#include "search.h" 
#include "OpenBook.h"

/* Managing thread
 * Timer, main engine, IO
 */

class THREAD{
	private:
		BOARD	INIT;
		time_t 	now;
		int 	SIGNAL;
		bool	UNLOCK;
		Search	GAME;
	public:
		void	AIMove(Search *A, int *TotalTime, int level, pair<Move, int> *ANS);
		void	InitBoard()	;
		void 	Init_engine();
		void 	Display_Move(BOARD A, int MType);
		void 	StartGame(void *threadArg);
		void 	Timer();
		void	SignalHandler();
};

#endif 

