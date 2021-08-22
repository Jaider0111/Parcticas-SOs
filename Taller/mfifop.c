#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <time.h>
#include <math.h>
#include <unistd.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>

int main() {
    char *pipea = "/tmp/pipea";
    char *pipeb = "/tmp/pipeb";
    char *pipec = "/tmp/pipec"; // la agregué al final para ver si así funcionaba pero no
    int pw, pr, psw;
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
	    for (int i = 0; i < reps; i++) {
      		psw = open(pipec, O_WRONLY); // tercera pipe para ver si así funcionaba pero no
      		write(psw, "r", 1);
      		close(psw);
		clock_gettime(CLOCK_REALTIME, &begin);
		write(pw, data, bytes); // envío de los datos
		pr = open(pipeb, O_RDONLY);
      		int r = read(pr, response, 2); // lectura de confirmación de recibido, parece que aquí se queda bloqueado
		close(pr);
		clock_gettime(CLOCK_REALTIME, &end);
		secs = end.tv_sec - begin.tv_sec;
		nsecs = end.tv_nsec - begin.tv_nsec;
		total_time += secs + nsecs*(1e-9);		
	    }
	    close(pw);
	    double avg = total_time / reps;
	    if (bytes < KB*KB)
		printf("El tiempo promedio para transferir %3dKB es: %.10f segundos\n", bytes/KB, avg);
	    else
		printf("El tiempo promedio para transferir %3dMB es: %.10f segundos\n", bytes/(KB*KB), avg);
	    free(data);			
	}
	return 0;
}
