#ifndef ZOBRISTHASH_H
#define ZOBRISTHASH_H

#include "mt64.h"
#include "typedefs.h"
#include "BitBoard.h"
#include <stdint.h>
#include <stdlib.h>     /* srand, rand */
#include <algorithm>    // std::sort
#include "MoveGen.h"

extern BitString	Random[13][64];

BitString	RandomGenerator();

BitString	GetKey(BOARD A);

void		InitZoBrist(bool On);

BitString	UpdateKey(BitString OldKey, Move A, BOARD S);

BitString	getHash(BOARD A);

#endif
