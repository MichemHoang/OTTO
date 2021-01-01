#include <stdio.h>
#include <iostream>
#include <thread>
#include "game.h"

int main(){
    Game A;
    A.Init_engine();
    int Arg[4];
	srand(time(0));
    std::cout << "enter level : ";		std::cin >> Arg[0];
    std::cout << "enter maxMove : ";		std::cin >> Arg[1];
    std::cout << "enter game mode : ";	std::cin >> Arg[2];
	if (Arg[2] == 0) {
        std::cout	<< "BLACK or WHITE? BLACK = " << WHITE << "  WHITE = " << BLACK << std::endl;
        std::cin		>> Arg[3];
	}
    std::thread SignaL(&Game::SignalHandler, &A);
    std::thread Game(&Game::StartGame, &A, (void *)&Arg);
    std::thread GameClock(&Game::Timer, &A);
	Game.join();
	GameClock.join();
	SignaL.join();
}
