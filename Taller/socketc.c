  
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <time.h>
#include <math.h>
#include <unistd.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <strings.h>


#define PERMISSIONS 0666
#define PORT 3535
#define BACKLOG 1

int main() {

    int serverfd, clientfd;
    int opt = 1;
    struct sockaddr_in server, client;
    socklen_t socklen = sizeof(struct sockaddr_in);
    serverfd = socket(AF_INET, SOCK_STREAM, 0);
    if (serverfd < 0) {
      printf("error creating server socket");
      exit(-1);
    }
    server.sin_family = AF_INET;
    server.sin_port = htons(PORT);
    server.sin_addr.s_addr = INADDR_ANY;
    bzero(&server.sin_zero, 8);
    setsockopt(serverfd, SOL_SOCKET, SO_REUSEADDR, (const char *) &opt, sizeof(int));
    int r = bind(serverfd, (struct sockaddr *) &server, sizeof(struct sockaddr));
    if (r < 0) {
      printf("error while binding socket");
      exit(-1);
    }
    char *pipeb = "/tmp/pipeb"; 
    unlink(pipeb);
    r = mkfifo(pipeb, PERMISSIONS);
    if (r < 0) {
        printf("error creating pipeb\n");
	      exit(-1);
    }
    int pr, pw;
    int KB = 1024;
    int sizes[] = { KB, 10*KB, 100*KB, KB*KB, 10*KB*KB, 100*KB*KB };
    r = listen(serverfd, BACKLOG);
    if (r < 0) {
      printf("error listening");
      exit(-1);
    }
    clientfd = accept(serverfd,(struct sockaddr *) &client, &socklen); 
    if (clientfd < 0) {
      printf("error accepting");
      exit(-1);
    }
    for (int s = 0; s < 6; s++) {
	int bytes = sizes[s];
	char *data = (char *) malloc(bytes * sizeof(char));
	int reps = (bytes <= KB*KB) ? 20 : 8;
	//pr = open(pipea, O_RDONLY);	
	int len;
	int received = 0;
	while ((len = recv(clientfd, data, bytes, 0)) > 0) {
	    received += len;
	}
	printf("Received %d\n", received);
	pw = open(pipeb, O_WRONLY);
	// write(pw, "r", 1);
  send(clientfd, "r", 1, 0);
	close(pw);
        close(pr);
        free(data);
    }
    //unlink(pipea);
    unlink(pipeb);
    return 0;
}
