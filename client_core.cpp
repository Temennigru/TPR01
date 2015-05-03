#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#include <string.h>
#include "client_core.h"
#include "defines.h"

int write_to_host(const int sockfd, char*buf, size_t size, FILE* log);

int client(const char address[], int port, const char dirName[]) {
	FILE* log = fopen("log_client.txt", "w");
    int sockfd;
    struct sockaddr_in serv_addr;
    struct hostent *server;

    char buffer[100000];

    sockfd = socket(AF_INET, SOCK_STREAM, 0);



    if (sockfd < 0) {
    	fprintf(log, "ERROR opening socket");
    	fclose(log);
    	return 1;
    }
    
    server = gethostbyname(address);
    
    if (server == NULL) {
        fprintf(stderr, "ERROR, no such host\n");
    	fclose(log);
        return 2;
    }
    
    bzero((char *) &serv_addr, sizeof(serv_addr));

    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);
    serv_addr.sin_port = htons(port);

    if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) {
    	fprintf(stderr, "ERROR connecting");
    	fclose(log);
    	return 3;
    }


    // aaaaaa

    strcpy((char*)buffer, "test.txt");

    int temp = write_to_host(sockfd, buffer, strlen(buffer), log);

    if (temp != 0) {
		fclose(log);
    	return temp;
    }

    printf("Please enter the message length: ");
    bzero(buffer,100000);
    fgets(buffer,99999,stdin);
    int msglen = atoi(buffer);
    int i = 0;

    for (i = 0; i < msglen; i++) {
        buffer[i] = 'a';
    }

    buffer[i] = '\0';

    struct timeval tv_before, tv_after;

    gettimeofday(&tv_before, NULL);

    temp = write_to_host(sockfd, buffer, strlen(buffer), log);

    gettimeofday(&tv_after, NULL);

    time_t seconds = tv_after.tv_sec - tv_before.tv_sec;
    time_t useconds = tv_after.tv_usec - tv_before.tv_usec;

    printf("Buffer size: %d | Message size: %d | Time: %ld.%06ld\n", BUFSIZE, msglen, seconds, useconds);

    if (temp != 0) {
		fclose(log);
    	return temp;
    }

    close(sockfd);

    fclose(log);
    return 0;

}

int write_to_host(const int sockfd, char*buf, size_t size, FILE* log) {

	int i = 0, timeout = 0, n = 0;

    char buffer[BUFSIZE];


	for (i = 0; (unsigned)i < size; i += BUFSIZE - 1) {
		if ((unsigned)(i + BUFSIZE - 1) <= size) {
	    	n = send(sockfd,&(buf[i]),BUFSIZE, 0);
		    if (n < 0) {
		    	fprintf(log, "ERROR writing to socket");
		    	return 4;
		    }
	    } else {
	    	n = send(sockfd,&(buf[i]),size - i, 0);
		    if (n < 0) {
		    	fprintf(log, "ERROR writing to socket");
		    	return 4;
		    }
	    }

        timeout = 0;
	    while (1) {
		    bzero(buffer,BUFSIZE);
		    n = recv(sockfd,buffer,BUFSIZE-1, 0);
		    
		    if (n < 0) {
		    	fprintf(log, "ERROR reading from socket");
		    	fclose(log);
		    	return 5;
		    }

            // Wait for server to get message
		    if (!strcmp(buffer, "GOT_IT")) {
		    	break;
		    }
            // Timeout
            if (timeout == 1000) {
                fprintf(log, "ERROR: Connection timeout. Closing connection.\n");
                strcpy((char*)buffer, "BYE");
                n = send(sockfd,buffer,BUFSIZE, 0);
                return 6;
            }
            timeout++;
		}
    }

    return 0;
}