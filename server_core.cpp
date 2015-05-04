#include "server_core.h"
#include <csignal>
#include <csetjmp>
#include <cstdio>
#include <cstdlib>
#include <unistd.h>
#include <cstring>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#include <cstring>
#include "defines.h"

jmp_buf __ex_buf__; // Global variable to define the jump. If it is local it won't work.
bool wasTry = false;

static void signal_catch(int signo) {
	if (!wasTry || signo != SIGINT) {
		printf ("Some error occured\n");
		abort();
	}
	longjmp(__ex_buf__, 1);
}

int server(int port) {
	FILE* log = fopen("log_server.txt", "w");
	int sockfd = 0, newsockfd = 0;
	// Set SIGSEGV return spot
	switch(setjmp(__ex_buf__)) {
		// Case no SIGSEGV
		case 0: {
	signal(SIGINT, signal_catch);
	wasTry = true;
	socklen_t clilen;
	char buffer[BUFSIZE];
	struct sockaddr_in serv_addr, cli_addr;
	int n;


	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	bzero((char *) &serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = htons(port);
	
	if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
		fprintf(log, "ERROR on binding\n");
		fclose(log);
		return 1;
	}

	listen(sockfd,5);
	clilen = sizeof(cli_addr);
	newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
	
	if (newsockfd < 0) {
		fprintf(log, "ERROR on accept\n");
		fclose(log);
		return 2;
	}
	
	bzero(buffer,BUFSIZE);

	while (1) {

		bzero(buffer,BUFSIZE);

		n = recv(newsockfd,buffer,BUFSIZE - 1, 0);

		#if defined(NDEBUG) && defined(__GNUC__)
		#else
		printf("Your message\n%d -> (%s)\n", n, buffer);
		#endif
		
		if (n < 0) {
			fprintf(log, "ERROR reading from socket\n");
			continue;
		}

		// Check if nothing was read

		if (!buffer[0]) { continue; }

		// Read stuff here
		FILE* out = fopen(buffer, "w");
		bzero(buffer,BUFSIZE);
		n = send(newsockfd,"GOT_IT",7, 0);
		int timeout = 0;
		while(1) {
			bzero(buffer,BUFSIZE);

			n = recv(newsockfd,buffer,BUFSIZE - 1, 0);

    		#if defined(NDEBUG) && defined(__GNUC__)
    		#else
			printf("Your message\n%d -> (%s)\n", n, buffer);
			#endif

			// Check if nothing was read
			if (!buffer[0]) {
				timeout++;
            	// Timeout
            	if (timeout == 20) {
                	fprintf(log, "ERROR: Connection timeout. Closing connection.\n");
					close(newsockfd);
					newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
                	return 8;
            	}
            continue;
        	}
        	timeout = 0;

			if (n < 0) {
				fprintf(log, "ERROR reading from socket\n");
				continue;
			}

			if (!strcmp(buffer, "BYE")) { 
				n = send(newsockfd,"GOT_IT",7, 0);
				close(newsockfd);
				close(sockfd);
				fclose(log);
				return 0;
			}

    		#if defined(NDEBUG) && defined(__GNUC__)
    		#else
			printf("Writing to output\n");
			#endif

			fprintf(out, "%s", buffer);

			n = send(newsockfd,"GOT_IT",7, 0);

			if (n < 0) {
				fprintf(log, "ERROR writing to socket\n");
				continue;
			}
		}


	} // End while loop


	break;
	} // End longjmp case 0
	
	// Case SIGSEGV

	case 1: {
		close(newsockfd);
		close(sockfd);
		fclose(log);
		return 0;
		break;
	}

	// Something other than 0 or 1 was returned. This should never happen.

	default: {
		printf ("Some error occured\n");
		fclose(log);
		abort();
	} }// End longjmp switch
	return 7;
}