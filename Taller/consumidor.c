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

int main(){

    //tuberia para el envio de los datos de consulta
    char *pipea = "/usr/pipea";
    //tuberia para recibir el resultado de la consulta
    char *pipeb = "/usr/pipeb";
    
    //pw y pr descriptores de archivo para las 2 tuberias anteriores
    int pw, pr, r;

    //Se borran las tuberias nombradas para evitar problemas si estas existen
    unlink(pipea);
    unlink(pipeb);

    //Crea la tuberia nombrada pipea
    r = mkfifo(pipea, PERMISSIONS);
    if(r < 0){
        printf("Error creando la tuberia de peticiones\n");
        exit(-1);
    }

    //Crea la tuberia nombrada pipeb
    r = mkfifo(pipeb, PERMISSIONS);
    if(r < 0){
        printf("Error creando la tuberia de respuestas\n");
        exit(-1);
    }
    int kb = 1024;
    //tamaños
    int sizes[] = {kb, 10*kb, 100*kb, kb*kb, 10*kb*kb, 100*kb*kb};
    FILE *file;
    file = fopen("file", "r");
    for(int j = 0; j < 6; j++){
        //Determina el tamaño de los datos a transmitir
        int size = sizes[j];
        //puntero hacia el mensaje que se va a recibir y  asignacion de la memoria necesaria para el mensaje
        char *msg = (char *)malloc(sizeof(char) * size);

        //numero de veces que se repitira la recepcion
        int reps = (size <= 1000000) ? 20 : 8;

        char c[2];
        for(int i = 0; i < reps; i++){
            pr = open(pipea, O_RDONLY);
            r = read (pr, c, 2);
            close(pr);
            int r = fread(msg, sizeof(char) * size, 1, file);
            fseek(file, 0, SEEK_SET);
            pw = open(pipeb, O_WRONLY);
            write(pw, "r", 1);
            close(pw);
        }
        //Liberacion de la memoria asignada al mensaje
        free(msg);
    }
    fclose(file);
    //Se borran las tuverias nombradas creadas para la ejecucion
    unlink(pipea);
    unlink(pipeb);
	return 0;
}