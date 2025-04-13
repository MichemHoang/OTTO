#ifndef ZOBRISTHASH_H
#define ZOBRISTHASH_H

#include "mt64.h"
#include "typedefs.h"
#include "bitboard.h"
#include "board.h"

#include <stdint.h>
#include <stdlib.h>     /* srand, rand */
#include <algorithm>    // std::sort

BitString	RandomGenerator();

BitString	GetKey(BOARD A);

BitString	GetKeyString(BOARD_C board);

void		InitZoBrist(bool On);

BitString	UpdateKey(BitString OldKey, Move A, BOARD S);

BitString	UpdateKey(BitString oldKey, Move newMove, BOARD_C board);

BitString	getHash(BOARD_C board);

#endif
