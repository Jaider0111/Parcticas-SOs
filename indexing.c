#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Estructura con la cual se van a almacenar los datos en el archivo binario
struct viaje {
	
    int origen;
    int destino;
    int hora;
    float tvm; // tiempo de viaje medio
    float dstv; // desviación estándar tiempo de viaje
    float mgtv; // media geométrica tiempo de viaje
    float dsgtv; // desviación estándar geométrica tiempo de viaje
    int origen_next; // número de lineas abajo hasta encontrar mismo origen y mismo destino

};

int main () {
	
    // Apuntadores para los archivos
    FILE *infile, *outfile, *hashfile;


    int origen, origen_next, count = 0;
    fpos_t pos;
    char c = 'a';

    // Memoria dinamica en la cual se guardará la tabla hash
    long *hashtable = malloc(1160*sizeof(long));
    if (hashtable == NULL) {
        printf("Error while allocating memory for hash table");
        exit(-1);
    }
    memset(hashtable, -1, 1160*sizeof(long));

    // Apertura en modo lectura del archivo csv que contiene los datos en bruto
    infile = fopen ("bogota-cadastral-2020-1-All-HourlyAggregate.csv", "r");
    if (infile == NULL) {
        printf("Error while opening the csv file");
        exit(-1);
    }

    // Apertura en modo escritura del archivo binario que contendra los datos ordenados en bloques
    outfile = fopen ("indexed-by-origin", "w");
    if (outfile == NULL) {
        printf("Error while opening indexed-by-origin file");
        exit(-1);
    }

    // Apertura en modo escritura del archivo binario que contendra la tabla hash
    hashfile = fopen ("hash-table", "w");
    if (outfile == NULL) {
        printf("Error while opening hash-table file");
        exit(-1);
    }

    // Lectura para omitir la primera linea del archivo csv que contiene el encabezado
    while (c != '\n')
        c = fgetc(infile);

    //Variable del tipo estructura viaje para almacenamiento binario
    struct viaje reg;

    while (feof(infile) == 0){
		
	// Lectura de un registro
	fscanf(infile, "%d,%d,%d,%f,%f,%f,%f\n", &reg.origen, &reg.destino,
			&reg.hora, &reg.tvm, &reg.dstv, &reg.mgtv, &reg.dsgtv);

	// Verificar si el origen del registro se encuentra en la tabla hash,
	// Si no es asi se agrega y se realizara el bloque de registros con este origen
	if (hashtable[reg.origen-1] == -1){
		hashtable[reg.origen-1] = count;
		break;
	}
		
        // Verificar que no se haya llegado al final del archivo
        if (feof(infile)) break;

	// La variable origen permitira seleccionar los registros con este mismo valor para el origen
        origen = reg.origen;
	destino = reg.destino;

        // Se guarda la actual posicion del cursor para poder volver y se puedan ordenar todos los registros
        fgetpos(infile, &pos);

	struct viaje nextreg;

        while (feof(infile) == 0) {
            // Lectura de un registro
            fscanf(infile, "%d,%d,%d,%f,%f,%f,%f\n", &nextreg.origen, &nextreg.destino, &nextreg.hora,
                &nextreg.tvm, &nextreg.dstv, &nextreg.mgtv, &nextreg.dsgtv);
			
	    origen_dest_next++;
			
            // Se verifica que el origen coincida con el deseado
            if (nextreg.origen == origen)
		break;
			
        }
		
	reg.origen_next = origen_next;
		
	// Se escribe el registro leido previamente
        fwrite(&reg, sizeof(struct viaje), 1, outfile);
        count++;
		
        // Volvemos el cursor a la posicion guardada para continuar con el ordenamiento
        fsetpos(infile, &pos);
    }
    // Escritura de la tabla hash en el archivo
    fwrite(hashtable, 1160*sizeof(long), 1, hash);

    // Cierre de los archivos usados
    fclose(infile);
    fclose(outfile);
    fclose(hash);

    // Se libera la memoria reservada con hashtable
    free(hashtable);
}
