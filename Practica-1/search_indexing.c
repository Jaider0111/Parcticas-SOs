#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

/*Constantes utiles para el programa*/
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
    int pos ;
};

int main(){
    //Apuntador a los valores de origen, destino y hora que se recibiran del otro proceso
    int *data = malloc(3*sizeof(int));
    
    //Variable para enviar el resultado de la consulta
    char response[RES_SIZE];
    //variable para recibir los datos de consulta
    char request[REQ_SIZE];
    
    //tuberia para el envio de los datos de consulta
    char *pipea = "/usr/pipea";
    //tuberia para recibir el resultado de la consulta
    char *pipeb = "/usr/pipeb";
    
    //pw y pr descriptores de archivo para las 2 tuberias anteriores
    int pw, pr, r;

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

    //variable del tipo estructura definido anteriormeente
    struct viaje reg;
    
    //infile apuntador al archivo de datos en bloques,
    //hash apuntador al archivo con la tabla hash
    FILE *infile, *hash;

    //apertura de los archivos
    infile = fopen ("data_indexing", "r");
    if (infile == NULL){
        printf("Error lectura");
        exit(-1);
    }
    
    hash = fopen ("tabla_hash_ind", "r");
    if (infile == NULL){
        printf("Error lectura");
        exit(-1);
    }

    //Carga de la tabla hash a memoria
    long* hash_table = malloc(1160*sizeof(long));
    fread(hash_table, 1160*sizeof(long), 1, hash);

    //Proceso de busqueda para cada trio de datos recibidos
    while(1){
        //apertura de la tuberia que recibe datos
        pr = open(pipea, O_RDONLY);
        //lectura de datos desde la tuberia a la variable request
        r = read (pr, request, 13);
        //cierre de la tuberia que recibe datos
        close(pr);
        //asignamos 0(caracter nulo) al ultimo byte de la cadena recibida con los datos para evitar errores
        request[r] = 0;

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
            if ( reg.pos == -1 ){
                break ; 
            }
            fseek(infile, sizeof(struct viaje)*reg.pos, SEEK_SET);
            fread(&reg, sizeof(struct viaje), 1, infile);
        }      

        //apertura de la tuberia que enviar el resultado de la busqueda
        pw = open(pipeb, O_WRONLY);
        //caso en el que se encuantra el registro especificado
        if(reg.origen == data[0]){
            //Escritura de la media del registro en la variable de envio de resultados
            sprintf(response, "%4.2f", reg.media);
            //Envio del resultado en ese registro
            write(pw, response, 8);
        }
        //caso que ese registro no exista
        else{
            //Se escribe NA en la variable de envio de resultados ya que no se encontro el registro
            sprintf(response, "NA");
            //Envio del resultado en ese registro
            write(pw, response, 2);
        }
        //cierre de la tuberia que enviar el resultado de la busqueda
        close(pw);
    }
    //Se borran las tuverias nombradas creadas para la ejecucion
    unlink(pipea);
    unlink(pipeb);
}