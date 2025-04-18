#include "engine.h"

//timeout a search
void Engine::Timer(int millseconds) {
	std::this_thread::sleep_for(std::chrono::seconds(millseconds));
}

Engine::Engine(){
	InitEngine();
}

Engine::~Engine(){
	//file--socket future planning
}

void Engine::InitEngine(){
	INITIALIZE::Mask();
	INITIALIZE::MoveData();
    InitZoBrist(true);
	initialBoard.ReadFENString(STANDARD);

	alphaBeta.SetTableSize(30000000);
	alphaBeta.SetPosition(initialBoard);
}

void Engine::Start(void *GameArg){
	std::string input;
	while (std::getline(std::cin, input)) {
		if (input == "uci") {
			// Respond to UCI initialization
		} else if (input == "isready") {
		} else if (input.substr(0, 8) == "position") {
			// Handle "position" command
		} else if (input.substr(0, 2) == "go") {
			// Handle "go" command
		} else if (input == "quit") {
			// Exit the program
			break;
		} else {
			// Handle other commands (optional)
			std::cerr << "Unknown command: " << input << "\n";
		}
	}	
}

void Engine::SetPosition(std::string position){
	initialBoard.ReadFENString(position);
}








