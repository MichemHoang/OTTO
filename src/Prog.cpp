#include <stdio.h>
#include <iostream>
#include <pthread.h>
#include <unistd.h>

#include "BitBoard.h"
#include "MoveGen.h"
#include "evaluation.h"
#include "search.h" 

using namespace std;
BOARD	INIT;
time_t 	now;
int SIGNAL;

void		InitBoard()				{
	READ_FEN(STANDARD, &INIT);
	getBoardInfo(INIT);
}

void Init_engine(){
	cout	<< "MICHEM 1.7 ALPHA ACTIVATED\n";
	GenerateMask();
	InitZoBrist(true);
	GenerateMoveData();
	InitBoard();
	getBoardInfo(INIT);
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

void *StopWatch (void * Time){
	while (!SIGNAL){
		sleep(10);
		cout	<< "Woohoo it has been 10 second\n";
	}
	pthread_exit(NULL);
}

struct HalfMoveInfo{
	pair <Move, int> Result;
	int	Time;
	int	SearchNode;
};

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

void *Start_Game(void * threadArg){
	cout	<< "Starting game\n";
	int	level; int AIvsAI;	int MaxMove;
	int	*PTR;
	PTR			=	(int *) threadArg;
	level		=	PTR[0];
	MaxMove		=	PTR[1];
	AIvsAI		=	PTR[2];
	cout	<< "level = " << level << endl;
	cout	<< "MaxMove = " << MaxMove << endl;
	cout	<< "AIvsAI = " << AIvsAI << endl;
	HalfMoveInfo	RES;
	double 	averageTime = 0;
	int		totalTime	= 0;
	int		TotalMove	= 0;
	int		MaxTime		= 0;
	if (AIvsAI){
		for (int i = 0; i < MaxMove * 2; i++){
			RES	=	AI_Move(INIT, level, &TotalMove, &totalTime, 0);
			if (RES.Result.second > 16666)		{cout	<< "Player 1 win\n"; 	break;}
			if (RES.Result.second < -16666)		{cout	<< "Player 2 win\n";	break;}
			if (RES.Time > MaxTime)	MaxTime	=	RES.Time;
			averageTime	=	(double)totalTime/(double)TotalMove;
			cout	<< "Average Time = " 	<< averageTime << endl;	
			cout	<< "\nMove n0 " << TotalMove << endl;	
			cout	<< "FEN STRING = " << toFEN(INIT) << endl;
		}
	} else {
		for (int i = 0; i < MaxMove; i++){
			BOARD Undo2, Undo1;
			//----------------------Player 1(AI)-----------------------
			RES	=	AI_Move(INIT, level, &TotalMove, &totalTime, 0);
			if (RES.Result.second > 16666)		{cout	<< "Player 1 win\n"; 	break;}
			if (RES.Result.second < -16666)		{cout	<< "Player 2 win\n";	break;}
			if (RES.Time > MaxTime)	MaxTime	=	RES.Time;
			cout	<< "Move n0 " << TotalMove << endl;		
			Undo1	=	INIT;
			averageTime	=	(double)totalTime/(double)TotalMove;
			cout	<< "Average Time = " 	<< averageTime << endl;
			cout	<< "\nMove n0 " << TotalMove << endl;	
			cout	<< "FEN STRING = " << toFEN(INIT) << endl;
			//----------------------Player 2(Human)--------------------
			
			Display_Move (INIT);
			ExtMove ok[265];
			int		PlayerMove;
			GenerateAllMove(INIT, ok, INIT.Side_to_move);
			cout	<< "Your Move " << endl;
			cin		>> PlayerMove;
			INIT	=	MakeMove(INIT, ok[PlayerMove]);
			getBoardInfo(INIT);
			TotalMove++;
			cout	<< "Move n0 " << TotalMove << endl;		
			Undo2	=	Undo1;
			cout	<< "FEN STRING = " << toFEN(INIT) << endl;
		}
	}
	cout	<< "Total Time = " 		<< totalTime << endl;
	cout	<< "Total Move = "		<< TotalMove << endl;
	averageTime	=	(double)totalTime/(double)TotalMove;
	cout	<< "Average Time = " 	<< averageTime << endl;
	cout	<< "Evaluate Final Board : " << EvaluateBOARD(INIT, INIT.Side_to_move) << endl;
	pthread_exit(NULL);
}


int main(){
	Init_engine();
	int N, M, AI = 0;
	pthread_t	Game;
	int	Arg[5];
	cin >> M; cin >> N; cin >> AI;
	Arg[0]	=	M;
	Arg[1]	=	N;
	Arg[2]	=	AI;
	pthread_create(&Game, NULL, Start_Game, (void *)&Arg);
	pthread_exit(NULL);
}
