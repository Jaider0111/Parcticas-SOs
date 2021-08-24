#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <netdb.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <strings.h>
#include <unistd.h>
#include <semaphore.h>
#include <pthread.h>
#include <time.h>

//Puerto en el que va a escuchar el servidor
#define PORT 3535
//Numero maximo de clientes que pueden acceder partalelamente al servidor
#define BACKLOG 1

//Constantes utiles para el programa
#define PERMISSIONS 0666  

int r ;

//Variable para almacenar el numero de clientes
int num_client ;


int main(){  
	//Descriptores para los sockets cliente y servidor 
    int serverfd,clientfd , opt = 1;
    struct sockaddr_in server, client;
    socklen_t socklen = sizeof(struct sockaddr_in);  
    serverfd = socket(AF_INET, SOCK_STREAM, 0);
    if(serverfd < 0){
        perror("Error en server");
        exit(-1);
    }
    server.sin_family = AF_INET;
    server.sin_port = htons(PORT);
    server.sin_addr.s_addr = INADDR_ANY;
    bzero(&server.sin_zero, 8);
    setsockopt(serverfd, SOL_SOCKET, SO_REUSEADDR, (const char *)&opt, sizeof(int));
    r = bind(serverfd, (struct sockaddr*) &server, sizeof(struct sockaddr));
    if(r < 0){
        perror("Error en bind");
        exit(-1);
    }
    r = listen(serverfd, BACKLOG);
    if(r < 0){
        perror("Error en listen");
        exit(-1);
    }

    int kb = 1024;
    //tamaños
    int sizes[] = {kb, 10*kb, 100*kb, kb*kb, 10*kb*kb, 100*kb*kb};
    clientfd = accept(serverfd,(struct sockaddr*) &client, &socklen);
    if(clientfd < 0){
        perror("Error en accept");
    }
    for(int j = 0; j < 6; j++){
        //Determina el tamaño de los datos a transmitir
        int size = sizes[j];
        char *msg = (char *)malloc(sizeof(char) * size);
        //llenado del mensaje
        for(int i = 0; i < size; i++){
            msg[i] = '1';
        }
        //Variables para contar el tiempo
        struct timespec begin, end; 
        //numero de ve10ces que se repitira la transmicion
        int reps = (size <= 1000000) ? 20 : 8;
        //variable para la suma de los tiempos
        double total_time = 0;
        long int seconds, nanoseconds;
        char c[2];
        r = recv(clientfd, c, 2, 0);
        for(int i = 0; i < reps; i++){
            clock_gettime(CLOCK_REALTIME, &begin);

            int wrt = 0; /* Núm. de caracteres que ya se han escrito */
            int sz = sizeof(char) * size; /* Total de caracteres a escribir */

            /* Bucle mientras no se hayan escrito todos los caracteres
            deseados */
            while (wrt < sz){
                /* Se escriben los caracteres */
                r = send (clientfd, msg + wrt, sz - wrt, 0);
                /* Si hemos conseguido escribir algún carácter */
                if (r > 0)
                    /* Incrementamos el número de caracteres escritos */
                    wrt += r;
                else{
                    if (r == 0)
                        printf ("Socket cerrado\n");
                    else if (r == -1)
                        printf ("Error\n");
                }
            }
            r = recv(clientfd, c, 2, 0);
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
        //Liberacion de la memoria asignada al mensaje
        free(msg);
    }
	//cierre de la conexion con el cliente
	close(clientfd);

    //Se cierra el servidor
    close(serverfd);
}
