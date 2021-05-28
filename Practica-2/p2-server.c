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

//Puerto en el que va a escuchar el servidor
#define PORT 3535
//Numero maximo de clientes que pueden acceder partalelamente al servidor
#define BACKLOG 32
//Constantes utiles para el programa
#define RES_SIZE 9
#define REQ_SIZE 13
#define PERMISSIONS 0666  

//Estructura que contiene los datos de cada registro
struct viaje{
    int origen;
    int destino;
    int hora;
    float media;
    float desviacion;
    float med_geo;
    float desv_geo;
};

int main(){
    //Apuntador a los valores de origen, destino y hora que se recibiran del otro proceso
    int *data = malloc(3*sizeof(int));
    
    //Variable para enviar el resultado de la consulta
    char response[RES_SIZE];
    //variable para recibir los datos de consulta
    char request[REQ_SIZE];

    //Descriptores para los sockets cliente y servidor 
    int serverfd, clientfd, opt = 1;
    struct sockaddr_in server, client;
    socklen_t socklen;

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
    int r = bind(serverfd, (struct sockaddr*) &server, sizeof(struct sockaddr));
    if(r < 0){
        perror("Error en bind");
        exit(-1);
    }
    r = listen(serverfd, BACKLOG);
    if(r < 0){
        perror("Error en listen");
        exit(-1);
    }

    //variable del tipo estructura definido anteriormeente
    struct viaje reg;
    
    //infile apuntador al archivo de datos en bloques,
    //hash apuntador al archivo con la tabla hash
    FILE *infile, *hash;

    //apertura de los archivos
    infile = fopen ("data_blocks", "r");
    if (infile == NULL){
        printf("Error lectura");
        exit(-1);
    }
    
    hash = fopen ("tabla_hash", "r");
    if (infile == NULL){
        printf("Error lectura");
        exit(-1);
    }

    //Carga de la tabla hash a memoria
    long* hash_table = malloc(1160*sizeof(long));
    fread(hash_table, 1160*sizeof(long), 1, hash);

    //Se recibe la peticion del cliente
    clientfd = accept(serverfd,(struct sockaddr*) &client, &socklen);
    if(clientfd < 0){
        perror("Error en accept");
        exit(-1);
    }


    //Proceso de busqueda para cada trio de datos recibidos
    while(1){
        //lectura de datos desde la tuberia a la variable request
        r = recv(clientfd, request, REQ_SIZE, 0);
        //asignamos 0(caracter nulo) al ultimo byte de la cadena recibida con los datos para evitar errores
        request[r] = 0; 
        printf("%s\n", request);

        //extraemos los valores de origen, destino y hora desde la variable request 
        sscanf(request, "%d %d %d", (data+0), (data+1), (data+2));
        
        //Se verifica que no sea la señal de finalizacion
        if(*(data+0) == 0) break;

        //Movemos el apuntador a la posicion donde se encuantra el primer registro con el origen recibido
        fseek(infile, sizeof(struct viaje)*hash_table[data[0]-1], SEEK_SET);

        //Se lee el registro
        fread(&reg, sizeof(struct viaje), 1, infile);
        
        //Buscamos el registro que cumpla con el origen, destino y hora dada
        while (reg.origen == data[0]){
            if(reg.destino == data[1] && reg.hora == data[2]) break;
            fread(&reg, sizeof(struct viaje), 1, infile);
        }

        //caso en el que se encuantra el registro especificado
        if(reg.origen == data[0]){
            //Escritura de la media del registro en la variable de envio de resultados
            sprintf(response, "%4.2f", reg.media);
            //Envio del resultado en ese registro
            send(clientfd, &response, RES_SIZE, 0);
        }
        //caso que ese registro no exista
        else{
            //Se escribe NA en la variable de envio de resultados ya que no se encontro el registro
            sprintf(response, "NA");
            //Envio del resultado en ese registro
            send(clientfd, &response, 2, 0);
        }
        //cierre de la conexion con el cliente
    }
    close(clientfd);
    
    //Se libera la memoria ocupada por la tabla hash
    free(hash_table);

    //Cierre de archivos
    fclose(hash);
    fclose(infile);

    //Se cierra el servidor
    close(serverfd);
}