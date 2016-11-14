#include <stdlib.h>     /* NULL, malloc(), free() */
#include <stdint.h>	/* uint32_t */
#include <string.h>     /* memset() */
#include <unistd.h>     /* write() */
#include <pthread.h>	/* pthread */

#include <arpa/inet.h>	/* htons */
#include <sys/socket.h> /* bind(), listen(), setsockopt() */
#include <netinet/in.h> /* struct sockaddr_in, struct sockaddr */

#include <stdio.h>	/* DEBUG */
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

static int send_deck(int *players, size_t pcnt, struct tile *deck, size_t dlen)
{
	char buf[TILE_SZ];
	for (size_t i = 0; i < dlen; ++i) {
		buf[0] = deck[i].edges[0];
		buf[1] = deck[i].edges[1];
		buf[2] = deck[i].edges[2];
		buf[3] = deck[i].edges[3];
		buf[4] = deck[i].edges[4];
		buf[5] = deck[i].attribute;
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
	tm.tv_sec = seconds;

	unsigned char buf[1 + sizeof(seconds)]; // First? + seconds.
	for (size_t i = 0; i < sizeof(seconds); ++i) {
		buf[i + 1] = (uint8_t) (seconds << (8 * i));
	}

	buf[0] = 1;
	write(players[first], buf, sizeof(buf));
	setsockopt(players[0],SOL_SOCKET,SO_RCVTIMEO,(char *)&tm,sizeof(tm));
	buf[0] = 0;
	write(players[first ^ 1], buf, sizeof(buf));
	setsockopt(players[1],SOL_SOCKET,SO_RCVTIMEO,(char *)&tm,sizeof(tm));
	return 0;
}

static uint8_t *serialize_tile(struct tile t, uint8_t *buf)
{
	for (size_t i = 0; i < 5; ++i) {
		buf[i] = t.edges[i];
	}
	buf[6] = t.attribute;
	return &buf[7];
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

/* Step through protocol with clients. */
static void protocol(void *args)
{
	/* Note, we should listen in a seperate thread. */
	int *hostfd = (int *)args;
	struct game *g = malloc(sizeof(*g));
	make_game(g);
	listen(*hostfd, 10);
	printf("Listening.\n");

	int current_player = 0; /* TODO: Randomly pick player to go first. */
	int players[PLAYER_COUNT] = {0};
	for (size_t i = 0; i < sizeof(players); ++i) {
		players[i] = accept(*hostfd, NULL, NULL);
	}

	send_clock_and_order(players, current_player, 5);
	printf("Connected both players!\n");

	if (send_deck(players, sizeof(players), g->tile_deck, TILE_COUNT)) {
		printf("Failed to send deck.\n");
	}

	unsigned char buf[100];
	struct move previous;
	while (1) { /* Play game. */
		struct tile t = deal_tile(g);
		serialize_move(previous, serialize_tile(t, buf));
		write(players[current_player], buf, sizeof(buf));
		// move = read(players[player], sizeof(move));
		// if (read(players[player],&move,sizeof(move))<sizeof(move)) {
		// 	/* player loses, player + 1 wins. */
		// 	break;
		// } else if (invalid_move(move)) {
		// 	/* player loses, player + 1 wins. */
		// 	break;
		// }
		// prev_move = move
		current_player ^= 1; /* Switch */
		break;
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
