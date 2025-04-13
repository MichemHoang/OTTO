#ifndef OPENBOOK_H_
#define OPENBOOK_H_

#include <stdint.h>
#include "typedefs.h"
#include "bitboard.h"
#include "transposition.h"
#include <iostream>
#include <fstream>
#include "zobristhash.h"

struct Opening{
	uint64_t	HashValue;
	Move		StoreMove;
	Move		PossibleMove[5];
	uint8_t		Total	=	0;
};

template <> //inline
inline void	HASH_TABLE<Opening, Key>::addEntry		( Key ZobristHash, Opening newEntry	)			{
    int	Hashkey	= HashFunction(ZobristHash);
    auto search	= HashTable.find(Hashkey);
    if (search == HashTable.end())	{		//not in the table
        newEntry.PossibleMove[newEntry.Total]	=	newEntry.StoreMove;
        HashTable.insert(std::make_pair(Hashkey, newEntry));
    } else {
        search->second.Total += 1;
        search->second.PossibleMove[search->second.Total]	=	newEntry.StoreMove;
    }
}

template <> //inline
inline bool	HASH_TABLE<Opening, Key>::FindEntry	( Key Hashkey, Opening *Result)  {
    auto it	= HashTable.find(HashFunction(Hashkey));
    if (it == HashTable.end()) return false;
    else {
        if (it->second.HashValue	==	Hashkey)	{
            *Result	=	it->second;
            return true;
        }
        else return false;
    }
}

class Book{
private:
    HASH_TABLE<Opening, Key>OpeningMoves;
public:
    Book();
    void Init();
    bool FindOpening(Key Position, Move *PMove);
};
#endif
