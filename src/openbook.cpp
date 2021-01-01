#include "openbook.h"

Book::Book(){
    OpeningMoves = HASH_TABLE<Opening, Key>(100000);
}

void Book::Init(){
    std::ifstream BookText("Book.txt");
    std::string Line;
    std::string FEN_ANS;
    std::string MoveStr;
	Opening Entry;
	Move   Deft;
	int afk = 0;
    while (std::getline(BookText, Line)){
		BOARD A;
		Key	Hashing;
		if (Line[0]	!= '(' && Line[0] != '{' && Line[0] != '\0'){
			afk++;
			uint16_t iter 	= 0;
			FEN_ANS		= "";
			MoveStr		= "";
			while (iter < Line.length()){
				FEN_ANS	+=	Line[iter];
				if (Line[++iter]	==	':') {
					++iter;
					break;
				}
			}
			FEN_Op::READ_FEN(FEN_ANS, &A);
			Hashing	=	GetKey(A);
			while (iter < Line.length()){
				MoveStr	+=	Line[iter];
				if (Line[++iter]	==	':') {
					++iter;
					break;
				}
			}
			//cout	<< "MoveStr = " << MoveStr << endl;
			Deft	=	stoi(MoveStr);
			Entry.HashValue	=	Hashing;	
			Entry.StoreMove	=	Deft;
            OpeningMoves.addEntry(Hashing, Entry);
		}
	}
}

bool Book::FindOpening	(Key Position, Move *PMove){
	Opening Result;
    if (OpeningMoves.FindEntry(Position, &Result)){
		*PMove	=	Result.PossibleMove[rand() % (Result.Total + 1)];
		return true;
	}
	return false;
}
