#ifndef MOVE_H_
#define MOVE_H_

#include "tile.h"
#include "slot.h"

struct move {
	struct tile tile; /** Unique tile descriptor */
	struct slot slot; /** Where tile is being placed */
	int rotation; /** Rotation to apply to tile */
};

enum move_validation_result {
	OK = 0,
	E_NOT_PLACEABLE = 1,
	E_EDGE_CONFLICT = 2
};

struct move make_move(struct tile t, struct slot s, int rotation);

#endif
