#include "move.h"

struct move make_move(struct tile t, struct slot s, int rotation)
{
	struct move m = {
		.tile = t,
		.slot = s,
		.rotation = rotation
	};
	return m;
}

