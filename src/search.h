#ifndef SEARCH_H_
#define SEARCH_H_

#include "evaluation.h"
#include "MoveGen.h"
#include "ZobristHash.h"
#include <utility>      // std::pair, std::make_pair
#include <stdio.h>
#include "Transposition.h"

extern Key		HOLY, SHIT;
extern uint64_t	SearchNode;
extern int		Match;
extern double	Total;
extern double 	AverageFail;
using namespace std;

pair<Move, int>	SearchS (BOARD A, int DEPTH, int Alpha, int Beta, int Timer, int FINAL_DEPT);

Move SEARCH_DEPTH(int DEPTH, BOARD A);

int	QuiesceneSearch(BOARD A, int Alpha, int Beta, uint64_t *SN);

int	QuiesceneSearch(BOARD A, int Alpha, int Beta);

pair<Move, int>	DEEPENING_SEARCH (BOARD A, int MAX_DEPTH, int choo, Move Draw);

pair<Move, int> IterDeepening_SEARCH(BOARD A, int MAX_DEPTH, int Choo);

pair<Move, int>	SEARCH_WITH_TABLE (BOARD A, int DEPTH, int Alpha, int Beta, int Timer, int FINAL_DEPT, Key ZobristKey, uint64_t *SN);

pair<Move, int>	SEARCH_WITH_TABLE (BOARD A, int DEPTH, int Alpha, int Beta, int Timer, int FINAL_DEPT, Key ZobristKey);

#endif 
