#include <stdio.h>
#include <iostream>
#include <thread>

#include "game.h"
#include "board.h"

#include <chrono>
#include <functional>

int main(){
    Game newGame;
    newGame.Init_engine();
    int Arg[4];
	srand(time(0));
    std::cout << "enter level : "; std::cin >> Arg[0];
    std::cout << "enter maxMove : "; std::cin >> Arg[1];
    std::cout << "enter game mode : "; std::cin >> Arg[2];
	if (Arg[2] == 0) {
        std::cout	<< "BLACK or WHITE? BLACK = " << WHITE << "  WHITE = " << BLACK << std::endl;
        std::cin    >> Arg[3];
	}
    std::thread SignaL(&Game::SignalHandler, &newGame);
    std::thread Game(&Game::Start, &newGame, (void *)&Arg);
    std::thread GameClock(&Game::Timer, &newGame);
    //should only Ai move be its own thread instead of the entire game being run on a thread?
    //hmm should be more like game->start -> start clock thread and ai move thread simultaneously.
    //
	Game.join();
	GameClock.join();
	SignaL.join();
}
