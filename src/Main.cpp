#include <stdio.h>
#include <iostream>
#include "Thread.h"

int main(){
	Init_engine();
	pthread_t	Game;
	int	Arg[4];
	srand(time(0));
	cout << "enter level : ";		cin >> Arg[0];
	cout << "enter maxMove : ";		cin >> Arg[1]; 
	cout << "enter game mode : ";	cin >> Arg[2];
	if (Arg[2] == 0) {
		cout	<< "BLACK or WHITE? BLACK = " << WHITE << "  WHITE = " << BLACK << endl; 
		cin		>> Arg[3];
	}
	pthread_create(&Game, NULL, StartGame, (void *)&Arg);
	//pthread_create(&Noti, NULL, Clock, NULL);
	pthread_exit(NULL);
}
