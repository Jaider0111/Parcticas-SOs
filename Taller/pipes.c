#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <time.h>
#include <math.h>
#include <unistd.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>

#define READ 0
#define WRITE 1

int main() {
  
  int fda[2];
  int fdb[2];
  pipe(fda);
  pipe(fdb);
  char response[2];
  int KB = 1024;
  int sizes[] = { KB, 10*KB, 100*KB, KB*KB, 10*KB*KB, 100*KB*KB };
  
  if (fork() > 0) {
    
    close(fda[READ]);
    close(fdb[WRITE]);
    for (int s = 0; s < 6; s++) {
    	int bytes = sizes[s];
    char *data = (char *) malloc(bytes * sizeof(char));
    for (int b = 0; b < bytes; b++)
        data[b] = 'a';
    struct timespec begin, end;
    int reps = (bytes <= KB*KB) ? 20 : 8;
    double total_time;
    long int secs, nsecs;
    for (int i = 0; i < reps; i++) {
      clock_gettime(CLOCK_REALTIME, &begin);
      int wrt = 0;
      int sz = bytes * sizeof(char);
      while (wrt < sz) {
      	int r = write(fda[WRITE], data, bytes);
      	if (r > 0)
            wrt += r;
      	else {
            if (r == 0)
                printf("socket closed\n");
	    else if (r == -1)
	        printf("error writing to socket\n");
        }
      }	      
      read(fdb[READ], response, 2);
      clock_gettime(CLOCK_REALTIME, &end);
      secs = end.tv_sec - begin.tv_sec;
      nsecs = end.tv_nsec - begin.tv_nsec;
      total_time += secs + nsecs*(1e-9);
    }
    double avg = total_time / reps;
    if (bytes < KB*KB)
        printf("El tiempo promedio para transferir %3dKB es: %.10f segundos\n", bytes/KB, avg);
    else
        printf("El tiempo promedio para transferir %3dMB es: %.10f segundos\n", bytes/(KB*KB), avg);
    free(data);		
    }
    close(fda[WRITE]);
    close(fdb[READ]);
  } else {
      close(fda[WRITE]);
      close(fdb[READ]);
      for (int s = 0; s < 6; s++) {
        int bytes = sizes[s];
        char *data = (char *) malloc(bytes * sizeof(char));
        int reps = (bytes <= KB*KB) ? 20 : 8;
	for (int i = 0; i < reps; i++) {
	   int rd = 0;
	   int sz = bytes * sizeof(char);
	   while (rd < sz) {
	       int r = read(fda[READ], data, bytes);
	       if (r > 0)
	           rd += r;
	       else {
	           if (r == 0)
	               printf("socket closed\n");
		   else if (r == -1)
		       printf("error reading from socket\n");
	       }
	   }
	   write(fdb[WRITE], "r", 1);
	}   
        free(data);
      }
      close(fda[READ]);
      close(fdb[WRITE]);
  }
  return 0;
}
