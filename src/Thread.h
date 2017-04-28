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


void	InitBoard()	;

void 	Init_engine();

void 	Display_Move(BOARD A);

void 	*StartGame(void * threadArg);

void 	*Timer(void *);

#endif 

