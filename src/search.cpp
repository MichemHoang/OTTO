#include "search.h"

Search::Search	(){
	TRANS_TABLE.setSize(1000000);
	FEN_Op::READ_FEN(STANDARD, &Position);
	SearchNode	=	0;
}

void	Search::TimeOut (bool FIN){
	OutOfTime = FIN;
}

void	Search::setPosition(BOARD Pos){
	Position	=	Pos;
}

void 	Search::setTableSize	(int   Size){
	TRANS_TABLE.setSize(Size);
}

uint64_t 	Search::getSearchNode	(){
	return SearchNode;
};

int 	Search::getDatabaseSize	(){
	return TRANS_TABLE.getSize();
};

int		Search::getTime(){
	return MoveTime;
}

int		Search::QuiesceneSearch	(BOARD A, int Alpha, int Beta){
	SearchNode++;
	ExtMove CaptureList[20];
	ExtMove *iter	=	CaptureList;
	int evaluation	=	EVALUATION::Evaluate(A, A.Side_to_move);
	if (evaluation 	>=	Beta) 	{	return Beta;	}
	if (evaluation  >=	Alpha) 	{	Alpha 	=	evaluation;	}
	int CAPTURE_LIST_SIZE	=	GENERATE::CaptureMove(A, iter, A.Side_to_move); 
	for (int i = 0; i < CAPTURE_LIST_SIZE; i++){
		int iMin = i;
		for (int j = i+1; j < CAPTURE_LIST_SIZE; j++)	if (CaptureList[j].value > CaptureList[iMin].value)	iMin	=	j;
		if ( i != iMin )	{
			ExtMove	tmp			=	CaptureList[i];
			CaptureList[i]		=	CaptureList[iMin];
			CaptureList[iMin]	=	tmp;
		}
		evaluation	=	-QuiesceneSearch(MOVE::MakeMove(A, CaptureList[i]), -Beta, -Alpha);
		if (evaluation 	>=	Beta) 	{	return Beta;	}
		if (evaluation  >=	Alpha) 	{	Alpha 	=	evaluation;	}
    }
    return Alpha;
}

