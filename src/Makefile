COMPILER	= g++
Flags		= -fPIC -g -c -Wall -std=c++11
objects 	= Thread.o BitBoard.o MoveGen.o evaluation.o search.o ZobristHash.o mt64.o Transposition.o Main.o OpenBook.o
PATH_DIR	= $(PWD)

OTTO :  $(objects)
	${COMPILER} -g -pthread -Wall -o OTTO $(objects)
Main.o : Main.cpp
	${COMPILER}  ${Flags} Main.cpp
search.o : search.cpp search.h
	${COMPILER}  ${Flags} search.cpp
Thread.o	 : Thread.cpp 
	${COMPILER}  ${Flags} Thread.cpp
MoveGen.o	: MoveGen.cpp MoveGen.h
	${COMPILER}  ${Flags} MoveGen.cpp	
BitBoard.o	: BitBoard.cpp BitBoard.h
	${COMPILER}  ${Flags} BitBoard.cpp
evaluation.o: evaluation.cpp evaluation.h 
	${COMPILER}  ${Flags} evaluation.cpp
ZobristHash.o	: ZobristHash.cpp ZobristHash.h
	${COMPILER} ${Flags} ZobristHash.cpp 
mt64.o	: mt64.h mt64.cpp
	${COMPILER} ${Flags} mt64.cpp
Transposition.o	: Transposition.h Transposition.cpp 
	${COMPILER} ${Flags} Transposition.cpp
OpenBook.o	: OpenBook.cpp OpenBook.h 
	${COMPILER} ${Flags} OpenBook.cpp	
clean_objFile :
	rm -f *.o
clean : 
	rm Engine_M
Scream : 
	echo $(PATH_DIR)
help : 
	echo "I havent written this part yet"
