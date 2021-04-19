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
    int pos ;
};

int main (){

    //Apuntadores para los archivos
    FILE *infile, *outfile, *hash;

    //Varibles usadas para el ordenamiento
    int origen, cont = 0;
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
    outfile = fopen ("data_indexing", "w");
    if (outfile == NULL){
        printf("Error archivo de lectura");
        exit(-1);
    }

    //Apertura en modo escritura del archivo binario que contendra la tabla hash
    hash = fopen ("tabla_hash_ind", "w");
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
        //lectura de un registro
		fscanf(infile, "%d,%d,%d,%f,%f,%f,%f\n", &reg.origen, &reg.destino,
        &reg.hora, &reg.media, &reg.desviacion, &reg.med_geo, &reg.desv_geo);
			
       
        reg.pos = hash_table[reg.origen-1] ; 
		
		// Se escribe el registro leido previamente
		fwrite(&reg, sizeof(struct viaje), 1, outfile);
		
		// Guardamos la posicion en donde se encuentra el registro, de acuerdo a su origen
		hash_table[reg.origen-1] = cont ; 
   

        //Se aumenta en 1 el contador de registros
        cont++;
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
