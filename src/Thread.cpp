#include "Thread.h"

void	THREAD::InitBoard()				{
	FEN_Op::READ_FEN(STANDARD, &INIT);
	//FEN_Op::READ_FEN("rnbqk2r/pp3ppp/4pn2/2pp4/1bPP4/2N1PN2/PP3PPP/R1BQKB1R w KQkq - 0 5 ", &INIT);
	BitOp::getBoardInfo(INIT);
}

void 	THREAD::Init_engine(){
	cout	<< sizeof(HashEntry) << endl;
	cout	<< "MICHEM 1.8 ALPHA ACTIVATED\n";
	INITIALIZE::Mask();
	INITIALIZE::MoveData();
	InitZoBrist(true);
	INIT_BOOK();
	InitBoard();
	SIGNAL	=	BEGIN_SEARCH;
	UNLOCK	=	false;
}

void 	THREAD::Display_Move(BOARD A, int MType){
	ExtMove ok[MAX_MOVES];
	int size;
	switch (MType){
		case (0):	size=	GENERATE::AllMove(A, ok, A.Side_to_move); break;
		case (1):	size=	GENERATE::CaptureMove(A, ok, A.Side_to_move); 
					cout <<"Gen Capture\n"; 
					cout << "size = " << size << endl;
					break;
		case (2):	size=	GENERATE::QuietMove(A, ok, A.Side_to_move); 
					cout <<"Gen Quiet\n"; 
					cout << "size = " << size << endl;
					break;
	}
	ExtMove*lo	=	ok;
	for (int t = 0; t < size; t++){
		cout	<< t << ": ";
		DECODE::DecodeMove(lo++);
	}
}

void 	THREAD::Timer(){
	bool	Stop = false;
	int SearchTime;
	int startTime;
	string line;
	while (!UNLOCK){
		switch (SIGNAL){
		case BEGIN_SEARCH:
			startTime = time(&now);
			SIGNAL	=	SEARCHING;
			break;
		case SHOWTIME:
			cout	<< "SearchTime so far: " << SearchTime << endl;
			SIGNAL	=	SEARCHING;
			break;
		case TIMEOUT:
			Stop = true;
			break;
		}
		sleep(2);
		SearchTime	=	time(&now) - startTime;
		if (SearchTime > 5000 || Stop) {
			cout	<< "Time Out";
			SearchTime	=	0;
			GAME.TimeOut(true);
			SIGNAL	=	WAITING;
			while (SIGNAL == WAITING){}
			startTime = time(&now);
			Stop = false;
		}
	}
}

void	THREAD::AIMove(Search *A, int *TotalTime, int level, pair<Move, int> *ANS){
	*ANS	=	A->SearchPosition(level);
	cout	<< "Time = " << A->getTime() << endl;
	*TotalTime	+=	A->getTime();
	Display_Move(INIT, 0);
	DECODE::DecodeMove(ANS->first);
	INIT	=	MOVE::MakeMove(INIT, ANS->first);
	BitOp::getBoardInfo(INIT);
	A->setPosition(INIT);
	cout	<< "Database size = " << GAME.getDatabaseSize() << endl;
	cout	<< "FEN STRING = " << FEN_Op::toFEN(INIT) << endl;
	cout	<< "SearchNode = " << GAME.getSearchNode() << endl;
}

void 	THREAD::StartGame(void * threadArg){
	int		level, AIvsAI, MaxMove, Side;
	int		*PTR;
	int		TotalTime		= 0;
	GAME.setTableSize(30000000);
	GAME.setPosition(INIT);
	PTR			=	(int *) threadArg;
	level		=	PTR[0];	MaxMove		=	PTR[1];
	AIvsAI		=	PTR[2];	Side		=	PTR[3];
	SIGNAL	=	BEGIN_SEARCH;
	pair<Move, int>	RES;
	if (AIvsAI){
		for (int i = 0; i < MaxMove * 2 - 1; i++){//MaxMove * 2
			AIMove(&GAME, &TotalTime, level, &RES);
			if (RES.second > 9666)		{cout	<< "Player 1 win\n"; 	break;}
			if (RES.second < -9666)		{cout	<< "Player 2 win\n";	break;}
			cout	<< "AverageTime	= " << (double)(TotalTime)/(double)(i + 1) << endl;
			//RESET SEARCH TIMER
			SIGNAL = BEGIN_SEARCH;
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
					if (FindOpening(Hash, &Koas)) {
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
					if (RES.second > 9666)		{cout	<< "Player 1 win\n"; 	break;}
					if (RES.second < -9666)		{cout	<< "Player 2 win\n";	break;}
					cout	<< "AverageTime	= " << (double)(TotalTime)/(double)(i/2 + 1) << endl;
				}
			}  else {
				Display_Move (INIT, 0);
				ExtMove ok[MAX_MOVES];
				int		PlayerMove;
				GENERATE::AllMove(INIT, ok, INIT.Side_to_move);
				cout	<< "Your Move " << endl;
				cin		>> PlayerMove;
				DECODE::DecodeMove(ok[PlayerMove].move); cout	<< (int)ok[PlayerMove].move << endl;
				if (PlayerMove < 0) {
					SIGNAL = SEARCHING;
					break;
				}
				INIT	=	MOVE::MakeMove(INIT, ok[PlayerMove]);
				BitOp::getBoardInfo(INIT);	
				cout	<< "FEN STRING = " << FEN_Op::toFEN(INIT) << endl;
				GAME.setPosition(INIT);
				//RESET SEARCH TIMER
				GAME.TimeOut(false);
				SIGNAL = BEGIN_SEARCH; 
			}	
		}
	UNLOCK	=	true;
	cout	<< "Total Time = " 		<< TotalTime << endl;
	cout	<< "Evaluate Final Board : " << EVALUATION::Evaluate(INIT, INIT.Side_to_move) << endl;
	cout	<< "Endgame (Press sth)\n";
}

void	THREAD::SignalHandler(){
	int UserInput;
	while (!UNLOCK && false){
		cin >> UserInput;
		if (UserInput < 3 && UserInput > -3)
			SIGNAL	=	UserInput;
	}
}









