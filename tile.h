#ifndef TILE_H_
#define TILE_H_

#include <stdio.h>
#include <string.h>	/* memcpy */
#include "edge.h"	/* edges. */

struct tile {
	enum edge edges[5]; /* Top, Right, Bottom, Left, Center. */
};

struct tile create_tile(enum edge edges[5]);
struct tile rotate_tile(struct tile old, int rotation);
void print_tile(struct tile t);

#endif
