#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 

#ifdef CLIENT
#include "client_core.h"
#define ARGC 4
#endif

#ifdef SERVER
#include "server_core.h"
#define ARGC 2
#endif

int main (int argc, char *argv[]) {
	int res = -1;
	#ifdef CLIENT
	if (argc == 1) { // Test args
		res = client("localhost", 8888, "test");
	} else if (argc < ARGC || argc > ARGC) {
		printf("Error: Wrong number of args\n");
		exit(1);
	} else {
		res = client(argv[1], atoi(argv[2]), argv[3]);
	}
	const char mode[32] = "Client";
	#endif

	#ifdef SERVER
	if (argc == 1) { // Test args
		res = server(8888);
	} else if (argc < ARGC || argc > ARGC) {
		printf("Error: Wrong number of args\n");
		exit(1);
	} else {
		res = server(atoi(argv[1]));
	}
	const char mode[32] = "Server";
	#endif

	// Error checking
	switch(res) {
		case -1: {
			printf("Error: It looks like this executable wasn\'t compiled correctly.\nPlease try recompiling with the makefile provided.");
			return res;
			break;
		}
		case 0: {
			printf("%s exited normally.\n", mode);
			return res;
			break;
		}
		default: {
			printf("%s exited with error code %d.\n", mode, res);
			return res;
			break;
		}
	}
}
