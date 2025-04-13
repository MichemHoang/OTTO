#include "search.h"
#include <vector>
#include <chrono>

int max(int A, int B){	if ( A > B ) return A; return B; }
int min(int A, int B){	if ( A <= B ) return A; return B; }

double movegenProfile = 0;
double evalProfile = 0;
double hashProfile = 0;

Search::Search(){
	TRANS_TABLE.SetSize(1000000);
	FEN_Op::READ_FEN(STANDARD, &position);
	searchNode = 0;
}

void Search::TimeOut(bool FIN){
	outOfTime = FIN;
}

void Search::SetPosition(BOARD _position){
	position = _position;
}

void Search::SetPosition(BOARD_C _position){
	boardPosition = _position;
}

void Search::SetTableSize(int   Size){
	TRANS_TABLE.SetSize(Size);
}

uint64_t Search::GetSearchNode(){
	return searchNode;
}

int Search::GetDatabaseSize(){
	return TRANS_TABLE.getSize();
}

int	Search::QuiesceneSearch	(BOARD A, int Alpha, int Beta){
	searchNode++;
	ExtMove CaptureList[20];
	ExtMove *iter =	CaptureList;

	auto start = std::chrono::high_resolution_clock::now();
	int evaluation = EVALUATION::Evaluate(A, A.Side_to_move);
	auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> evalDuration = end - start;
	evalProfile += evalDuration.count(); 

	if (evaluation 	>=	Beta) {	return Beta; }
	if (evaluation  >=	Alpha) { Alpha = evaluation; }
	start = std::chrono::high_resolution_clock::now();
	int CAPTURE_LIST_SIZE	=	GENERATE::CaptureMove(A, iter, A.Side_to_move);
	end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration = end - start;
	movegenProfile += duration.count(); 

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

int	Search::QuiesceneSearch	(BOARD_C board, int Alpha, int Beta){
	searchNode++;
	std::vector<ExtMove> CaptureList;
	int evaluation	=	EVALUATION::EvaluateBoard(board, board.Side_to_move);
	if (evaluation 	>=	Beta) 	{	return Beta;	}
	if (evaluation  >=	Alpha) 	{	Alpha 	=	evaluation;	}
	
	auto start = std::chrono::high_resolution_clock::now();
	CaptureList	= GENERATE::CaptureMoves(board, board.Side_to_move); 
	auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration = end - start;
	movegenProfile += duration.count();

	for (int i = 0; i < CaptureList.size(); i++){
		int iMin = i;
		for (int j = i+1; j < CaptureList.size(); j++)	if (CaptureList[j].value > CaptureList[iMin].value)	iMin	=	j;
		if ( i != iMin )	{
			ExtMove	tmp = CaptureList[i];
			CaptureList[i] = CaptureList[iMin];
			CaptureList[iMin] = tmp;
		}
		evaluation = -QuiesceneSearch(board.MakeMove(CaptureList[i]), -Beta, -Alpha);
		if (evaluation 	>=	Beta) 	{ return Beta; }
		if (evaluation  >=	Alpha) 	{ Alpha = evaluation; }
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
		HashMove.move =	Memo.BestMove;
		HashMoveFound =	true;
		if ( Memo.Depth	>=	(DEPTH)){
			switch (Memo.Node_Type){
				case EXACT :	
                    if (DEPTH != FINAL_DEPT) 
						return std::make_pair(A.PreviousMove, Memo.Evaluation);
                    else 
						return	std::make_pair(Memo.BestMove, Memo.Evaluation);
                case UPPERBOUND	:	Alpha = max(Alpha, Memo.Evaluation); break;
				case LOWERBOUND	:	Beta = min(Beta, Memo.Evaluation); break;
			}
			if (Alpha >= Beta) {
                return std::make_pair(A.PreviousMove, Memo.Evaluation);
			}
		}
	}
	searchNode++;
	
	//----------------------------NEGAMAX--------------------------
	
	//Reaching Leaf node
	if (DEPTH == 0 || A.Pieces[wK + 6* (A.Side_to_move)] == 0) {
		if ( A.Pieces[wK + 6* (A.Side_to_move)] == 0 )	
            return std::make_pair(A.PreviousMove, -19999);
		int evaluation = QuiesceneSearch(A, Alpha, Beta);
        return std::make_pair(A.PreviousMove, evaluation);
	}

	v = MIN_VALUE;
	
	//Best move from hash table
	if (HashMove.move != 0)	{
		BestValue =	v;
		int	Temp = -AlphaBeta(MOVE::MakeMove(A, HashMove), DEPTH - 1, -Beta, -Alpha, 
								   FINAL_DEPT, UpdateKey(ZobristHash, HashMove.move, A),false).second;
		v = max(v, Temp);
		if ( outOfTime == 1) return result;
        if ( BestValue != v ) result = std::make_pair(HashMove.move, v);
		if ( Alpha < v) Alpha = v;
		if ( Alpha >= Beta ) {	
			if (HashMove.getFlags() != 4 )	killerMove[DEPTH][0] = HashMove.move;
			BreakSign = true;	
		}
	}
	
	LateMove = 0;
	if (!BreakSign){	
		auto start = std::chrono::high_resolution_clock::now();
		ListSize = GENERATE::AllMove(A, MoveList, A.Side_to_move);	
		for (int i = 0; i < ListSize; i++){
			if (MoveList[i].move == killerMove[DEPTH][0]){
				MoveList[i].value += 150;
				break;
			}	
		}	
		auto end = std::chrono::high_resolution_clock::now();
    	std::chrono::duration<double> duration = end - start;
		movegenProfile += duration.count();

		for (int moveIterator = 0; moveIterator < ListSize; moveIterator++){
			int iMin = moveIterator;
			for (int j = moveIterator+1; j < ListSize; j++)	if (MoveList[j].value > MoveList[iMin].value)	iMin	=	j;
			if ( moveIterator != iMin )	{
				ExtMove	tmp	= MoveList[moveIterator];
				MoveList[moveIterator]	=	MoveList[iMin];
				MoveList[iMin]		=	tmp;
				if (moveIterator == 0)	{MaxMoveValue	=	MoveList[moveIterator].value;}
			};
			
			//Late Move Reduction;
			if (!CheckLMR){
				if (shallowSearch && (moveIterator > 4 || moveIterator > ListSize/4) && !LMR){
					if (DEPTH > 2 && DEPTH <= FINAL_DEPT - 1) {
						if (HashMoveFound || MaxMoveValue > 200){
							LMR	= true;
							CheckLMR = true;
							LateMove = 2;
						} 
					}
				}
			}
			if (MoveList[moveIterator].move != HashMove.move){
				BestValue = v;
				int	Temp = -AlphaBeta(MOVE::MakeMove(A, MoveList[moveIterator]), DEPTH - 1 - LateMove, -Beta, -Alpha, 
											FINAL_DEPT, UpdateKey(ZobristHash, MoveList[moveIterator].move, A), LMR).second;
				v = max(v, Temp);
				if ( BestValue != v &&  LMR && LateMove != 2){
					v =	BestValue;
					Temp = -AlphaBeta(MOVE::MakeMove(A, MoveList[moveIterator]), DEPTH - 1, -Beta, -Alpha, 
										    FINAL_DEPT, UpdateKey(ZobristHash, MoveList[moveIterator].move, A), false).second;
					v = max(v, Temp);
				}
				if (outOfTime == 1)	return result;
                if ( BestValue != v ) result = std::make_pair(MoveList[moveIterator].move, v);
				if ( Alpha < v) Alpha = v;
				if ( Alpha >= Beta )  {	
					if (MoveList[moveIterator].getFlags() != 4 ) killerMove[DEPTH][0] = MoveList[moveIterator].move;
					break;
				}
			}
		}
	}
	
    //---------------------STORE_NODES_IN_DATABASE---------------------
	auto startH = std::chrono::high_resolution_clock::now();
	uint8_t flag;
	if (BestValue <= Alpha0) flag = UPPERBOUND;
	else if (BestValue	>=	Beta) flag = LOWERBOUND;
	else flag = EXACT;
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

ExtMove Search::AlphaBeta (BOARD_C board, int depth, int alpha, int beta, int finalDepth, Key zobristHash, bool lateMoveReduction){
	bool breakSign = false;
	outOfTime = false;
    ExtMove result;
	std::vector<ExtMove> moveList;
	int	alpha0 = alpha;
	int	currentSearchValue, bestValue, lateMove;
	ExtMove	hashMove;
	bool hashMoveFound = false;
	int maxMoveValue = 0;
	bool checkLMR =	false;
	hashMove.move =	0;
	HashEntry memo;

	//std::cout << "Check if value is in Hash Table\n";
	//-------------------Check if value is in Hash Table-----------------
	if (TRANS_TABLE.FindEntry(zobristHash, &memo)){
		hashMove.move = memo.BestMove;
		hashMoveFound = true;
		if ( memo.Depth	>= (depth)){
			switch (memo.Node_Type){
				case EXACT :	
                    if (depth != finalDepth){
						result.move = board.PreviousMove;
						result.searchValue = memo.Evaluation;
						return result;
					} else {
						result.move = memo.BestMove;
						result.searchValue = memo.Evaluation;
						return result;
					}
                case UPPERBOUND : alpha = max(alpha, memo.Evaluation); break;
				case LOWERBOUND : beta = min(beta, memo.Evaluation); break;
			}
			if (alpha >= beta) {
                result.move = board.PreviousMove;
				result.searchValue = memo.Evaluation;
				return result;
			}
		}
	}
	searchNode++;
	
	//Reaching Leaf node
	if (depth == 0 || board.Pieces[wK + 6* (board.Side_to_move)] == 0) {
		//std::cout << "Reaching Leaf node\n";
		if ( board.Pieces[wK + 6* (board.Side_to_move)] == 0 )	{
			result.move = board.PreviousMove;
			result.searchValue = -19999;
			return  result;
		}
		int evaluation = QuiesceneSearch(board, alpha, beta);
		result.move = board.PreviousMove;
		result.searchValue = evaluation;
        return  result;
	}

	currentSearchValue = MIN_VALUE;
	
	//Best move from hash table
	if (hashMove.move != 0)	{
		bestValue = currentSearchValue;
		int	value = -AlphaBeta(board.MakeMove(hashMove), depth - 1, -beta, -alpha, 
								   finalDepth, UpdateKey(zobristHash, hashMove.move, board), false).searchValue;
		
		currentSearchValue = max(currentSearchValue, value);
		if ( outOfTime == 1 ) return result;
        if ( bestValue != currentSearchValue ) {
			result.move = hashMove.move;
			result.searchValue = currentSearchValue;
		}
		if ( alpha < currentSearchValue) alpha = currentSearchValue;
		if ( alpha >= beta )  {	
			if (hashMove.getFlags() != 4 )	killerMove[depth][0] = hashMove.move;
			breakSign = true;	
		}
	}
	lateMove = 0;

	if (!breakSign){
		auto start = std::chrono::high_resolution_clock::now();
		moveList = GENERATE::AllMoves(board, board.Side_to_move); //generate all moves
		auto end = std::chrono::high_resolution_clock::now();
		std::chrono::duration<double> duration = end - start;
		movegenProfile += duration.count();
		//check if move are in killerMove (previously search)
		for (int i = 0; i < moveList.size(); i++){
			if (moveList[i].move == killerMove[depth][0]){
				moveList[i].value = 300;
				break;
			}	
		}	

		//std::cout << "actual alphaBeta\n";
		for (int moveIterator = 0; moveIterator < moveList.size(); moveIterator++){
			int maxPosition = moveIterator;
			//selection sort since the moveList array is quite small, smaller overhead
			//also if cut off happen we dont have to sort the rest of the list
			//tiny optimization victory - or maybe doesnt even matter at all, oh well
			for (int j = moveIterator+1; j < moveList.size(); j++) if (moveList[j].value > moveList[maxPosition].value) maxPosition = j;
			if ( moveIterator != maxPosition )	{
				ExtMove	tmp	= moveList[moveIterator];
				moveList[moveIterator] = moveList[maxPosition];
				moveList[maxPosition] = tmp;
				if (moveIterator ==	0)	{maxMoveValue = moveList[moveIterator].value;}
			};
			
			//Late Move Reduction;
			if (!checkLMR){
				if (shallowSearch && (moveIterator > 4 || moveIterator > moveList.size()/4) && !lateMoveReduction){
					if (depth > 2 && depth <= finalDepth - 1) {
						if (hashMoveFound || maxMoveValue > 200){
							lateMoveReduction = true;
							checkLMR = true;
							lateMove = 2;
						} 
					}
				}
			}

			//need to reexamine the latemove reduction (logic seems to be off)
			if (moveList[moveIterator].move != hashMove.move){
				bestValue =	currentSearchValue;
				int	Temp = -AlphaBeta(board.MakeMove(moveList[moveIterator]), depth - 1 - lateMove, -beta, -alpha, 
									finalDepth, UpdateKey(zobristHash, moveList[moveIterator].move, board), lateMoveReduction).searchValue;
				currentSearchValue = max(currentSearchValue, Temp);
				if ( bestValue != currentSearchValue && lateMoveReduction && lateMove != 2){
					currentSearchValue = bestValue;
					Temp = -AlphaBeta(board.MakeMove(moveList[moveIterator]), depth - 1, -beta, -alpha, 
								finalDepth, UpdateKey(zobristHash, moveList[moveIterator].move, board), false).searchValue;
					currentSearchValue = max(currentSearchValue, Temp);
				}
				if (outOfTime == 1)	return result;
				if ( bestValue != currentSearchValue ) 	{
					result.move	= moveList[moveIterator].move;
					result.searchValue = currentSearchValue;
				}
				if ( alpha < currentSearchValue) alpha = currentSearchValue;
				if ( alpha >= beta ){	
					if (moveList[moveIterator].getFlags() != 4 ) killerMove[depth][0] = moveList[moveIterator].move;
					break;
				}
			}
		}
	}

    //---------------------STORE_NODES_IN_DATABASE---------------------
	uint8_t flag;
	if (bestValue <= alpha0) flag = UPPERBOUND;
	else if (bestValue >= beta) flag = LOWERBOUND;
	else flag = EXACT;
	HashEntry newEntry;
	newEntry.BestMove	=	result.move;
	newEntry.Depth		=	depth;
	newEntry.Flag		=	false;
	newEntry.Node_Type	=	flag;
	newEntry.Evaluation	=	currentSearchValue;
	newEntry.HashValue	=	zobristHash;
	TRANS_TABLE.addEntry(zobristHash, newEntry);
	
	return result;
}

ExtMove Search::IterativeDeepening(int maxDepth, bool iterative){
	std::cout << "begin alphabeta search\n";
	BitString boardHashValue = GetKeyString(boardPosition);
	int	alpha = MIN_VALUE;
	int beta = MAX_VALUE;
    ExtMove optimalMove;
	ExtMove previousMove;
	movegenProfile = 0;
	evalProfile = 0;
	if (iterative){
		shallowSearch = false;
		for (int depthReduce = 4; depthReduce >= 0; depthReduce-=2){ //
		std::cout << "iterative search\n";
			searchNode = 0;
			optimalMove	= AlphaBeta(boardPosition, maxDepth - depthReduce, alpha, beta, maxDepth - depthReduce, boardHashValue, false);
			TRANS_TABLE.UpdateTable();
			shallowSearch = true;
			if (outOfTime) optimalMove = previousMove;
			else previousMove = optimalMove;
			alpha =	optimalMove.searchValue - 250;
			beta = optimalMove.searchValue + 250;
		}
	}
	TRANS_TABLE.UpdateTable();
	std::cout << "Move = " << std::hex << optimalMove.move << std::endl;
	std::cout << "Move score = " << optimalMove.searchValue << std::endl;
	std::cout << "MovegenProfile = " << movegenProfile << "\n";
	std::cout << "EvalProfile = " << evalProfile << "\n";
	std::cout << "search Node = " << std::dec << searchNode << "\n";
	std::cout << "Database size = " << std::dec  <<  TRANS_TABLE.getSize() << "\n";
	return optimalMove;
}

std::pair<Move, int> Search::IterativeDeepening (int MAX_DEPTH){
	BitString BoardHashValue = GetKey(position);
	int	Alpha =	MIN_VALUE;
	int Beta = MAX_VALUE;
    std::pair<Move, int> STORE_MOVE;
    std::pair<Move, int> OPTIMAL_MOVE;
	movegenProfile = 0;
	evalProfile = 0;
	hashProfile = 0;
    outOfTime = false;
		
	//Iterative deepening search loop
	shallowSearch = false;
	for (int depthReduce = 8; depthReduce <= MAX_DEPTH; depthReduce+=2){
		searchNode = 0;
		OPTIMAL_MOVE = AlphaBeta(position, depthReduce, Alpha, Beta, depthReduce, BoardHashValue, false);
		TRANS_TABLE.UpdateTable();
		std::cout << "Optimal move score = " << OPTIMAL_MOVE.second << std::endl;
		shallowSearch = true;
		if (outOfTime) OPTIMAL_MOVE = STORE_MOVE;
		else STORE_MOVE = OPTIMAL_MOVE;
		Alpha = STORE_MOVE.second - 50;
		Beta = STORE_MOVE.second + 50;
	}
	TRANS_TABLE.UpdateTable();
    std::cout << "Move score = " << OPTIMAL_MOVE.second <<  std::endl;
	std::cout << "movegenProfile = " << movegenProfile << "\n";
	std::cout << "evalProfile = " << evalProfile << "\n";
	std::cout << "search Node = " << std::dec << searchNode << "\n";
	std::cout << "Database size = " << std::dec <<  TRANS_TABLE.getSize() << "\n";
	return OPTIMAL_MOVE;
}
