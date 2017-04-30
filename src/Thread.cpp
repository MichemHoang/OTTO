#include "Thread.h"

using namespace std;
BOARD	INIT;
time_t 	now;
int 	SIGNAL	=	0;
bool	UNLOCK	=	false;
Search	GAME;

void		InitBoard()				{
	FEN_Op::READ_FEN(STANDARD, &INIT);
	//"rnbqk2r/pp3ppp/4pn2/2pp4/1bPP4/2N1PN2/PP3PPP/R1BQKB1R w KQkq - 0 5 "
	BitOp::getBoardInfo(INIT);
}

void Init_engine(){
	cout	<< "MICHEM 1.8 ALPHA ACTIVATED\n";
	INITIALIZE::Mask();
	INITIALIZE::MoveData();
	InitZoBrist(true);
	INIT_BOOK();
	InitBoard();
}

void Display_Move(BOARD A){
	ExtMove ok[265];
	int size=	GENERATE::AllMove(A, ok, A.Side_to_move);
	ExtMove *lo	=	ok;
	for (int t = 0; t < size; t++){
		cout	<< t << ": ";
		DECODE::DecodeMove(lo++);
	}
}

void *Timer(void *){
	bool	Stop = false;
	int SearchTime;
	int startTime = time(&now);
	string line;
	while (!UNLOCK){
		sleep(4);
		SearchTime	=	time(&now) - startTime;
		if (SearchTime > 500 || Stop) {
			GAME.TimeOut(true);
			SIGNAL	=	1;
			while (SIGNAL == 1){}
			startTime = time(&now);
			Stop = false;
		}
	}
	pthread_exit(NULL);
}

void	AIMove(Search *A, int *TotalTime, int level, pair<Move, int> *ANS){
	*ANS	=	A->SearchPosition(level);
	cout	<< "Time = " << A->getTime() << endl;
	*TotalTime	+=	A->getTime();
	Display_Move(INIT);
	DECODE::DecodeMove(ANS->first);
	INIT	=	MOVE::MakeMove(INIT, ANS->first);
	BitOp::getBoardInfo(INIT);
	A->setPosition(INIT);
	cout	<< "FEN STRING = " << FEN_Op::toFEN(INIT) << endl;
	cout	<< "SearchNode = " << GAME.getSearchNode() << endl;
	cout	<< "AverageTime	= " << (double)(*TotalTime)/(double)(INIT.No_Ply) << endl;;
}

void *StartGame(void * threadArg){
	int		level, AIvsAI, MaxMove, Side;
	int		*PTR;
	int		TotalTime		= 0;
	GAME.setTableSize(4000000);
	GAME.setPosition(INIT);
	PTR			=	(int *) threadArg;
	level		=	PTR[0];	MaxMove		=	PTR[1];
	AIvsAI		=	PTR[2];	Side		=	PTR[3];
	pair<Move, int>	RES;
	if (AIvsAI){
		for (int i = 0; i < MaxMove * 2; i++){//MaxMove * 2
			AIMove(&GAME, &TotalTime, level, &RES);
			if (RES.second > 9666)		{cout	<< "Player 1 win\n"; 	break;}
			if (RES.second < -9666)		{cout	<< "Player 2 win\n";	break;}
			//RESET SEARCH TIMER
			SIGNAL = 0;
			GAME.TimeOut(false);
			sleep(3);	//CPU relief
		}
	} else
		for (int i = 0; i < MaxMove * 2; i++){
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
				}
			}  else {
				Display_Move (INIT);
				ExtMove ok[70];
				int		PlayerMove;
				GENERATE::AllMove(INIT, ok, INIT.Side_to_move);
				cout	<< "Your Move " << endl;
				cin		>> PlayerMove;
				DECODE::DecodeMove(ok[PlayerMove].move); cout	<< (int)ok[PlayerMove].move << endl;
				if (PlayerMove < 0) break;
				INIT	=	MOVE::MakeMove(INIT, ok[PlayerMove]);
				BitOp::getBoardInfo(INIT);	
				cout	<< "FEN STRING = " << FEN_Op::toFEN(INIT) << endl;
				GAME.setPosition(INIT);
				//RESET SEARCH TIMER
				GAME.TimeOut(false);
				SIGNAL = 0; 
			}	
		}
	UNLOCK	=	true;
	cout	<< "Total Time = " 		<< TotalTime << endl;
	cout	<< "Evaluate Final Board : " << EVALUATION::Evaluate(INIT, INIT.Side_to_move) << endl;
	pthread_exit(NULL);
}









