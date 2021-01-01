#include "game.h"

void	Game::InitBoard()				{
    FEN_Op::READ_FEN(STANDARD, &INIT);
	BitOp::getBoardInfo(INIT);
}

void 	Game::Init_engine(){
    std::cout	<< sizeof(HashEntry) << std::endl;
	INITIALIZE::Mask();
	INITIALIZE::MoveData();
    InitZoBrist(true);
	InitBoard();
    book.Init();
    Sig	=	BEGIN_SEARCH;
	UNLOCK	=	false;
}

void 	Game::Display_Move(BOARD A, int MType){
	ExtMove ok[MAX_MOVES];
	int size;
	switch (MType){
		case (0):	size=	GENERATE::AllMove(A, ok, A.Side_to_move); break;
		case (1):	size=	GENERATE::CaptureMove(A, ok, A.Side_to_move); 
                    std::cout <<"Gen Capture\n";
                    std::cout << "size = " << size << std::endl;
					break;
		case (2):	size=	GENERATE::QuietMove(A, ok, A.Side_to_move); 
                    std::cout <<"Gen Quiet\n";
                    std::cout << "size = " << size << std::endl;
					break;
	}
	ExtMove*lo	=	ok;
	for (int t = 0; t < size; t++){
        std::cout	<< t << ": ";
		DECODE::DecodeMove(lo++);
	}
}

void 	Game::Timer(){
	bool	Stop = false;
	int SearchTime;
	int startTime;
    std::string line;
	while (!UNLOCK){
        switch (Sig){
		case BEGIN_SEARCH:
			startTime = time(&now);
            Sig	=	SEARCHING;
			break;
		case SHOWTIME:
            std::cout	<< "SearchTime so far: " << SearchTime << std::endl;
            Sig	=	SEARCHING;
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
			GAME.TimeOut(true);
            Sig	=	WAITING;
            while (Sig == WAITING){}
			startTime = time(&now);
			Stop = false;
		}
	}
}

void	Game::AIMove(Search *A, int *TotalTime, int level, std::pair<Move, int> *ANS){
	*ANS	=	A->SearchPosition(level);
    std::cout	<< "Time = " << A->getTime() << std::endl;
	*TotalTime	+=	A->getTime();
	Display_Move(INIT, 0);
	DECODE::DecodeMove(ANS->first);
	INIT	=	MOVE::MakeMove(INIT, ANS->first);
	BitOp::getBoardInfo(INIT);
	A->setPosition(INIT);
    std::cout	<< "Database size = " << GAME.getDatabaseSize() << std::endl;
    std::cout	<< "FEN std::string = " << FEN_Op::toFEN(INIT) << std::endl;
    std::cout	<< "SearchNode = " << GAME.getSearchNode() << std::endl;
}

void 	Game::StartGame(void * GameArg){
	int		level, AIvsAI, MaxMove, Side;
	int		*PTR;
	int		TotalTime		= 0;
	GAME.setTableSize(30000000);
	GAME.setPosition(INIT);
    PTR			=	(int *) GameArg;
	level		=	PTR[0];	MaxMove		=	PTR[1];
	AIvsAI		=	PTR[2];	Side		=	PTR[3];
    Sig	=	BEGIN_SEARCH;
    std::pair<Move, int>	RES;
	if (AIvsAI){
		for (int i = 0; i < MaxMove * 2 - 1; i++){//MaxMove * 2
			AIMove(&GAME, &TotalTime, level, &RES);
            if (RES.second > 9666)		{std::cout	<< "Player 1 win\n"; 	break;}
            if (RES.second < -9666)		{std::cout	<< "Player 2 win\n";	break;}
            std::cout	<< "AverageTime	= " << (double)(TotalTime)/(double)(i + 1) << std::endl;
			//RESET SEARCH TIMER
            Sig = BEGIN_SEARCH;
			GAME.TimeOut(false);
			sleep(1);	//CPU relief
		}
	} else
		for (int i = 0; i < MaxMove * 2 - 1; i++){
			if (INIT.Side_to_move == Side){
				bool FoundOpening = false;
				if (INIT.No_Ply <=10){
					Key Hash = GetKey(INIT);
					Move Koas;
                    if (book.FindOpening(Hash, &Koas)) {
						DECODE::DecodeMove(Koas);
						INIT	=	MOVE::MakeMove(INIT, Koas);
						BitOp::getBoardInfo(INIT);
						FoundOpening = true;	
						GAME.setPosition(INIT);
					}
				} 
				if (FoundOpening){}
				else {
					AIMove(&GAME, &TotalTime, level, &RES);
                    if (RES.second > 9666)		{std::cout	<< "Player 1 win\n"; 	break;}
                    if (RES.second < -9666)		{std::cout	<< "Player 2 win\n";	break;}
                    std::cout	<< "AverageTime	= " << (double)(TotalTime)/(double)(i/2 + 1) << std::endl;
				}
			}  else {
				Display_Move (INIT, 0);
				ExtMove ok[MAX_MOVES];
				int		PlayerMove;
				GENERATE::AllMove(INIT, ok, INIT.Side_to_move);
                std::cout	<< "Your Move " << std::endl;
                std::cin		>> PlayerMove;
                DECODE::DecodeMove(ok[PlayerMove].move); std::cout	<< (int)ok[PlayerMove].move << std::endl;
				if (PlayerMove < 0) {
                    Sig = SEARCHING;
					break;
				}
				INIT	=	MOVE::MakeMove(INIT, ok[PlayerMove]);
				BitOp::getBoardInfo(INIT);	
                std::cout	<< "FEN std::string = " << FEN_Op::toFEN(INIT) << std::endl;
				GAME.setPosition(INIT);
				//RESET SEARCH TIMER
				GAME.TimeOut(false);
                Sig = BEGIN_SEARCH;
			}	
		}
	UNLOCK	=	true;
    std::cout	<< "Total Time = " 		<< TotalTime << std::endl;
    std::cout	<< "Evaluate Final Board : " << EVALUATION::Evaluate(INIT, INIT.Side_to_move) << std::endl;
    std::cout	<< "Endgame (Press sth)\n";
}

void	Game::SignalHandler(){
	int UserInput;
	while (!UNLOCK && false){
        std::cin >> UserInput;
		if (UserInput < 3 && UserInput > -3)
            Sig	=	UserInput;
	}
}









