#include "Thread.h"

using namespace std;
BOARD	INIT;
time_t 	now;
int SIGNAL;
bool	LOCK	=	false;

void		InitBoard()				{
	READ_FEN(STANDARD, &INIT);
	getBoardInfo(INIT);
}

void Init_engine(){
	cout	<< "MICHEM 1.5 ALPHA ACTIVATED\n";
	GenerateMask();
	InitZoBrist(true);
	INIT_BOOK();
	GenerateMoveData();
	InitBoard();
}

void Display_Move(BOARD A){
	ExtMove ok[265];
	int size=	GenerateAllMove(A, ok, A.Side_to_move);
	ExtMove *lo	=	ok;
	for (int t = 0; t < size; t++){
		cout	<< t << ": ";
		DecodeMove(lo++);
	}
}

HalfMoveInfo AI_Move ( BOARD A, int level, int *TotMove, int *TotTime, Move DrawCondition ) {
	int start_time;
	HalfMoveInfo INFO;
	start_time	=	time(&now);
	INFO.Result	=	DEEPENING_SEARCH(A, level, 2, DrawCondition);
	INFO.Time	=	time(&now) - start_time;
	*TotTime	+=	INFO.Time;
	cout	<<  "time = " << INFO.Time << endl;
	*TotMove	+=	1;
	Display_Move(INIT);
	DecodeMove(INFO.Result.first);
	INIT	=	MakeMove(A, INFO.Result.first);
	getBoardInfo(INIT);
	return INFO;
}

void *Clock(void *){
	while (!LOCK){
	}
	pthread_exit(NULL);
}

