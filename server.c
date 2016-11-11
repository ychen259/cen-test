#include <stdlib.h>     /* NULL */
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

/* Step through protocol with clients. */
static void protocol(void *args)
{
	/* Note, we should listen in a seperate thread. */
	struct game g = make_game();
	int *hostfd = (int *)args;
	unsigned char buf[6 * TILE_COUNT];
	for (size_t i = 0; i < TILE_COUNT; ++i) {
		buf[i * 6 + 0] = g.tile_deck[i].edges[0];
		buf[i * 6 + 1] = g.tile_deck[i].edges[1];
		buf[i * 6 + 2] = g.tile_deck[i].edges[2];
		buf[i * 6 + 3] = g.tile_deck[i].edges[3];
		buf[i * 6 + 4] = g.tile_deck[i].edges[4];
		buf[i * 6 + 5] = g.tile_deck[i].attribute;
	}
	listen(*hostfd, 10);
	printf("Listening.\n");

	int players[PLAYER_COUNT] = {0};
	for (int i = 0; i < PLAYER_COUNT; ++i) {
		players[i] = accept(*hostfd, NULL, NULL);
		write(players[i], buf, TILE_SZ * TILE_COUNT);

		struct timeval tm;
		memset(&tm, 0, sizeof(tm));
		tm.tv_sec = 5; /* 5 seconds per move timer. */
		setsockopt(players[i], SOL_SOCKET, SO_RCVTIMEO,
				(char *)&tm, sizeof(tm));
	}
	printf("Connected both players!\n");

	/* TODO: Randomly pick player to go first. */
	int player = 0; /* Player 0 goes first. */
	/* Send both players the tile set. */
	// prev_move = NULL
	while (1) { /* Play game. */
		// piece = draw_tile(game);
		// write(players[player], (piece, prev_move))
		// move = read(players[player], sizeof(move));
		// if (read(players[player],&move,sizeof(move))<sizeof(move)) {
		// 	/* player loses, player + 1 wins. */
		// 	break;
		// } else if (invalid_move(move)) {
		// 	/* player loses, player + 1 wins. */
		// 	break;
		// }
		// prev_move = move
		player = (player + 1) % 2; /* Switch */
		break;
	}
	for (int i = 0; i < PLAYER_COUNT; ++i) {
		close(players[i]);
	}
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
		pthread_detach(*child); /* OS will free() for us. */
		int *hostfd = malloc(sizeof(int)); /* Thread will free() */
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
