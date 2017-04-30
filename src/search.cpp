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

pair<Move, int>	Search::AlphaBeta (BOARD A, int DEPTH, int Alpha, int Beta, int FINAL_DEPT, Key ZobristHash){
	pair<Move, int> result;
	int	Alpha0		=		Alpha;
	//-------------------Check if value is in Hash Table-----------------
	HashEntry Memo;
	if (TRANS_TABLE.FindEntry(ZobristHash, &Memo)){
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
	int v, BestValue;
	
	//Reaching Leaf node
	if (DEPTH	==	0 || A.Pieces[wK + 6* (A.Side_to_move)] == 0) {
		if ( A.Pieces[wK + 6* (A.Side_to_move)] == 0 )	{
			return  make_pair(A.PreviousMove, -9999);
		}
		int evaluation	=	QuiesceneSearch(A, -Beta - 150, -Alpha + 150);
		return make_pair(A.PreviousMove, evaluation);
	}
	ExtMove 	MoveList[100];
	ExtMove 	*lo		=	MoveList;
	int 		FSIZE	=	GENERATE::AllMove(A, lo, A.Side_to_move);
	v	=	MIN_VALUE;
	
	//Best move from hash table
	if (TRANS_TABLE.FindEntry(ZobristHash, &Memo))	{
		for (int i = 0; i < FSIZE; i++) {
			if (MoveList[i].move == Memo.BestMove) {
				MoveList[i].value	+=	5000;
				break;
			}
		}
	}
	
	//Sibling KillerMove
	
	for (int iter = 0; iter < FSIZE ; iter++){
		if (MoveList[iter].move == KillerMove[DEPTH][0]){
			MoveList[iter].value += 100;
			break;
		}
	}//*/
	
	for (int iterYo = 0; iterYo < FSIZE; iterYo++){
		int iMin = iterYo;
		for (int j = iterYo+1; j < FSIZE; j++)	if (MoveList[j].value > MoveList[iMin].value)	iMin	=	j;
		if ( iterYo != iMin )	{
			ExtMove	tmp			=	MoveList[iterYo];
			MoveList[iterYo]	=	MoveList[iMin];
			MoveList[iMin]		=	tmp;
		};
		BestValue	=	v;
		int	Temp	=	-AlphaBeta(MOVE::MakeMove(A, MoveList[iterYo]), DEPTH - 1, -Beta, -Alpha, 
								   FINAL_DEPT, UpdateKey(ZobristHash, MoveList[iterYo].move, A)).second;
		v			=	max(v, Temp);
		if (OutOfTime == 1)	return result;
		if ( BestValue != v ) 	result	=	make_pair(MoveList[iterYo].move, v);
		if ( Alpha < v) 		Alpha	=	v;
		if ( Alpha >= Beta )  {	
			if (MoveList[iterYo].getFlags() != 4 )	KillerMove[DEPTH][0] = MoveList[iterYo].move;
			break;
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
    return result;
}


pair<Move, int>	Search::SearchPosition (int MAX_DEPTH){
	time_t 		now;
	int 		Timer	=	time(&now);
	int			Material_Point	=	0;
	BitString 	BoardHashValue	=	GetKey(Position);
	ExtMove 	FirstBatch[100];
	int			Alpha	=	MIN_VALUE;
	int 		Beta	=	MAX_VALUE;
	pair<Move, int> STORE_MOVE;
	pair<Move, int> OPTIMAL_MOVE;
	OutOfTime	=	false;
	//Checking gamestate based on material score
	for (int i = 0; i < 5; i++) 
		Material_Point += (BitOp::PopsCount(Position.Pieces[i] | Position.Pieces[i + 6])) * VALUE[i];
	if (Material_Point <= 7600) {
		cout	<< "Middle Game\n";
		MAX_DEPTH += 2;
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
	if (FirstBatch[0].getFlags() == 4)
		if ((FirstBatch[0].value - FirstBatch[1].value ) >= 250) {
			cout	<< "Capure reduction\n";
			MAX_DEPTH -= 2;
		}
	
	//Iterative deepening search loop
	for (int inc =	2; inc >= 0; inc-=2){
		SearchNode	=	0;
		OPTIMAL_MOVE	=	AlphaBeta(Position, MAX_DEPTH - inc, Alpha, Beta, MAX_DEPTH - inc, BoardHashValue);
		TRANS_TABLE.UpdateTable();
		if (inc == 2)	{
			DECODE::DecodeMove(OPTIMAL_MOVE.first);
			int TimeLimit	=	time(&now) - Timer;
			if (TimeLimit > 60) {
				cout	<< "Exceed Time Limit\n"; 
				MoveTime	=	TimeLimit;
				break;
			}
		}
		if (OutOfTime)	OPTIMAL_MOVE =	STORE_MOVE;
		else STORE_MOVE = OPTIMAL_MOVE;
		Alpha		=	STORE_MOVE.second - 150;
		Beta		=	STORE_MOVE.second + 150;
	}
	TRANS_TABLE.UpdateTable();
	MoveTime	=	time(&now) - Timer;
	return OPTIMAL_MOVE;
}


int max (int A, int B){	if ( A > B ) return A; return B;	}

int min (int A, int B){	if ( A <= B ) return A; return B;	}
