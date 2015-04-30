#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#include <string.h>
#include "client_core.h"
#include "defines.h"


int client(const char address[], int port, const char dirName[]) {
	FILE* log = fopen("log_client.txt", "w");
    int sockfd, portno, n;
    struct sockaddr_in serv_addr;
    struct hostent *server;

    char buffer[2048];

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

    strcpy(buffer, "test.txt");

    int temp = write_to_host(address, port, buffer, strlen(buffer), log);

    printf("Please enter the message: ");
    bzero(buffer,2048);
    fgets(buffer,2047,stdin);

    int temp = write_to_host(address, port, buffer, strlen(buffer), log);

    if (temp != 0) {
		fclose(log);
    	return temp;
    }

    bzero(buffer,BUFSIZE);
    n = read(sockfd,buffer,BUFSIZE);
    
    if (n < 0) {
    	fprintf(log, "ERROR reading from socket");
    	fclose(log);
    	return 5;
    }
    
    printf("%s\n",buffer);
    close(sockfd);


    fclose(log);
    return 0;

}

int write_to_host(const int sockfd, int port, void*buf, size_t size, FILE* log) {

	int i = 0, j = 0, n = 0;

    char buffer[BUFSIZE];


	for (i = 0; i < size; i += BUFSIZE - 1) {
		if (i + BUFSIZE - 1 <= size) {
		    for (j = 0; j < BUFSIZE - 1; j++) {
		    	buffer[j] = buf[j];
		    }
	    	n = write(sockfd,buffer,BUFSIZE);
		    if (n < 0) {
		    	fprintf(log, "ERROR writing to socket");
		    	return 4;
		    }
	    } else {
		    for (j = 0; j < size - i; j++) {
		    	buffer[j] = buf[j];
		    }
	    	n = write(sockfd,buffer,size - i);
		    if (n < 0) {
		    	fprintf(log, "ERROR writing to socket");
		    	return 4;
		    }
	    }

	    while (1) {
		    bzero(buffer,BUFSIZE);
		    n = read(sockfd,buffer,BUFSIZE);
		    
		    if (n < 0) {
		    	fprintf(log, "ERROR reading from socket");
		    	fclose(log);
		    	return 5;
		    }

		    // Wait for server to get message
		    if (!strcmp(buffer), "GOT_IT") {
		    	break;
		    }
		}
    }

    return 0;
}