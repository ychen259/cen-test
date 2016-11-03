#ifndef MOVE_H_
#define MOVE_H_

#include "tile.h"
#include "slot.h"

struct move {
	struct tile tile;
	struct slot slot;
	int rotation;
};

struct move make_move(struct tile t, struct slot s, int rotation);

#endif
