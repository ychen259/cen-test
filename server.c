#include <stdlib.h>     /* NULL, malloc(), free() */
#include <stdint.h>	/* uint32_t */
#include <string.h>     /* memset() */
#include <unistd.h>     /* write() */
#include <pthread.h>	/* pthread */

#include <arpa/inet.h>	/* htons */
#include <sys/socket.h> /* bind(), listen(), setsockopt() */
#include <netinet/in.h> /* struct sockaddr_in, struct sockaddr */

#include <stdio.h>	/* DEBUG */
#include <errno.h>	/* errno */
#include "limits.h"	/* AXIS, TILE_SZ */
#include "game.h"	/* Server needs to validate moves. */

struct sockaddr_in init_sockaddr(int port)
{
	struct sockaddr_in s;
	memset(&s, '0', sizeof(s));
	s.sin_family = AF_INET;
	s.sin_addr.s_addr = htonl(INADDR_ANY);
	s.sin_port = htons(port);
	return s;
}

/* TODO: Send client hosts with sockets so that 3rd parties can't jump in */
#define PLAYER_COUNT 2

static uint8_t *serialize_tile(struct tile t, uint8_t *buf)
{
	for (size_t i = 0; i < 5; ++i) {
		buf[i] = t.edges[i];
	}
	buf[6] = t.attribute;
	return &buf[7];
}

static struct tile deserialize_tile(uint8_t *buf)
{
	enum edge edges[5];
	for (size_t i = 0; i < 5; ++i) {
		edges[i] = buf[i];
	}
	enum attribute a = buf[6];
	return make_tile(edges, a);
}

static uint8_t *serialize_move(struct move m, uint8_t *buf)
{
	buf = serialize_tile(m.tile, buf);
	uint8_t x = m.slot.x, y = m.slot.y;
	uint8_t rotation = m.rotation;
	*buf++ = x;
	*buf++ = y;
	*buf++ = rotation;
	return buf;
}

static struct move deserialize_move(uint8_t *buf)
{
	struct tile t = deserialize_tile(buf);
	return make_move(t, make_slot(buf[7], buf[8]), buf[9]);
}

static void print_buffer(uint8_t *buf, size_t len)
{
	for (size_t i = 0; i < len; ++i) {
		printf("%d ", buf[i]);
	}
	printf("\n");
}

static int send_deck(int *players, size_t pcnt, struct tile *deck, size_t dlen)
{
	unsigned char buf[TILE_SZ];
	for (size_t i = 0; i < dlen; ++i) {
		serialize_tile(deck[i], buf);
		for (size_t j = 0; j < pcnt; ++j) {
			write(players[j], buf, TILE_SZ); // TODO: Error handling
		}
	}
	return 0;
}

static int send_clock_and_order(int *players, int first, uint64_t seconds)
{
	struct timeval tm;
	memset(&tm, 0, sizeof(tm));
	tm.tv_sec = (time_t) seconds;

	unsigned char buf[1 + sizeof(seconds)]; // First? + seconds.
	for (size_t i = 0; i < sizeof(seconds); ++i) { /* Serialize seconds */
		buf[i + 1] = (uint8_t) (seconds << (8 * i));
	}

	for (size_t i = 0; i < 2; ++i) {
		if (i == (size_t) first) {
			buf[0] = 1; /* First */
		} else {
			buf[0] = 0; /* Not first */
		}
		printf("Sending clock to player %zu: ", i);
		print_buffer(buf, sizeof(buf));
		write(players[i], buf, sizeof(buf));
		setsockopt(players[i], SOL_SOCKET, SO_RCVTIMEO,
				(char *)&tm, sizeof(tm));
	}

	return 0;
}

enum reason {
	SCORE = 0,
	TIMEOUT = 1,
	INVALID = 2
};

static void game_over(int *players, int winner, enum reason r)
{
	unsigned char buf[16];
	buf[0] = 1; /* Game over */
	buf[2] = (uint8_t) r;

	for (size_t i = 0; i < 2; ++i) {
		if (i == (size_t) winner) {
			buf[1] = 1;
			printf("Player %zu has won\n", i);
			printf("DEBUG: Winner | Player: %zu buf: ", i);
		} else {
			buf[1] = 0;
			printf("Player %zu has lost\n", i);
			printf("DEBUG: Loser | Player: %zu buf: ", i);
		}
		print_buffer(buf, sizeof(buf));
		assert(write(players[i], buf, sizeof(buf)) == sizeof(buf));
	}
	return;
}

