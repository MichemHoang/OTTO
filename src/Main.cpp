#include <stdio.h>
#include <iostream>
#include <thread>
#include "Thread.h"

int main(){
	THREAD A;
	A.Init_engine();
	int	Arg[4];
	srand(time(0));
	cout << "enter level : ";		cin >> Arg[0];
	cout << "enter maxMove : ";		cin >> Arg[1]; 
	cout << "enter game mode : ";	cin >> Arg[2];
	if (Arg[2] == 0) {
		cout	<< "BLACK or WHITE? BLACK = " << WHITE << "  WHITE = " << BLACK << endl; 
		cin		>> Arg[3];
	}
	std::thread SignaL(&THREAD::SignalHandler, &A);
	std::thread Game(&THREAD::StartGame, &A, (void *)&Arg);
	std::thread GameClock(&THREAD::Timer, &A);
	Game.join();
	GameClock.join();
	SignaL.join();
}
