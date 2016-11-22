#include "game.h"

/** Returns a random int between the given <em>low</em> and <em>high</em> bounds, inclusive. */
static size_t rand_bound(size_t low, size_t high)
{
	size_t r;
	do {
		r = round(genrand64_real1());
	} while (r < low || r > high);
	return r;
}

/* Modern Fisher-Yates per Wikipedia.
 * /wiki/Fisher%E2%80%93Yates_shuffle#The_modern_algorithm
*/
static void shuffle_tiles(struct tile *a, size_t top)
{
	struct timespec tp;
	clock_gettime(CLOCK_REALTIME, &tp);
	init_genrand64(tp.tv_nsec); /* Seed the PRNG for the shuffling. */

	for (size_t i = top - 1; i > 1; --i) {
		size_t j = rand_bound(0, i);
		struct tile swap = a[i];
		a[i] = a[j];
		a[j] = swap;
	}
}

/** Creates all tiles, unshuffled. */
static void init_deck(struct tile deck[TILE_COUNT])
{
	/* Tileset: http://russcon.org/RussCon/carcassonne/tiles.html */
	/* TODO: Refactor so that this fits in 80 columns. */
	size_t ind = 0;
	deck[ind++] = /* start tile, must be first. */
		make_tile((enum edge[5]){CITY, ROAD, FIELD, ROAD, ROAD},
		NONE);
	deck[ind++] =
		make_tile((enum edge[5]){CITY, CITY, CITY, CITY, CITY}, SHIELD);
	deck[ind++] =
		make_tile((enum edge[5]){ROAD, ROAD, ROAD, ROAD, ROAD}, NONE);
	for (int i = 0; i < 7; ++i) {
		const enum attribute a[7] =
			{NONE, NONE, NONE, SHIELD, NONE, SHIELD, SHIELD};
		const enum edge b[7] = {FIELD,FIELD,FIELD,FIELD,ROAD,ROAD,ROAD};
		deck[ind++] =
			make_tile((enum edge[5]){CITY, CITY, b[i], CITY, CITY},
			a[i]);
	}
	for (int i = 0; i < 4; ++i) {
		deck[ind++] =
			make_tile((enum edge[5]){FIELD, ROAD, ROAD, ROAD, ROAD},
			NONE);
	}
	for (int i = 0; i < 3; ++i) {
		const enum attribute a[3] = {NONE, SHIELD, SHIELD};
		deck[ind++] =
			make_tile((enum edge[5]){FIELD, CITY, FIELD, CITY,CITY},
			a[i]);
	}
	for (int i = 0; i < 8; ++i) {
		deck[ind++] =
			make_tile((enum edge[5]){ROAD, FIELD, ROAD, FIELD,ROAD},
			NONE);
	}
	for (int i = 0; i < 10; ++i) {
		const enum attribute a[5] = {NONE, NONE, NONE, SHIELD, SHIELD};
		const enum edge b[2] = {ROAD, FIELD};
		deck[ind++] =
			make_tile((enum edge[5]){CITY,b[i%2],b[i%2],CITY,CITY},
			a[i]);
	}
	for (int i = 0; i < 9; ++i) {
		deck[ind++] =
			make_tile((enum edge[5]){FIELD, FIELD, ROAD, ROAD,ROAD},
			NONE);
	}
	for (int i = 0; i < 2; ++i) {
		deck[ind++] =
			make_tile((enum edge[5]){CITY,CITY,FIELD,FIELD,FIELD},
			NONE);
	}
	for (int i = 0; i < 3; ++i) {
		deck[ind++] =
			make_tile((enum edge[5]){FIELD,CITY,FIELD,CITY,FIELD},
			NONE);
	}
	for (int i = 0; i < 2; ++i) {
		deck[ind++] =
			make_tile((enum edge[5]){FIELD,FIELD,ROAD,FIELD,FIELD},
			MONASTERY);
	}
	for (int i = 0; i < 4; ++i) {
		deck[ind++] =
			make_tile((enum edge[5]){FIELD,FIELD,FIELD,FIELD,FIELD},
			MONASTERY);
	}
	for (int i = 0; i < 5; ++i) {
		deck[ind++] =
			make_tile((enum edge[5]){CITY,FIELD,FIELD,FIELD,FIELD},
			NONE);
	}
	for (int i = 0; i < 3; ++i) {
		deck[ind++] =
			make_tile((enum edge[5]){CITY,ROAD,ROAD,FIELD,ROAD},
			NONE);
	}
	for (int i = 0; i < 3; ++i) {
		deck[ind++] =
			make_tile((enum edge[5]){CITY,FIELD,ROAD,ROAD,ROAD},
			NONE);
	}
	for (int i = 0; i < 3; ++i) {
		deck[ind++] =
			make_tile((enum edge[5]){CITY,ROAD,ROAD,ROAD,ROAD},
			NONE);
	}
	for (int i = 0; i < 3; ++i) {
		deck[ind++] =
			make_tile((enum edge[5]){CITY,ROAD,FIELD,ROAD,ROAD},
			NONE);
	}
	assert(ind == TILE_COUNT);
	return;
}

/** Initialises the given game. */
void init_game(struct game *g)
{
	g->graphs_used = g->curr_tile_deck_idx = g->scores[0] = g->scores[1] = 0; /* Ensure these are all initialised to 0. */
	init_deck(g->tile_deck);
	/* Leave index 0 untouched. That index will be the starting tile. */
	shuffle_tiles(&g->tile_deck[1], TILE_COUNT - 1);
	g->board = make_board();
	return;
}

/** Writes the given deck into the given game's tile_deck by deep copy. */
void set_game_deck(struct game *g, struct tile *deck)
{
	memcpy(g->tile_deck, deck, sizeof(*deck) * TILE_COUNT);
}

/** Tries to play the given move by the player on the given game. */
int play_move(struct game *g, struct move m, int player)
{
	return play_move_board(&g->board, m);
	// Graph and score stuff here.
}

/** Returns the next tile from the given game's tile_deck */
struct tile deal_tile(struct game *g)
{
	return g->tile_deck[g->curr_tile_deck_idx++];
}

#ifdef TEST
int main(void)
{
	struct game g;
	init_game(&g);
	char buf[TILE_LEN];
	for (int i = 0; i < TILE_COUNT; ++i) {
		printf("%s\n", print_tile(deal_tile(&g), buf));
	}
	return 0;
}
#endif
