#include <errno.h>	/* errno */
#include <stdio.h>	/* printf() */
#include <stdint.h>	/* uint32_t */
#include <string.h>	/* memset(), strerror() */
#include <unistd.h>	/* read(), write() */

#include <arpa/inet.h>  /* inet_pton() */

#include <sys/socket.h> /* socket() */
#include <sys/uio.h>	/* read(), write() */
#include <sys/types.h>	/* read(), write() */

#include "limits.h"
#include "game.h"

static struct sockaddr_in make_sockaddr_in_port(int port)
{
	struct sockaddr_in s;
	memset(&s, '0', sizeof(s));
	s.sin_family = AF_INET;
	s.sin_port = port;
	return s;
}

static int retry_connection(int sock, struct sockaddr *a, size_t asize, int cnt)
{
	for (int i = 0; i < cnt; ++i) {
		if (!connect(sock, a, asize)) {
			return 0;
		}
		switch (errno) {
		case ECONNREFUSED:
		case EHOSTUNREACH:
		case ENETDOWN:
		case ENETUNREACH:
		case ETIMEDOUT:
		case ECONNRESET:
			break;	/* retry */
		default:	/* can't fix, so bail out */
			return 1;
		}
		sleep(1);
	}
	return 1;
}

static int connect_retry(char *host, int port)
{
	int sockfd;
	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		printf("Could not create socket: %s\n", strerror(errno));
		return -1;
	}

	struct sockaddr_in s = make_sockaddr_in_port(port);
	if (!inet_pton(AF_INET, host, &s.sin_addr)) {
		printf("Bad address.\n");
		return -1;
	} 

	if (retry_connection(sockfd, (struct sockaddr *)&s, sizeof(s), 10)) {
		printf("Connection Failure: %s\n", strerror(errno));
		return -1;
	}

	return sockfd;
}

#define REMOTE_HOST "127.0.0.1" /* TODO: Get a command line variable. */
#define REMOTE_PORT 5000 /* TODO: Factor port into shared header. */

//int main(int argc, char *argv[])
int main(void)
{
	int sockfd;
	if ((sockfd = connect_retry(REMOTE_HOST, htons(REMOTE_PORT))) < 0) {
		printf("Error: %s\n", strerror(errno));
		return 1;
	} 

	uint16_t port;
	if (read(sockfd, &port, sizeof(port)) < 0) {
		printf("Read error: %s\n", strerror(errno));
		return 1;
	}
	printf("Got port: %u\n", ntohs(port));
	close(sockfd);

	if ((sockfd = connect_retry(REMOTE_HOST, port)) < 0) {
		printf("Error: %s\n", strerror(errno));
		return 1;
	}

	printf("Successfully connected.\n");
	unsigned char buf[100];
	struct tile *tileset = malloc(sizeof(*tileset) * TILE_COUNT);
	for (int i = 0; i < TILE_COUNT; ++i) {
		read(sockfd, buf, TILE_SZ);
		enum edge edges[5];
		enum attribute a;
		edges[0] = buf[0];
		edges[1] = buf[1];
		edges[2] = buf[2];
		edges[3] = buf[3];
		edges[4] = buf[4];
		a = buf[5];
		tileset[i] = make_tile(edges, a);
	}
	struct game *g = malloc(sizeof(*g));
	make_game_with_deck(g, tileset);
	for (int i = 0; i < TILE_COUNT; ++i) {
		printf("%s\n", print_tile(deal_tile(g), buf));
	}
	free(tileset);

	close(sockfd);

	free(g);
	return 0;
}
