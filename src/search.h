#ifndef SEARCH_H_
#define SEARCH_H_

#include <utility>      // std::pair, std::make_pair
#include <stdio.h>
#include <time.h>

#include "Transposition.h"
#include "evaluation.h"
#include "MoveGen.h"
#include "ZobristHash.h"

extern Key		HOLY, SHIT;
extern uint64_t	SearchNode;
extern int		Match;
extern double	Total;
extern double 	AverageFail;

using namespace std;


int	QuiesceneSearch(BOARD A, int Alpha, int Beta);

pair<Move, int>	DEEPENING_SEARCH (BOARD A, int MAX_DEPTH, int choo, Move Draw);

pair<Move, int> IterDeepening_SEARCH(BOARD A, int MAX_DEPTH, int Choo);

pair<Move, int>	SEARCH_WITH_TABLE (BOARD A, int DEPTH, int Alpha, int Beta, int Timer, int FINAL_DEPT, Key ZobristKey);

#endif 
