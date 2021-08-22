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
#include <netinet/in.h>
#include <netdb.h>

#define PORT 3535

int main() {
 
  int clientfd;
  struct sockaddr_in client;
  struct hostent *he;
  clientfd = socket(AF_INET, SOCK_STREAM, 0);
  if (clientfd < 0) {
    printf("error creating client socket\n");
    exit(-1);
  }
  client.sin_family = AF_INET;
  client.sin_port = htons(PORT);
  inet_aton("127.0.0.1", &client.sin_addr);
  int r = connect(clientfd, (struct sockaddr *) &client, (socklen_t) sizeof(struct sockaddr));
  if (r < 0) {
    printf("error connecting to the server");
    exit(-1);
  }
  char *pipeb = "/tmp/pipeb";
  int pr;
  int response[2]; // respuesta de confirmación de recibido
  int KB = 1024;
  int sizes[] = { KB, 10*KB, 100*KB, KB*KB, 10*KB*KB, 100*KB*KB };
  for (int s = 0; s < 6; s++) {
    	int bytes = sizes[s];
	char *data = (char *) malloc(bytes * sizeof(char));
	for (int b = 0; b < bytes; b++)
	    data[b] = 'a';
	struct timespec begin, end;
        int reps = (bytes <= KB*KB) ? 20 : 8;
	double total_time;
	long int secs, nsecs;
	pw = open(pipea, O_WRONLY);
	clock_gettime(CLOCK_REALTIME, &begin);
	for (int i = 0; i < reps; i++) {	
	    r = send(clientfd, &data, bytes, 0); // envío de los datos
	}
  r = recv(clientfd, response, 2, 0); // confirmación
	clock_gettime(CLOCK_REALTIME, &end);
	secs = end.tv_sec - begin.tv_sec;
	nsecs = end.tv_nsec - begin.tv_nsec;
	total_time = secs + nsecs*(1e-9);
	double avg = total_time / reps;
	if (bytes < KB*KB)
	    printf("El tiempo promedio para transferir %3dKB es: %.10f segundos\n", bytes/KB, avg);
	else
	    printf("El tiempo promedio para transferir %3dMB es: %.10f segundos\n", bytes/(KB*KB), avg);
	free(data);			
    }
    close(clientfd);
    return 0;
}
