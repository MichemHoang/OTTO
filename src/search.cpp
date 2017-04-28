#include "search.h"

HASH_TABLE<HashEntry, Key> TRANS_TABLE(4000000);
int History_heristic[64][64];
int			Out_Of_Time;
uint64_t	SearchNode;
pair<Move, int>		BestMove[2];

pair<Move, int>	DEEPENING_SEARCH (BOARD A, int MAX_DEPTH, Move Draw){
	int			Material_Point	=	0;
	time_t 		now;
	int 		Timer	=	time(&now);
	int			FINAL_DEPT;
	BitString 	BoardHashValue	=	GetKey(A);
	ExtMove 	FirstBatch[100];
	int			Alpha	=	MIN_VALUE;
	int 		Beta	=	MAX_VALUE;
	pair<Move, int> STORE_MOVE;
	pair<Move, int> OPTIMAL_MOVE;
	Out_Of_Time	=	0;
	
	//Checking if Board has reach Middle game. If so, increase depth;
	for (int i = 0; i < 5; i++) 
		Material_Point += (PopsCount(A.Pieces[i] | A.Pieces[i + 6])) * VALUE[i];
	if (Material_Point <= 7700) 
		MAX_DEPTH += 2;
		
	//Reduce Depth based on killer capture	
	ExtMove *Batch	=	FirstBatch;
	int BSIZE	=	GENERATE::AllMove(A, Batch, A.Side_to_move);
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
		if ((FirstBatch[0].value - FirstBatch[1].value ) >= 250) 
			MAX_DEPTH -= 2;
	Out_Of_Time	=	false;
	
	//Deepening search loop
	for (int inc =	2; inc >= 0; inc-=2){
		SearchNode	=	0;
		BestMove[0]		=	make_pair(0, -99999);
		BestMove[1]		=	make_pair(0, -99999);
		FINAL_DEPT	=	MAX_DEPTH - inc;
		OPTIMAL_MOVE	=	SEARCH_WITH_TABLE(A, FINAL_DEPT, Alpha, Beta, Timer, FINAL_DEPT, BoardHashValue);
		TRANS_TABLE.UpdateTable();
		if (inc == 2)	{
			cout << "Shallow search\n"; 
			DECODE::DecodeMove(OPTIMAL_MOVE.first);
			int TimeLimit	=	time(&now) - Timer;
			if (TimeLimit > 60) {cout	<< "Exceed Time Limit, break\n"; break;}
		}
		if (Out_Of_Time) 	{
			OPTIMAL_MOVE	=	STORE_MOVE;
			cout	<< "OUT OF TIME\n";
		}
		else 			STORE_MOVE	=	OPTIMAL_MOVE;
		Alpha		=	STORE_MOVE.second - 150;
		Beta		=	STORE_MOVE.second + 150;
	}
	cout	<< 	"Total Node	= "	<< SearchNode	<< endl;
	TRANS_TABLE.UpdateTable();
	return OPTIMAL_MOVE;
}

pair<Move, int>	SEARCH_WITH_TABLE (	BOARD A, int DEPTH, int Alpha, int Beta, int Timer, 
									int FINAL_DEPT, BitString ZobristHash){
	time_t now;
	int	Alpha0		=		Alpha;
	pair<Move, int> result;
	//----------------Check if Search run out of time--------------
	int Current_Timer	=	time(&now);
	if (Current_Timer	-	Timer	>= 800){
		Out_Of_Time	=	1;
		return 	result;
	}
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
	if (TRANS_TABLE.FindEntry(ZobristHash, &Memo))	{
		for (int i = 0; i < FSIZE; i++) {
			if (MoveList[i].move == Memo.BestMove) {
				MoveList[i].value	+=	5000;
				break;
			}
		}
	}
	for (int iterYo = 0; iterYo < FSIZE; iterYo++){
		int iMin = iterYo;
		for (int j = iterYo+1; j < FSIZE; j++)	if (MoveList[j].value > MoveList[iMin].value)	iMin	=	j;
		if ( iterYo != iMin )	{
			ExtMove	tmp			=	MoveList[iterYo];
			MoveList[iterYo]	=	MoveList[iMin];
			MoveList[iMin]		=	tmp;
		};
		BestValue	=	v;
		int	Temp	=	-SEARCH_WITH_TABLE(MOVE::MakeMove(A, MoveList[iterYo]), DEPTH - 1, -Beta, -Alpha, 
						Timer, FINAL_DEPT, UpdateKey(ZobristHash, MoveList[iterYo].move, A)).second;
		v			=	max(v, Temp);
		if (Out_Of_Time == 1)	return result;
		if (DEPTH	==	FINAL_DEPT){
			pair<Move, int> asas	=	make_pair(MoveList[iterYo].move, Temp);
			if (asas.second > BestMove[0].second){
				BestMove[1]	=	BestMove[0];
				BestMove[0]	=	asas;
			} else if (asas.second > BestMove[1].second) BestMove[1]	=	asas;
		}
		if ( BestValue != v ) 	result	=	make_pair(MoveList[iterYo].move, v);
		if ( Alpha < v) 		Alpha	=	v;
		if ( Alpha >= Beta )  {	break; }
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

//QUIETSCENE SEARCH: Continute searching until no capture move availabe)
int	QuiesceneSearch(BOARD A, int Alpha, int Beta){
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

int max (int A, int B){	if ( A > B ) return A; return B;	}

int min (int A, int B){	if ( A <= B ) return A; return B;	}
