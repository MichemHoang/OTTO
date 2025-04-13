#ifndef TRANSPOSITION_H_
#define TRANSPOSITION_H_

#include <unordered_map>

#include "typedefs.h"
#include "bitboard.h"


/*3 kinds of Node_Type
 * exact evaluation	( Exact Score )
 * Beta Node		( Beta Score )
 * Alpha Node		( Alpha Score )	
 * Flag is used to recognize old node
 */


#define	EXACT 		1
#define LOWERBOUND 	2
#define UPPERBOUND	3


struct HashEntry{
	bool		Flag;			//Old node
	uint64_t	HashValue;		
	int			Evaluation;		
	uint8_t		Depth;			
	Move		BestMove;		
	uint8_t		Node_Type;	
};

template <typename T, typename HKey> 
class HASH_TABLE{
private:
	int Size;
	std::unordered_map<int, T> HashTable;
public:
	HASH_TABLE();
 	HASH_TABLE(int A);
 	void echo();
 	void addEntry(HKey ZobristHash, T newEntry);
 	int	HashFunction( HKey Hashkey)	;
 	void SetSize(int AS);
 	int	getSize();
 	bool FindEntry(HKey Hashkey, T* Result)  ;
 	void UpdateTable();	// Updating table after every Move so that it wont be filled with old useless entry;
 	void PrintDatabase();	//just for testing. Aint nobody got time to read 10000000000 entry.
};

///Generic temlate methods implementation
template <typename T, typename HKey> 
HASH_TABLE<T, HKey>::HASH_TABLE() {Size = 4000;};

template <typename T, typename HKey> 
HASH_TABLE<T, HKey>::HASH_TABLE(int A) { Size = A; }

template <typename T, typename HKey> 
void HASH_TABLE<T, HKey>::SetSize (int AS) { Size = AS;	};

template <typename T, typename HKey> 
int	HASH_TABLE<T, HKey>::getSize() {return HashTable.size(); };

template <typename T, typename HKey> 
void HASH_TABLE<T, HKey>::addEntry( HKey ZobristHash, T newEntry ) {}

template <typename T, typename HKey> 
int	HASH_TABLE<T, HKey>::HashFunction( HKey Hashkey) { return Hashkey % Size; };//Did i make a mistake??

//for debuging only, aint no one read 1000+ entries
template <typename T, typename HKey> 
void HASH_TABLE<T, HKey>::PrintDatabase(){	}

///*

template <> //inline
inline void	HASH_TABLE<HashEntry, Key>::UpdateTable(){
	for (auto iter = HashTable.begin(); iter != HashTable.end(); iter++)	{
		iter->second.Flag = true;
	}
}

template <> //inline
inline void	HASH_TABLE<HashEntry, Key>::addEntry(Key ZobristHash, HashEntry newEntry){	
	int	Hashkey = HashFunction(ZobristHash);
	auto search = HashTable.find(Hashkey);
	if (search == HashTable.end()){		//not in the table
		HashTable.insert(std::make_pair(Hashkey, newEntry));
	} else if (search->second.Flag){ 
		search->second = newEntry;
	} else if (search->second.Depth < newEntry.Depth){ 
		search->second = newEntry;
	}
}

template <> //inline
inline bool	HASH_TABLE<HashEntry, Key>::FindEntry( Key Hashkey, HashEntry *Result)  {	
	auto it	= HashTable.find(HashFunction(Hashkey));
	if (it == HashTable.end()) return false;
	else {
		if (it->second.HashValue == Hashkey){
			*Result	= it->second;
			return true;
		}
		else return false;
	}
}
///END---

#endif
