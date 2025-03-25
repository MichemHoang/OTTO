#include "game.h"

void Game::InitBoard()				{
    FEN_Op::READ_FEN("4rb2/3qrk2/1p1p1n2/7p/P2P4/4R2P/1BQN1P2/1K4R1 w - - 3 39", &initialPosition);
	//FEN_Op::READ_FEN(STANDARD, &initialPosition);
	BitOp::getBoardInfo(initialPosition);
}

void Game::Init_engine(){
    std::cout << sizeof(HashEntry) << std::endl;
	INITIALIZE::Mask();
	INITIALIZE::MoveData();
    InitZoBrist(true);
	InitBoard();
    openingBook.Init();
    stopSig	= BEGIN_SEARCH;
	unlock = false;
}

void Game::DisplayMove(BOARD_C board, int MType){
	std::vector<ExtMove> moveList;
	switch (MType){
		case (0):	moveList = GENERATE::AllMoves(board, board.Side_to_move); break;
		case (1):	moveList = GENERATE::CaptureMoves(board, board.Side_to_move); 
                    std::cout <<"Gen Capture\n";
                    std::cout << "Capture list size = " << moveList.size() << std::endl;
					break;
		case (2):	moveList = GENERATE::QuietMoves(board, board.Side_to_move); 
                    std::cout <<"Gen Quiet\n";
                    std::cout << "Quiet list size = " << moveList.size() << std::endl;
					break;	
	}
	for (int t = 0; t < moveList.size(); t++){
        std::cout << t << ": ";
	}
}

void Game::Display_Move(BOARD A, int MType){
	ExtMove ok[MAX_MOVES];
	int size;
	switch (MType){
		case (0):	size = GENERATE::AllMove(A, ok, A.Side_to_move); break;
		case (1):	size = GENERATE::CaptureMove(A, ok, A.Side_to_move); 
                    std::cout <<"Gen Capture\n";
                    std::cout << "size = " << size << std::endl;
					break;
		case (2):	size = GENERATE::QuietMove(A, ok, A.Side_to_move); 
                    std::cout <<"Gen Quiet\n";
                    std::cout << "size = " << size << std::endl;
					break;
	}
	ExtMove*lo	=	ok;
	for (int t = 0; t < size; t++){
        std::cout	<< t << ": ";
	}
}

//this isnt safe
void Game::Timer(){
	bool Stop = false;
	int SearchTime;
	int startTime;
    std::string line;
	while (!unlock){
        switch (stopSig){
		case BEGIN_SEARCH:
			startTime = time(&now);
            stopSig	=	SEARCHING;
			break;
		case SHOWTIME:
            std::cout	<< "SearchTime so far: " << SearchTime << std::endl;
            stopSig	=	SEARCHING;
			break;
		case TIMEOUT:
			Stop = true;
			break;
		}
		sleep(2);
		SearchTime	=	time(&now) - startTime;
		if (SearchTime > 5000 || Stop) {
            std::cout	<< "Time Out";
			SearchTime	=	0;
			game.TimeOut(true);
            stopSig	=	WAITING;
            while (stopSig == WAITING){}
			startTime = time(&now);
			Stop = false;
		}
	}
}

void Game::AIMove(Search *A, int *TotalTime, int level, std::pair<Move, int> *ANS){
	*ANS	=	A->SearchPosition(level);
    std::cout	<< "Time = " << A->GetTime() << std::endl;
	*TotalTime	+=	A->GetTime();
	Display_Move(initialPosition, 0);
	initialPosition	= MOVE::MakeMove(initialPosition, ANS->first);
	BitOp::getBoardInfo(initialPosition);
	A->SetPosition(initialPosition);
    std::cout	<< "Database size = " << game.GetDatabaseSize() << std::endl;
    std::cout	<< "FEN std::string = " << FEN_Op::toFEN(initialPosition) << std::endl;
    std::cout	<< "SearchNode = " << game.GetSearchNode() << std::endl;
}

void Game::Start(void * GameArg){
	int	level, AIvsAI, MaxMove, Side;
	int	*PTR;
	int	TotalTime = 0;
	game.SetTableSize(30000000);
	game.SetPosition(initialPosition);
    PTR	= (int *) GameArg;
	level =	PTR[0];	MaxMove = PTR[1];
	AIvsAI = PTR[2]; Side = PTR[3];
    stopSig	= BEGIN_SEARCH;
    std::pair<Move, int> RES;
	if (AIvsAI){
		for (int i = 0; i < MaxMove * 2 - 1; i++){//MaxMove * 2
			AIMove(&game, &TotalTime, level, &RES);
            if (RES.second > 9666)		{std::cout	<< "Player 1 win\n"; 	break;}
            if (RES.second < -9666)		{std::cout	<< "Player 2 win\n";	break;}
            std::cout	<< "AverageTime	= " << (double)(TotalTime)/(double)(i + 1) << std::endl;
			//RESET SEARCH TIMER
            stopSig = BEGIN_SEARCH;
			game.TimeOut(false);
			sleep(1);	//CPU relief
		}
	} else
		for (int i = 0; i < MaxMove * 2 - 1; i++){
			if (initialPosition.Side_to_move == Side){
				bool FoundOpening = false;
				if (initialPosition.No_Ply <=10){
					Key Hash = GetKey(initialPosition);
					Move Koas;
                    if (openingBook.FindOpening(Hash, &Koas)) {
						//
						initialPosition	=	MOVE::MakeMove(initialPosition, Koas);
						BitOp::getBoardInfo(initialPosition);
						FoundOpening = true;	
						game.SetPosition(initialPosition);
					}
				} 
				if (FoundOpening){}
				else {
					AIMove(&game, &TotalTime, level, &RES);
                    if (RES.second > 9666)		{std::cout	<< "Player 1 win\n"; 	break;}
                    if (RES.second < -9666)		{std::cout	<< "Player 2 win\n";	break;}
                    std::cout	<< "AverageTime	= " << (double)(TotalTime)/(double)(i/2 + 1) << std::endl;
				}
			}  else {
				Display_Move (initialPosition, 0);
				ExtMove ok[MAX_MOVES];
				int	PlayerMove;
				GENERATE::AllMove(initialPosition, ok, initialPosition.Side_to_move);
                std::cout	<< "Your Move " << std::endl;
                std::cin		>> PlayerMove;
                //
				std::cout	<< (int)ok[PlayerMove].move << std::endl;
				if (PlayerMove < 0) {
                    stopSig = SEARCHING;
					break;
				}
				initialPosition	=	MOVE::MakeMove(initialPosition, ok[PlayerMove]);
				BitOp::getBoardInfo(initialPosition);	
                std::cout	<< "FEN std::string = " << FEN_Op::toFEN(initialPosition) << std::endl;
				game.SetPosition(initialPosition);
				//RESET SEARCH TIMER
				game.TimeOut(false);
                stopSig = BEGIN_SEARCH;
			}	
		}
	unlock	=	true;
    std::cout	<< "Total Time = " 		<< TotalTime << std::endl;
    std::cout	<< "Evaluate Final Board : " << EVALUATION::Evaluate(initialPosition, initialPosition.Side_to_move) << std::endl;
    std::cout	<< "Endgame (Press sth)\n";
}

void Game::SignalHandler(){
	int UserInput;
	while (!unlock && false){
        std::cin >> UserInput;
		if (UserInput < 3 && UserInput > -3)
            stopSig	=	UserInput;
	}
}









