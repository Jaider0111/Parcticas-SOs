#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <time.h>
#include <math.h>
#include <unistd.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define PERMISSIONS 0666

int main(){
	printf ( "PASO" ) ; 	
	// Creacion de clave para memoria compartida
	key_t key = 1234;
	if (key == -1)
	{
		printf("No se pudo conseguir la clave para la memoria compartida");
		exit(0);
	}
	
	
	//tuberia para el envio de los datos de consulta
    char *pipea = "/usr/pipea";
    //tuberia para recibir el resultado de la consulta
    char *pipeb = "/usr/pipeb";
    
    //pw y pr descriptores de archivo para las 2 tuberias anteriores
    int pw, pr, r, option;
    int kb = 1024;
    //tamaños
    int sizes[] = {kb, 10*kb, 100*kb, kb*kb, 10*kb*kb, 100*kb*kb};
    printf ( "%d" , 100 * kb * kb ) ; 
    //Creacion de la memoria compartida
	int id_memory = shmget (key, sizeof(char)*100*kb*kb, PERMISSIONS| IPC_CREAT);
	if (id_memory == -1)
	{
		printf("No consigo Id para memoria compartida");

		exit (0);
	}
    
    
    for(int j = 0; j < 6; j++){
        //Determina el tamaño de los datos a transmitir
        int size = sizes[j];
        //Variables para contar el tiempo
        struct timespec begin, end; 
        //numero de ve10ces que se repitira la transmicion
        int reps = (size <= 1000000) ? 20 : 8;
        //variable para la suma de los tiempos
        double total_time = 0;
        long int seconds, nanoseconds;
        char c[2];
        
        char *msg = (char *)malloc(sizeof(char) * size);

		for ( int d = 0 ; d < size ; d++ ) {
			msg [ d ] = '1' ; 
			
		}
        for(int i = 0; i < reps; i++){
            
            clock_gettime(CLOCK_REALTIME, &begin);
			
			char *memory = (char *)shmat (id_memory, 0, 0) ; 
			if (memory == NULL){
				printf ( "Error al buscar la memoria compartida" ) ;
				exit (0);
			}
			strcpy (memory, msg);
			 
			shmdt (memory);

            pw = open(pipea, O_WRONLY);
            write(pw, "r", 1);
            close(pw);
            pr = open(pipeb, O_RDONLY);
            r = read(pr, c, 2);
            close(pr);
            
            clock_gettime(CLOCK_REALTIME, &end);
            
            seconds = end.tv_sec - begin.tv_sec;
            nanoseconds = end.tv_nsec - begin.tv_nsec;
            total_time += seconds + nanoseconds*(1e-9);
        
        }
        double prom = total_time / reps;
        if(size < 1000000){
            size /= kb;
            printf("El tiempo promedio para compatir %3dKB es: %.10f segundos\n", size, prom);
        }else{
            size /= kb*kb;
            printf("El tiempo promedio para compatir %3dMB es: %.10f segundos\n", size, prom);
        }
        //Liberación memoria compartida
    }
	shmctl (id_memory, IPC_RMID, (struct shmid_ds *)NULL);

	return 0;
}
