#include <stdio.h>
#include <iostream>
#include <thread>

#include "engine.h"
#include "board.h"

#include <chrono>
#include <functional>

int main(){
    Engine OTTO;
    int Arg[4];
	srand(time(0));
    std::cout << "enter level : "; std::cin >> Arg[0];
    std::cout << "enter maxMove : "; std::cin >> Arg[1];
    OTTO.InitEngine();
    OTTO.Start(&Arg);
}
