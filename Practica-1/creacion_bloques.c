#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Estructura con la cual se van a almacenar los datos en el archivo binario
struct viaje{
    int origen;
    int destino;
    int hora;
    float media;
    float desviacion;
    float med_geo;
    float desv_geo;
};
  
int main (){
    //Apuntadores para los archivos
    FILE *infile, *outfile, *hash;

    //Varibles usadas para el ordenamiento 
    int origen, cont = 0;
    fpos_t pos;
    char c = 'a';
    
    //Memoria dinamica en la cual se guardara la tabla hash
    long *hash_table = malloc(1160*sizeof(long));
    if (hash_table == NULL){
        printf("Error malloc");
        exit(-1);
    }
    memset(hash_table, -1, 1160*sizeof(long));

    //Apertura en modo lectura del archivo csv que contiene los datos en bruto
    infile = fopen ("bogota-cadastral-2020-1-All-HourlyAggregate.csv", "r");
    if (infile == NULL){
        printf("Error archivo de lectura");
        exit(-1);
    }

    //Apertura en modo escritura del archivo binario que contendra los datos ordenados en bloques 
    outfile = fopen ("data_blocks", "w");
    if (outfile == NULL){
        printf("Error archivo de lectura");
        exit(-1);
    }

    //Apertura en modo escritura del archivo binario que contendra la tabla hash
    hash = fopen ("tabla_hash", "w");
    if (outfile == NULL){
        printf("Error archivo tabla hash");
        exit(-1);
    }

    //lectura para omitir la primera linea del archivo csv que contiene el encabezado
    while (c != '\n')
        c = fgetc(infile);

    //Variable del tipo estructura viaje para almacenamiento binario
    struct viaje reg;

    while (feof(infile) == 0){
        while (feof(infile) == 0){
            //lectura de un registro
            fscanf(infile, "%d,%d,%d,%f,%f,%f,%f\n", &reg.origen, &reg.destino,
                &reg.hora, &reg.media, &reg.desviacion, &reg.med_geo, &reg.desv_geo);

            //Verificar si el origen del registro se encuentra en la tabla hash,
            //si no es asi se agrega y se realizara el bloque de registros con este origen
            if(hash_table[reg.origen-1] == -1){
                hash_table[reg.origen-1] = cont;
                break;
            }
        }
        //Verificar que no se haya llegado al final del archivo
        if(feof(infile)) break;

        //Se escribe el registro leido previamente
        fwrite(&reg, sizeof(struct viaje), 1, outfile);
        cont++;
        
        //La variable origen permitira seleccionar los registros con este mismo valor para el origen
        origen = reg.origen;

        //Se guarda la actual posicion del cursor para poder volver y se puedan ordenar todos los registros
        fgetpos(infile, &pos);

        while (feof(infile) == 0){
            //lectura de un registro
            fscanf(infile, "%d,%d,%d,%f,%f,%f,%f\n", &reg.origen, &reg.destino, &reg.hora,
                &reg.media, &reg.desviacion, &reg.med_geo, &reg.desv_geo);
            
            //se verifica que el origen coincida con el deseado
            if(reg.origen == origen){
                ////Se escribe el registro leido previamente
                fwrite(&reg, sizeof(struct viaje), 1, outfile);
                cont++;
            }
        }
        //Volvemos el cursor a la posicion guardada para continuar con el ordenamiento
        fsetpos(infile, &pos);
    }
    //Escritura de la tabla hash en el archivo
    fwrite(hash_table, 1160*sizeof(long), 1, hash);
    
    //cierre de los archivos usados
    fclose(infile);
    fclose(outfile);
    fclose(hash);

    //Se libera la memoria reservada con hash_table
    free(hash_table);
}