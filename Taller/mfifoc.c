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
    char *pipea = "/usr/pipea";	
    unlink(pipea);
    int r = mkfifo(pipea, PERMISSIONS);
    if (r < 0) {
        printf("error creating pipea\n");
        exit(-1);
    }
    char *pipeb = "/usr/pipeb"; 
    unlink(pipeb);
    r = mkfifo(pipeb, PERMISSIONS);
    if (r < 0) {
        printf("error creating pipeb\n");
	exit(-1);
    }
    int pr, pw;
    int KB = 1024;
    int sizes[] = { KB, 10*KB, 100*KB, KB*KB, 10*KB*KB, 100*KB*KB };
    for (int s = 0; s < 6; s++) {
        int bytes = sizes[s];
        char *data = (char *) malloc(bytes * sizeof(char));
        int reps = (bytes < KB*KB) ? 20 : 8;
        for (int i = 0; i < reps; i++) {	
            pr = open(pipea, O_RDONLY);	
            int wrt = 0; /* Núm. de caracteres que ya se han leido */
            int sz = sizeof(char) * bytes; /* Total de caracteres a leer */

            /* Bucle mientras no se hayan leido todos los caracteres
            deseados */
            while (wrt < sz){
                /* Se leen los caracteres */
                r = read(pr, data+ wrt, sz - wrt);
                /* Si hemos conseguido leer algún carácter */
                if (r > 0)
                    /* Incrementamos el número de caracteres leidos */
                    wrt += r;
                else{
                    if (r == 0)
                        printf ("Socket cerrado c\n");
                    else if (r == -1)
                        printf ("Error c\n");
                }
            }
            close(pr);
            pw = open(pipeb, O_WRONLY);
            write(pw, "r", 1);
            close(pw);
        }
        free(data);
    }
    unlink(pipea);
    unlink(pipeb);
    return 0;
}
