#include "search.h"


int max (int A, int B){	if ( A > B ) return A; return B;	}

int min (int A, int B){	if ( A <= B ) return A; return B;	}


Search::Search	(){
	TRANS_TABLE.setSize(1000000);
	FEN_Op::READ_FEN(STANDARD, &Position);
	SearchNode	=	0;
}

void Search::TimeOut (bool FIN){
	OutOfTime = FIN;
}

void Search::setPosition(BOARD Pos){
	Position	=	Pos;
}

void Search::setTableSize	(int   Size){
	TRANS_TABLE.setSize(Size);
}

uint64_t Search::getSearchNode	(){
	return SearchNode;
}

int Search::getDatabaseSize	(){
	return TRANS_TABLE.getSize();
}

int	Search::getTime(){
	return MoveTime;
}

int	Search::QuiesceneSearch	(BOARD A, int Alpha, int Beta){
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

std::pair<Move, int> Search::AlphaBeta (BOARD A, int DEPTH, int Alpha, int Beta, int FINAL_DEPT, Key ZobristHash, bool LMR){
	bool		BreakSign = false;
    std::pair<Move, int> result;
	int			Alpha0	=	Alpha;
	ExtMove 	MoveList[MAX_MOVES];
	int			ListSize, v, BestValue, LateMove;
	ExtMove		HashMove;
	bool		HashMoveFound	=	false;
	int 		MaxMoveValue	=	0;
	bool		CheckLMR		=	false;
	HashMove.move	=	0;
	//-------------------Check if value is in Hash Table-----------------
	HashEntry Memo;
	if (TRANS_TABLE.FindEntry(ZobristHash, &Memo)){
		HashMove.move	=	Memo.BestMove;
		HashMoveFound	=	true;
		if ( Memo.Depth	>=	(DEPTH)){
			switch (Memo.Node_Type){
				case EXACT		:	
                    if (DEPTH	!=	FINAL_DEPT)	return 	std::make_pair(A.PreviousMove, Memo.Evaluation);
                    else 						return	std::make_pair(Memo.BestMove, Memo.Evaluation);
                case UPPERBOUND	:	Alpha	=	max(Alpha, Memo.Evaluation);	break;
				case LOWERBOUND	:	Beta	=	min(Beta, Memo.Evaluation);		break;
			}
			
			if (Alpha >= Beta) {
                // std::cout	<< "RETURNING FROM TABLE\n";
                return std::make_pair(A.PreviousMove, Memo.Evaluation);
			}
		}
	}
	SearchNode++;
	
	//----------------------------NEGAMAX--------------------------
	
	//Reaching Leaf node
	if (DEPTH	==	0 || A.Pieces[wK + 6* (A.Side_to_move)] == 0) {
		if ( A.Pieces[wK + 6* (A.Side_to_move)] == 0 )	
            return   std::make_pair(A.PreviousMove, -19999);
		int evaluation	=	QuiesceneSearch(A, Alpha, Beta);
        return  std::make_pair(A.PreviousMove, evaluation);
	}

	v	=	MIN_VALUE;
	
	//Best move from hash table
	if (HashMove.move != 0)	{
		BestValue	=	v;
		int	Temp	=	-AlphaBeta(MOVE::MakeMove(A, HashMove), DEPTH - 1, -Beta, -Alpha, 
								   FINAL_DEPT, UpdateKey(ZobristHash, HashMove.move, A),false).second;
		v			=	max(v, Temp);
		if (OutOfTime == 1)	return result;
        if ( BestValue != v ) 	result	=	 std::make_pair(HashMove.move, v);
		if ( Alpha < v) 		Alpha	=	v;
		if ( Alpha >= Beta )  {	
			if (HashMove.getFlags() != 4 )	KillerMove[DEPTH][0] = HashMove.move;
			BreakSign = true;	
		}
	}
	
	LateMove	=	0;
    // std::cout	<< "Generating Move\n";
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
				if (iterYo	==	0)	{MaxMoveValue	=	MoveList[iterYo].value;}
			};
			
			
			//Late Move Reduction;
			if (!CheckLMR){
				if (ShallowDone && (iterYo > 4 || iterYo > ListSize/4) && !LMR){
					if (DEPTH > 2 && DEPTH <= FINAL_DEPT - 1) {
						if (HashMoveFound || MaxMoveValue > 200){
                            // std::cout	<< "LMR\n";
							LMR	=	true;
							CheckLMR	=	true;
							LateMove	=	2;
						} 
					}
				}
			}
			if (MoveList[iterYo].move != HashMove.move){
				BestValue	=	v;
				int	Temp	=	-AlphaBeta(MOVE::MakeMove(A, MoveList[iterYo]), DEPTH - 1 - LateMove, -Beta, -Alpha, 
											FINAL_DEPT, UpdateKey(ZobristHash, MoveList[iterYo].move, A), LMR).second;
				v			=	max(v, Temp);
				if ( BestValue != v &&  LMR && LateMove != 2){
					v 		=	BestValue;
					Temp	=	-AlphaBeta(MOVE::MakeMove(A, MoveList[iterYo]), DEPTH - 1, -Beta, -Alpha, 
										   FINAL_DEPT, UpdateKey(ZobristHash, MoveList[iterYo].move, A), false).second;
					v			=	max(v, Temp);
				}
				if (OutOfTime == 1)	return result;
                if ( BestValue != v ) 	result	=	 std::make_pair(MoveList[iterYo].move, v);
				if ( Alpha < v) Alpha	=	v;
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
	if (DEPTH > 1 && false) {
         std::cout	<< "RETURNING FROM DEPTH " << DEPTH <<  std::endl;
         std::cout	<< "Best Value	=	" << result.second <<  std::endl;
         std::cout	<< "BestMove	=	"; DECODE::DecodeMove(result.first);
	}
    return result;
}

std::pair<Move, int> Search::SearchPosition (int MAX_DEPTH){
	time_t 		now;
	int 		Timer	=	time(&now);
	int			Material_Point	=	0;
	BitString 	BoardHashValue	=	GetKey(Position);
	ExtMove 	FirstBatch[MAX_MOVES];
	int			Alpha	=	MIN_VALUE;
	int 		Beta	=	MAX_VALUE;
     std::pair<Move, int> STORE_MOVE;
     std::pair<Move, int> OPTIMAL_MOVE;
	
    OutOfTime	=	false;
	//Checking gamestate based on material score
	//Max Material (No King) = 8140 (both side);
	for (int i = 0; i < 5; i++) 
		Material_Point += (BitOp::PopsCount(Position.Pieces[i] | Position.Pieces[i + 6])) * VALUE[i];
	if (Material_Point <= 6000) {
         std::cout	<< "Increase search depth\n";\
		MAX_DEPTH += 2;
	}
	if (Material_Point <= 4000) {
         std::cout	<< "EndGame Game\n";\
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
	if (FirstBatch[0].getFlags() == 4 && Material_Point < 6500)
		if ((FirstBatch[0].value - FirstBatch[1].value ) >= 350) {
             std::cout	<< "Capture reduction\n";
			MAX_DEPTH -= 2;
		}
		
	//Iterative deepening search loop
	ShallowDone	=	false;
	for (int inc =	4; inc >= 0; inc-=2){
		SearchNode	=	0;
		OPTIMAL_MOVE	=	AlphaBeta(Position, MAX_DEPTH - inc, Alpha, Beta, MAX_DEPTH - inc, BoardHashValue, false);
		TRANS_TABLE.UpdateTable();
         std::cout	 << "Move score = " << OPTIMAL_MOVE.second <<  std::endl;
		if (inc == 2)	{
             std::cout	<< "Search node = " << getSearchNode() <<  std::endl;
			DECODE::DecodeMove(OPTIMAL_MOVE.first);
			int TimeLimit	=	time(&now) - Timer;
			if (TimeLimit > 400) {
                 std::cout	<< "Exceed Time Limit\n";
				MoveTime	=	TimeLimit;
				break;
			}
		}
		ShallowDone	=	true;
		if (OutOfTime)	OPTIMAL_MOVE =	STORE_MOVE;
		else STORE_MOVE = OPTIMAL_MOVE;
		//Alpha		=	STORE_MOVE.second - 250;
		//Beta		=	STORE_MOVE.second + 250;
	}
	TRANS_TABLE.UpdateTable();
	MoveTime	=	time(&now) - Timer;
    std::cout	 << "Move score = " << OPTIMAL_MOVE.second <<  std::endl;
	return OPTIMAL_MOVE;
}
