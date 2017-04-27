#include "Transposition.h"

//Using unordered map to implement hash table

DATABASES::DATABASES		()		{	Database_Size		=	4000000; }
 
DATABASES::DATABASES		(int A)	{	Database_Size		=	A; }

void		DATABASES::addEntry(Key ZobristHash, int eval, uint8_t Depth, uint8_t Node_Type, Move BestMove){
	HashEntry	input;
	int	 	Hashkey	 =	HashFunction(ZobristHash);
	auto	search	 =	HashTable.find(Hashkey);
	if (search	==	HashTable.end())	{		//not in the table
		input.BestMove	 =	BestMove;
		input.HashValue	 =	ZobristHash;
		input.Evaluation =	eval;
		input.Depth		 =	Depth;
		input.Node_Type	 =	Node_Type;
		input.Flag		 =	false;
		HashTable.insert(std::make_pair(Hashkey, input));
	} else if (search->second.Flag){			//in the table but old node
		search->second.BestMove	 	=	BestMove;
		search->second.HashValue	=	ZobristHash;
		search->second.Evaluation 	=	eval;
		search->second.Depth		=	Depth;
		search->second.Node_Type	=	Node_Type;
		search->second.Flag		 	=	false;
		//HashTable.insert(std::make_pair(Hashkey, input));		//*/	
	} else if (search->second.Depth <  Depth){
		search->second.BestMove	 	=	BestMove;
		search->second.HashValue	=	ZobristHash;
		search->second.Evaluation 	=	eval;
		search->second.Depth		=	Depth;
		search->second.Node_Type	=	Node_Type;
		search->second.Flag		 	=	false;
	} else if(search->second.Depth == Depth && true){
		if (Node_Type == EXACT && search->second.Node_Type != EXACT){
		search->second.BestMove	 	=	BestMove;
		search->second.HashValue	=	ZobristHash;
		search->second.Evaluation 	=	eval;
		search->second.Depth		=	Depth;
		search->second.Node_Type	=	Node_Type;
		search->second.Flag		 	=	false;
		}
	} 
};	

int			DATABASES::HashFunction( Key Hashkey)	{	return Hashkey % Database_Size;	};
int			DATABASES::getSize		(	)			{	return HashTable.size();	};
bool		DATABASES::FindEntry	( Key Hashkey, HashEntry *Result)  {
	auto it	=	HashTable.find(HashFunction(Hashkey));
	if (it	==	HashTable.end())	return false;
	else {
		if (it->second.HashValue	==	Hashkey)	{
			*Result	=	it->second;
			return true;
		}
		else return false;
	}
}

void		DATABASES::UpdateTable	( ){
	for (auto iter	=	HashTable.begin(); iter != HashTable.end();  iter++)	{
		iter->second.Flag	=	true;
	}
}
	
void		DATABASES::PrintDatabase(){
	int cout	=	0;
	for (auto iter	=	HashTable.begin(); iter != HashTable.end();  iter++){
		std::cout	<< 	cout++	<< "  :  "	<<	iter->second.HashValue << std::endl;
		std::cout	<<	"Value	=	"		<<	iter->second.Evaluation	<< "\n";
		std::cout	<<	"Depth	=	"		<<	iter->second.Depth	<< "\n\n";
	}
}