pair<Move, int>	Search::AlphaBeta (BOARD A, int DEPTH, int Alpha, int Beta, int FINAL_DEPT, Key ZobristHash, bool LMR){
	bool		BreakSign = false;
	pair<Move, int> result;
	int			Alpha0	=	Alpha;
	ExtMove 	MoveList[MAX_MOVES];
	int			ListSize, v, BestValue, LateMove;
	ExtMove		HashMove;
	bool		HashMoveFound	=	false;
	HashMove.move	=	0;
	
	//-------------------Check if value is in Hash Table-----------------
	HashEntry Memo;
	if (TRANS_TABLE.FindEntry(ZobristHash, &Memo)){
		HashMove.move	=	Memo.BestMove;
		HashMoveFound	=	true;
		if ( Memo.Depth	>=	(DEPTH)){
			switch (Memo.Node_Type){
				case EXACT		:	
					if (DEPTH	!=	FINAL_DEPT)	return 	make_pair(A.PreviousMove, Memo.Evaluation);
					else 						return	make_pair(Memo.BestMove, Memo.Evaluation);
				case UPPERBOUND	:	Alpha	=	max(Alpha, Memo.Evaluation);	break;
				case LOWERBOUND	:	Beta	=	min(Beta, Memo.Evaluation);		break;
			}
			
			if (Alpha >= Beta) return make_pair(A.PreviousMove, Memo.Evaluation);
		}
	}
	SearchNode++;
	
	//----------------------------NEGAMAX--------------------------
	
	//Reaching Leaf node
	if (DEPTH	==	0 || A.Pieces[wK + 6* (A.Side_to_move)] == 0) {
		if ( A.Pieces[wK + 6* (A.Side_to_move)] == 0 )	{
			return  make_pair(A.PreviousMove, -99999);
		}
		int evaluation	=	QuiesceneSearch(A, Alpha, Beta);
		return make_pair(A.PreviousMove, evaluation);
	}

	v	=	MIN_VALUE;
	
	//Best move from hash table
	if (HashMove.move != 0)	{
		BestValue	=	v;
		int	Temp	=	-AlphaBeta(MOVE::MakeMove(A, HashMove), DEPTH - 1, -Beta, -Alpha, 
								   FINAL_DEPT, UpdateKey(ZobristHash, HashMove.move, A),false).second;
		v			=	max(v, Temp);
		if (OutOfTime == 1)	return result;
		if ( BestValue != v ) 	result	=	make_pair(HashMove.move, v);
		if ( Alpha < v) 		Alpha	=	v;
		if ( Alpha >= Beta )  {	
			if (HashMove.getFlags() != 4 )	KillerMove[DEPTH][0] = HashMove.move;
			BreakSign = true;	
		}
	}
	
	LateMove	=	0;
	//cout	<< "Generating Move\n";
	if (!BreakSign){
		ListSize	=	GENERATE::AllMove(A, MoveList, A.Side_to_move);	
		for (int i = 0; i < ListSize; i++){
			if (MoveList[i].move == KillerMove[DEPTH][0]){
				MoveList[i].value = 100;
				break;
			}	
		}	
			
			
		for (int iterYo = 0; iterYo < ListSize; iterYo++){
			int iMin = iterYo;
			for (int j = iterYo+1; j < ListSize; j++)	if (MoveList[j].value > MoveList[iMin].value)	iMin	=	j;
			if ( iterYo != iMin )	{
				ExtMove	tmp			=	MoveList[iterYo];
				MoveList[iterYo]	=	MoveList[iMin];
				MoveList[iMin]		=	tmp;
			};
			
			//Large unsorted tree
			//Late Move Reduction;
			if (ShallowDone && iterYo > ListSize/4 && !LMR){
				if (DEPTH > 2 && DEPTH < FINAL_DEPT - 1 && MoveList[iterYo].value <= 50 && GameState != STATE::END) 
					LateMove	=	2;
					LMR	=	true;
			}
			
			if (MoveList[iterYo].move != HashMove.move){
				BestValue	=	v;
				int	Temp	=	-AlphaBeta(MOVE::MakeMove(A, MoveList[iterYo]), DEPTH - 1 - LateMove, -Beta, -Alpha, 
										   FINAL_DEPT, UpdateKey(ZobristHash, MoveList[iterYo].move, A), LMR).second;
				v			=	max(v, Temp);
				if (OutOfTime == 1)	return result;
				if ( BestValue != v ) 	result	=	make_pair(MoveList[iterYo].move, v);
				if ( Alpha < v) 		Alpha	=	v;
				if ( Alpha >= Beta )  {	
					if (MoveList[iterYo].getFlags() != 4 )	KillerMove[DEPTH][0] = MoveList[iterYo].move;
					break;
				}
			}
		}
	}
	
    //---------------------STORE_NODES_IN_DATABASE---------------------
	uint8_t flag;
	if 		(BestValue		<=	Alpha0)	flag	=	UPPERBOUND;
	else if (BestValue		>=	Beta)	flag	=	LOWERBOUND;
	else   								flag	=	EXACT;
	HashEntry AA;
	AA.BestMove		=	result.first;
	AA.Depth		=	DEPTH;
	AA.Flag			=	false;
	AA.Node_Type	=	flag;
	AA.Evaluation	=	v;
	AA.HashValue	=	ZobristHash;
	TRANS_TABLE.addEntry(ZobristHash, AA);
	//cout	<< "RETURNING\n";
    return result;
}

