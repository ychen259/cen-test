#ifndef TILE_H_
#define TILE_H_

#include <stdio.h>
#include <stdlib.h>	/* malloc() */
#include <string.h>	/* memcpy() */
#include "edge.h"	/* edges. */

struct tile {
	enum edge edges[5]; /* Top, Right, Bottom, Left, Center. */
};

struct tile create_tile(const enum edge edges[5]);
struct tile rotate_tile(const struct tile old, const int rotation);
void print_tile(struct tile t, char *b);

#endif
