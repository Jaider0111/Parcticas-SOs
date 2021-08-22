#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <time.h>
#include <math.h>
#include <unistd.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>

#define PERMISSIONS 0666

int main() {
    char *pipea = "/tmp/pipea";	
    unlink(pipea);
    int r = mkfifo(pipea, PERMISSIONS);
    if (r < 0) {
        printf("error creating pipea\n");
        exit(-1);
    }
    char *pipeb = "/tmp/pipeb"; 
    unlink(pipeb);
    r = mkfifo(pipeb, PERMISSIONS);
    if (r < 0) {
        printf("error creating pipeb\n");
	exit(-1);
    }
    char *pipec = "/tmp/pipec"; // tercera pipe agregada para ver si así funcionaba pero no
    unlink(pipec);
    r = mkfifo(pipec, PERMISSIONS);
    if (r < 0) {
        printf("error creating pipec\n");
	exit(-1);
    }
    int pr, pw, psr;
    char signal[2]; // aquí se guarda el dato recibido por la tercera pipe, actúa como una señal, para que esté proceso vaya en la misma iteración que el productor, pero no funciona
    int KB = 1024;
    int sizes[] = { KB, 10*KB, 100*KB, KB*KB, 10*KB*KB, 100*KB*KB };
    for (int s = 0; s < 6; s++) {
	int bytes = sizes[s];
	char *data = (char *) malloc(bytes * sizeof(char));
	int reps = (bytes <= KB*KB) ? 20 : 8;
	pr = open(pipea, O_RDONLY);	
	for (int i = 0; i < reps; i++) {
	    psr = open(pipec, O_RDONLY);
	    int r = read(psr, signal, 2);
	    close(psr);
	    int len;
	    int received = 0;
	    while ((len = read(pr, data, bytes)) > 0) { // todos los datos de las 20 u 8 reps se reciben en la primera iteración i de este ciclo, por lo que podía removerse, pero tras añadir la segunda pipe dejó de servir
	        received += len;
	    }
	    printf("Received %d\n", received); // esto funcionaba antes de añadir la segunda pipe para enviar la confirmación
	    pw = open(pipeb, O_WRONLY);
	    write(pw, "r", 1);
	    close(pw);
	}
	close(pr);
	free(data);
    }
    unlink(pipea);
    unlink(pipeb);
    unlink(pipec);
    return 0;
}