/* Step through protocol with clients. */
static void protocol(void *args)
{
	int *hostfd = (int *)args;
	struct game *g = malloc(sizeof(*g));
	make_game(g);
	listen(*hostfd, 10);
	printf("Listening.\n");

	int current_player = 0; /* TODO: Randomly pick player to go first. */
	int players[PLAYER_COUNT] = {0};
	for (size_t i = 0; i < PLAYER_COUNT; ++i) {
		players[i] = accept(*hostfd, NULL, NULL);
	}
	printf("Connected both players!\n");

	send_clock_and_order(players, current_player, 5);
	printf("Sent clock and order.\n");

	if (send_deck(players, PLAYER_COUNT, g->tile_deck, TILE_COUNT)) {
		printf("Failed to send deck.\n");
	}

	unsigned char buf[16];
	struct move previous;
	memset(&previous, 0, sizeof(previous));
	while (1) { /* Play game. */
		buf[0] = 0; /* Assume we keep playing. */
		if (!more_tiles(g)) {
			/* TODO: Scoring */
			game_over(players, 0, SCORE);
			break;
		}
		struct tile t = deal_tile(g);
		serialize_move(previous, serialize_tile(t, &buf[1]));
		printf("Sending: ");
		print_buffer(buf, sizeof(buf));
		write(players[current_player], buf, sizeof(buf));
		ssize_t bytes = read(players[current_player], buf, sizeof(buf));
		if (bytes < (ssize_t)sizeof(buf)) {
			game_over(players, current_player^1, TIMEOUT);
			break;
		}
		struct move m = deserialize_move(buf);
		if (!tile_eq(m.tile, t)) {
			game_over(players, current_player^1, INVALID);
			break;
		}
#if 0
		if (!tile_eq(m.tile, t) || play_move(g, m, current_player)) {
			game_over(players, current_player^1, INVALID);
			break;
		}
#endif
		previous = m;
		current_player ^= 1; /* Switch */
	}
	for (int i = 0; i < PLAYER_COUNT; ++i) {
		close(players[i]);
	}
	free(g);
	free(hostfd);
        return;
}

/* Returns the port a socket is listen()ing on in NETWORK ORDER */
static int get_socket_port(int socket)
{
	struct sockaddr_in info;
	memset(&info, 0, sizeof(info));
	socklen_t len = sizeof(info);
	getsockname(socket, (struct sockaddr *)&info, &len);
	return info.sin_port; /* Already in network order. */
}

#define LISTEN_PORT 5000 /* Arbitrarily chosen server port. */
int main(void)
{
        struct sockaddr_in serv_addr = init_sockaddr(LISTEN_PORT);

        int listenfd = socket(AF_INET, SOCK_STREAM, 0);
        bind(listenfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
        listen(listenfd, 10);

	int players[PLAYER_COUNT];
	int queued_players = 0;

        while (1) {
		/* TODO Ensure unique clients (can't play with self) */
		players[queued_players++] = accept(listenfd, NULL, NULL);
		if (queued_players < PLAYER_COUNT) { /* Wait for match */
			printf("Waiting for match.\n");
			continue;
		}

		/* Found a match. Create a child thread to host game. */
		/* TODO: Lots of error handling. */
		pthread_t *child = malloc(sizeof(pthread_t));
		pthread_detach(*child);			/* OS will free() */
		int *hostfd = malloc(sizeof(int));	/* Thread will free() */
		*hostfd = socket(AF_INET, SOCK_STREAM, 0);
		struct sockaddr_in host_addr = init_sockaddr(0); // random port
		bind(*hostfd, (struct sockaddr *)&host_addr, sizeof(host_addr));
		pthread_create(child, NULL, &protocol, hostfd);
		
		/* Tell the clients where to go. */
		uint16_t port = get_socket_port(*hostfd);
		printf("Sending port: %u\n", ntohs(port));
		for (int i = 0; i < PLAYER_COUNT; ++i) {
			write(players[i], &port, sizeof(port));
			close(players[i]); /* No longer need to talk to them. */
		}
		queued_players = 0;
        }
	close(listenfd);

        return 0;
}
