#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <termios.h>

#define PORT 3535
/*Constantes utiles para el programa*/
#define PERMISSIONS 0666  


int main(){
    
    //pw y pr descriptores de archivo para las 2 tuberias anteriores
    int clientfd, r, option;
    struct sockaddr_in client;
    struct hostent *he;

    clientfd = socket(AF_INET, SOCK_STREAM, 0);
    if(clientfd < 0){
        perror("\n-->Error en socket():");
        exit(-1);
    }
    client.sin_family = AF_INET;
    client.sin_port = htons(PORT);
 
    inet_aton("127.0.0.1", &client.sin_addr);
    //abrimos la tuberia de envio
    r = connect(clientfd, (struct sockaddr *)&client, (socklen_t)sizeof(struct sockaddr));
    if(r < 0){
        perror("\n-->Error en connect(): ");
        exit(-1);
    }
    int kb = 1024;
    //tamaños
    int sizes[] = {kb, 10*kb, 100*kb, kb*kb, 10*kb*kb, 100*kb*kb};
    for(int j = 0; j < 6; j++){
        //Determina el tamaño de los datos a transmitir
        int size = sizes[j];
        //puntero hacia el mensaje que se va a recibir y  asignacion de la memoria necesaria para el mensaje
        char *msg = (char *)malloc(sizeof(char) * size);

        //numero de veces que se repitira la recepcion
        int reps = (size <= 1000000) ? 20 : 8;

        char c[2];
        r = send(clientfd, "r", 1, 0);
        for(int i = 0; i < reps; i++){
            int wrt = 0; /* Núm. de caracteres que ya se han leido */
            int sz = sizeof(char) * size; /* Total de caracteres a leer */

            /* Bucle mientras no se hayan leido todos los caracteres
            deseados */
            while (wrt < sz){
                /* Se leen los caracteres */
                r = recv (clientfd, msg + wrt, sz - wrt, 0);
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
            r = send(clientfd, "r", 1, 0);
        }
        //Liberacion de la memoria asignada al mensaje
        free(msg);
    }
    close(clientfd);
}