void *Start_Game(void * threadArg){
	int	level, AIvsAI, MaxMove, Side;
	int	*PTR;
	PTR			=	(int *) threadArg;
	level		=	PTR[0];	MaxMove		=	PTR[1];
	AIvsAI		=	PTR[2];	Side		=	PTR[3];
	HalfMoveInfo	RES;
	Move 	DrawCondition 	= 0;
	double 	averageTime 	= 0;
	int		totalTime		= 0;
	int		TotalMove		= 0;
	int		MaxTime			= 0;
	if (AIvsAI){
		for (int i = 0; i < MaxMove * 2; i++){//MaxMove * 2
			if (INIT.No_Ply	>= 8){
				if ((INIT.PrevMove[INIT.No_Ply - 1] == INIT.PrevMove[INIT.No_Ply - 5]) && 
					(INIT.PrevMove[INIT.No_Ply - 9] == INIT.PrevMove[INIT.No_Ply - 5])){
					if (INIT.PrevMove[INIT.No_Ply - 2] == INIT.PrevMove[INIT.No_Ply - 6]) {
						DrawCondition	=	INIT.PrevMove[INIT.No_Ply - 2];
					}
				} else DrawCondition	=	0;
			} 
			RES	=	AI_Move(INIT, level, &TotalMove, &totalTime, DrawCondition);
			if (RES.Result.second > 9666)		{cout	<< "Player 1 win\n"; 	break;}
			if (RES.Result.second < -9666)		{cout	<< "Player 2 win\n";	break;}
			if (RES.Time > MaxTime)	MaxTime	=	RES.Time;
			averageTime	=	(double)totalTime/(double)TotalMove;
			if (RES.Result.first == DrawCondition) {cout	<< "DRAW\n"; 	break;}
			cout	<< "Average Time = " 	<< averageTime << endl;	
			cout	<< "\nMove n0 " << TotalMove << endl;	
			cout	<< "FEN STRING = " << toFEN(INIT) << endl;
			if (INIT.No_Ply > 15){
				if ((INIT.PrevMove[INIT.No_Ply - 1] == INIT.PrevMove[INIT.No_Ply - 5]) && 
						(INIT.PrevMove[INIT.No_Ply - 9] == INIT.PrevMove[INIT.No_Ply - 5])){
						if ((INIT.PrevMove[INIT.No_Ply - 2] == INIT.PrevMove[INIT.No_Ply - 6]) && 
						(INIT.PrevMove[INIT.No_Ply - 10] == INIT.PrevMove[INIT.No_Ply - 6])){
							cout	<< "DRAW\n";
							break;
						}
				}
			}
		}
	} else {
		for (int i = 0; i < MaxMove; i++){
			bool FoundOpening = false;
			if (INIT.No_Ply <=10){
				Key Hash = GetKey(INIT);
				Move Koas;
				if (FindOpening(Hash, &Koas)) {
					DecodeMove(Koas);
					INIT	=	MakeMove(INIT, Koas);
					getBoardInfo(INIT);
					FoundOpening = true;
					cout	<< "Move n0 " << TotalMove << endl;		
					averageTime	=	2*(double)totalTime/(double)(TotalMove+1);
					cout	<< "Average Time = " 	<< averageTime << endl;
					cout	<< "\nMove n0 " << TotalMove << endl;	
					cout	<< "FEN STRING = " << toFEN(INIT) << endl;
				}
			}
			if (FoundOpening){}
			else {
				if (Side	==	WHITE || INIT.No_Ply > 0){
				//-------------------------Player 1(AI)--------------------------
					if (INIT.No_Ply	>= 6){
						if ((INIT.PrevMove[INIT.No_Ply - 1] == INIT.PrevMove[INIT.No_Ply - 3]) && 
							(INIT.PrevMove[INIT.No_Ply - 3] == INIT.PrevMove[INIT.No_Ply - 5])){
							if (INIT.PrevMove[INIT.No_Ply - 2] == INIT.PrevMove[INIT.No_Ply - 4]) 
								DrawCondition	=	INIT.PrevMove[INIT.No_Ply - 2];
						} else DrawCondition	=	0;
					} 
					RES	=	AI_Move(INIT, level, &TotalMove, &totalTime,DrawCondition);
					if (RES.Result.second > 16666)		{cout	<< "Player 1 win\n"; 	break;}
					if (RES.Result.second < -16666)		{cout	<< "Player 2 win\n";	break;}
					if (RES.Time > MaxTime)	MaxTime	=	RES.Time;
					if (RES.Result.first == DrawCondition) {cout	<< "DRAW\n"; 	break;}
					cout	<< "Move n0 " << TotalMove << endl;		
					averageTime	=	2*(double)totalTime/(double)(TotalMove+1);
					cout	<< "Average Time = " 	<< averageTime << endl;
					cout	<< "\nMove n0 " << TotalMove << endl;	
					cout	<< "FEN STRING = " << toFEN(INIT) << endl;
				}//*/
			}
			//--------------------------Player 2(Human)-----------------------
			if (INIT.No_Ply	>= 6){
				if ((INIT.PrevMove[INIT.No_Ply - 1] == INIT.PrevMove[INIT.No_Ply - 3]) && 
					(INIT.PrevMove[INIT.No_Ply - 3] == INIT.PrevMove[INIT.No_Ply - 5])){
					if (INIT.PrevMove[INIT.No_Ply - 2] == INIT.PrevMove[INIT.No_Ply - 4]) 
						DrawCondition	=	INIT.PrevMove[INIT.No_Ply - 2];
				} else DrawCondition	=	0;
			} 
			Display_Move (INIT);
			ExtMove ok[70];
			int		PlayerMove;
			GenerateAllMove(INIT, ok, INIT.Side_to_move);
			cout	<< "Your Move " << endl;
			cin		>> PlayerMove;
			DecodeMove(ok[PlayerMove].move); cout	<< (int)ok[PlayerMove].move << endl;
			if (PlayerMove < 0) break;
			INIT	=	MakeMove(INIT, ok[PlayerMove]);
			if (ok[PlayerMove].move == DrawCondition) {cout	<< "DRAW\n"; 	break;}
			getBoardInfo(INIT);
			TotalMove++;
			cout	<< "Move n0 " << TotalMove << endl;		
			cout	<< "FEN STRING = " << toFEN(INIT) << endl;
		}
	}
	LOCK	=	true;
	cout	<< "Total Time = " 		<< totalTime << endl;
	cout	<< "Total Move = "		<< TotalMove << endl;
	averageTime	=	(double)totalTime/(double)TotalMove;
	cout	<< "AverageFail = " << AverageFail/(double)(MaxMove*2) << endl;
	cout	<< "Average Time = " 	<< averageTime << endl;
	cout	<< "Evaluate Final Board : " << EvaluateBOARD(INIT, INIT.Side_to_move) << endl;
	pthread_exit(NULL);
}
