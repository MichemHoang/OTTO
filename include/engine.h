#ifndef GAME_H_
#define GAME_H_

#include <stdio.h>
#include <iostream>
#include <thread>

#include "evaluation.h"
#include "search.h" 
#include "openbook.h"

/* Managing thread
 * Timer, main engine, IO
 */

class Engine{
private:
	BOARD_C	initialBoard;
	Search  alphaBeta;
	int		depth, MaxMove;
	std::vector<ExtMove> gameMove;
public:
	Engine();
	~Engine();
	void	Timer		(int milliseconds);
	void	SetPosition	(std::string fenString);
	void 	InitEngine	();
	void 	Start		(void *GameArg);
};

#endif 

