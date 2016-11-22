#include "slot.h"

struct slot make_slot(unsigned int x, unsigned int y)
{
	struct slot s = {
		.x = x,
		.y = y
	};
	return s;
}

/**
 * Returns the relative position of <em>a</em> to <em>b</em>; x position takes precedence over y position.
 * 	If <em>a</em> comes before <em>b</em>, -1 is returned.
 * 	If <em>a</em> is the same position as <em>b</em>, 0 is returned.
 * 	If <em>a</em> comes after <em>b</em>, 1 is returned.
 */
int compare_slot_positions(struct slot a, struct slot b)
{
	if (a.x < b.x) {
		return -1;
	} else if (a.x > b.x) {
		return 1;
	} else if (a.y < b.y) {
		return -1;
	} else if (a.y > b.y) {
		return 1;
	} else {
		return 0;
	}
}
