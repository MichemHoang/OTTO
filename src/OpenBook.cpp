#include "OpenBook.h"

HASH_TABLE<Opening, Key> OPEN_BOOK(1000000000);

void INIT_BOOK(){
	ifstream BookText("Book.txt");
	string Line;
	string FEN_ANS;
	string MoveStr;
	Opening Entry;
	Move   Deft;
	int afk = 0;
	while (getline(BookText, Line)){
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
			READ_FEN(FEN_ANS, &A);
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
			OPEN_BOOK.addEntry(Hashing, Entry);
		}
	}
};

bool FindOpening	(Key Position, Move *PMove){
	Opening Result;
	if (OPEN_BOOK.FindEntry(Position, &Result)){
		*PMove	=	Result.PossibleMove[rand() % (Result.Total + 1)];
		return true;
	}
	return false;
}
