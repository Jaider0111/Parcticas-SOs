#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <time.h>
#include <math.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define PERMISSIONS 0666

int main()
{

    key_t key = 1234;

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
    if (r < 0)
    {
        printf("Error creando la tuberia de peticiones\n");
        exit(-1);
    }

    //Crea la tuberia nombrada pipeb
    r = mkfifo(pipeb, PERMISSIONS);
    if (r < 0)
    {
        printf("Error creando la tuberia de respuestas\n");
        exit(-1);
    }
    int kb = 1024;
    //tamaños
    int sizes[] = {kb, 10 * kb, 100 * kb, kb * kb, 10 * kb * kb, 100 * kb * kb};
    //Creacion de la memoria compartida
    int id_memory = shmget(key, sizeof(char) * kb * kb * kb, PERMISSIONS | IPC_CREAT);

    if (id_memory == -1)
    {
        printf("No consigo Id para memoria compartida");

        exit(0);
    }
    char *memory = (char *)shmat(id_memory, 0, 0);

    if (memory == NULL)
    {
        printf("Error al buscar la memoria compartida");
        exit(0);
    }

    for (int j = 0; j < 6; j++)
    {
        //Determina el tamaño de los datos a transmitir
        int size = sizes[j];
        //puntero hacia el mensaje que se va a recibir y  asignacion de la memoria necesaria para el mensaje
	char *msg = (char *)malloc(sizeof(char) * kb*kb*kb);


        //numero de veces que se repitira la recepcion
        int reps = (size <= 1000000) ? 20 : 8;

        char c[2];
        for (int i = 0; i < reps; i++)
        {

            pr = open(pipea, O_RDONLY);
            r = read(pr, c, 2);
            close(pr);

            strcpy(msg, memory);

            pw = open(pipeb, O_WRONLY);
            write(pw, "r", 1);
            close(pw);
        }
        //Liberacion de la memoria asignada al mensaje
        free(msg);
    }
    shmdt(memory);
    shmctl(id_memory, IPC_RMID, (struct shmid_ds *)NULL);

    //Se borran las tuverias nombradas creadas para la ejecucion
    unlink(pipea);
    unlink(pipeb);
    return 0;
}