pair<Move, int>	Search::SearchPosition (int MAX_DEPTH){
	time_t 		now;
	int 		Timer	=	time(&now);
	int			Material_Point	=	0;
	BitString 	BoardHashValue	=	GetKey(Position);
	ExtMove 	FirstBatch[MAX_MOVES];
	int			Alpha	=	MIN_VALUE;
	int 		Beta	=	MAX_VALUE;
	pair<Move, int> STORE_MOVE;
	pair<Move, int> OPTIMAL_MOVE;
	
	OutOfTime	=	false;
	GameState	=	STATE::BEGIN;
	
	//Checking gamestate based on material score
	for (int i = 0; i < 5; i++) 
		Material_Point += (BitOp::PopsCount(Position.Pieces[i] | Position.Pieces[i + 6])) * VALUE[i];
	if (Material_Point <= 7600) {
		cout	<< "Middle Game\n";
		GameState	=	STATE::MID;
		MAX_DEPTH += 0;
	}
	if (Material_Point <= 4000) {
		cout	<< "EndGame Game\n";
		GameState	=	STATE::END;
	}
	//Reduce Depth based on killer capture	
	ExtMove *Batch	=	FirstBatch;
	int BSIZE	=	GENERATE::AllMove(Position, Batch, Position.Side_to_move);
	for (int iterYo = 0; iterYo < BSIZE; iterYo++){
		int iMin = iterYo;
		for (int j = iterYo+1; j < BSIZE; j++)	if (Batch[j].value > Batch[iMin].value)	iMin	=	j;
		if ( iterYo != iMin )	{
			ExtMove	tmp			=	Batch[iterYo];
			Batch[iterYo]	=	Batch[iMin];
			Batch[iMin]		=	tmp;
		};
	}
	if (FirstBatch[0].getFlags() == 4 && Position.No_Ply > 10)
		if ((FirstBatch[0].value - FirstBatch[1].value ) >= 250) {
			cout	<< "Capure reduction\n";
			MAX_DEPTH -= 0;
		}
	
	//Iterative deepening search loop
	ShallowDone	=	false;
	for (int inc =	4; inc >= 0; inc-=2){
		SearchNode	=	0;
		OPTIMAL_MOVE	=	AlphaBeta(Position, MAX_DEPTH - inc, Alpha, Beta, MAX_DEPTH - inc, BoardHashValue, false);
		TRANS_TABLE.UpdateTable();
		if (inc == 2)	{
			DECODE::DecodeMove(OPTIMAL_MOVE.first);
			cout	 << "Move score = " << OPTIMAL_MOVE.second << endl;
			int TimeLimit	=	time(&now) - Timer;
			if (TimeLimit > 400) {
				cout	<< "Exceed Time Limit\n"; 
				MoveTime	=	TimeLimit;
				break;
			}
			ShallowDone	=	true;
		}
		if (OutOfTime)	OPTIMAL_MOVE =	STORE_MOVE;
		else STORE_MOVE = OPTIMAL_MOVE;
		Alpha		=	STORE_MOVE.second - 100;
		Beta		=	STORE_MOVE.second + 100;
	}
	TRANS_TABLE.UpdateTable();
	MoveTime	=	time(&now) - Timer;
	cout	 << "Move score = " << OPTIMAL_MOVE.second << endl;
	return OPTIMAL_MOVE;
}

int		DynamicEval	(BOARD A, int Alpha, int Beta){
	ExtMove CaptureList[20];
	ExtMove *iter	=	CaptureList;
	int evaluation	=	EVALUATION::Evaluate(A, A.Side_to_move);
	if (evaluation 	>=	Beta) 	{	return Beta;	}
	if (evaluation  >=	Alpha) 	{	Alpha 	=	evaluation;	}
	int CAPTURE_LIST_SIZE	=	GENERATE::CaptureMove(A, iter, A.Side_to_move); 
	for (int i = 0; i < CAPTURE_LIST_SIZE; i++){
		int iMin = i;
		for (int j = i+1; j < CAPTURE_LIST_SIZE; j++)	if (CaptureList[j].value > CaptureList[iMin].value)	iMin	=	j;
		if ( i != iMin )	{
			ExtMove	tmp			=	CaptureList[i];
			CaptureList[i]		=	CaptureList[iMin];
			CaptureList[iMin]	=	tmp;
		}
		evaluation	=	-DynamicEval(MOVE::MakeMove(A, CaptureList[i]), -Beta, -Alpha);
		if (evaluation 	>=	Beta) 	{	return Beta;	}
		if (evaluation  >=	Alpha) 	{	Alpha 	=	evaluation;	}
    }
    return Alpha;
}


int max (int A, int B){	if ( A > B ) return A; return B;	}

int min (int A, int B){	if ( A <= B ) return A; return B;	}
