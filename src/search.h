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

class Search{
	private:
		Move		KillerMove[15][3];
		bool		OutOfTime;
		uint64_t	SearchNode;
		BOARD		Position;
		HASH_TABLE<HashEntry, Key> TRANS_TABLE;
		int			MoveTime;
		bool		ShallowDone;
		int 		FINAL_DEPT;
	public:
		Search	();
		void		getBranchFactor();
		void		TimeOut (bool FIN);
		int			getTime();
		void 		setPosition	(BOARD Pos);
		void 		setTableSize	(int   Size);
		uint64_t	getSearchNode	();
		int 		getDatabaseSize	();
		int			QuiesceneSearch	(BOARD A, int Alpha, int Beta);
		pair<Move, int>	AlphaBeta (BOARD A, int DEPTH, int Alpha, int Beta, int FINAL_DEPT, Key ZobristKey);
		pair<Move, int>	SearchPosition (int MAX_DEPTH);
};

#endif 
