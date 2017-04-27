#ifndef THREAD_H_
#define THREAD_H_

#include <stdio.h>
#include <iostream>
#include "BitBoard.h"
#include "MoveGen.h"
#include "evaluation.h"
#include "search.h" 
#include <pthread.h>
#include <unistd.h>
#include "OpenBook.h"

struct HalfMoveInfo{
	pair <Move, int> Result;
	int	Time;
	int	SearchNode;
};

void	InitBoard()	;

void 	Init_engine();

void 	Display_Move(BOARD A);

HalfMoveInfo AI_Move ( BOARD A, int level, int *TotMove, int *TotTime, Move DrawCondition );

void 	*Start_Game(void * threadArg);

void *Clock(void *);

#endif 

