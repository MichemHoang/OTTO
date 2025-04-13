#ifndef SEARCH_H_
#define SEARCH_H_

#include "transposition.h"
#include "evaluation.h"
#include "movegen.h"
#include "zobristhash.h"

#include <utility>      // std::pair, std::make_pair
#include <stdio.h>
#include <time.h>

class Search{
private:
	Move		killerMove[15][3]; 
	bool		outOfTime;
	uint64_t	searchNode;
	BOARD		position;
	BOARD_C		boardPosition;
	HASH_TABLE<HashEntry, Key> TRANS_TABLE;
	int			moveTime;
	bool		shallowSearch;
    int 		FINAL_DEPT;
public:
	Search();
	void		GetBranchFactor();
	void		TimeOut (bool FIN);
	void 		SetPosition	(BOARD Pos);	
	void 		SetPosition	(BOARD_C Pos);
	void 		SetTableSize	(int   Size);
	uint64_t	GetSearchNode	();
	int 		GetDatabaseSize	();
	int			QuiesceneSearch	(BOARD A, int Alpha, int Beta);
	int			QuiesceneSearch	(BOARD_C A, int Alpha, int Beta);
    std::pair<Move, int> AlphaBeta (BOARD A, int DEPTH, int Alpha, int Beta, int FINAL_DEPT, Key ZobristKey, bool LMR);
	std::pair<Move, int> IterativeDeepening (int MAX_DEPTH);
	ExtMove	AlphaBeta (BOARD_C A, int DEPTH, int Alpha, int Beta, int FINAL_DEPT, Key ZobristKey, bool LMR);
    ExtMove IterativeDeepening(int maxDept, bool iterative);
};

#endif 
