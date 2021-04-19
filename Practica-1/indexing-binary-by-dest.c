#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

// Estructura con la cual se van a almacenar los datos en el archivo binario
struct viaje {

    int origen;
    int destino;
    int hora;
    float media;
    float desviacion;
    float med_geo;
    float desv_geo;
};

// Estructura para escribir en el nuevo archivo binario indexado por destinos
struct w {
    int origen;
    int destino;
    int hora;
    float tvm; // tiempo de viaje medio
    float dstv; // desviación estándar tiempo de viaje
    float mgtv; // media geométrica tiempo de viaje
    float dsgtv; // desviación estándar geométrica tiempo de viaje
    int dest_next; // número de lineas abajo hasta encontrar mismo origen y mismo destino

};

int main () {

    // Apuntadores para los archivos
    FILE *infile, *outfile;

    int origen, dest, dest_next, count = 0;
    fpos_t pos;

    // Apertura del archivo binario en bloques
    infile = fopen("data_blocks", "rb");
    if (infile == NULL) {
        printf("Error while opening the csv file");
        exit(-1);
    }

    // Apertura en modo escritura en binario del archivo que tendrá los bloques indexados por destino
    outfile = fopen ("indexed-by-dest", "wb");
    if (outfile == NULL) {
        printf("Error while opening indexed-by-dest file");
        exit(-1);
    }

    // con esta estructura se determinará el tamaño para leer del data_blocks
    struct viaje reg;

    while (true) {

        bool x = false; // flag que indica si se llego al EOF en el loop interior

        dest_next = 0;

        // Lectura de un registro del archivo de data blocks
        size_t elements = fread(&reg, sizeof(struct viaje), 1, infile);
        if (elements==0) {
            printf("Error while reading binary file in the outside loop\n");
            break;
        }

        // Código para verificar que el índice generado para la primera linea es correcto
        // Se obtiene un dest_next de 620 y por eso aquí lo evaluamos con count==620 para ver el registro de esa linea
        // y observar si coincide en origen y destino con la primera linea (478 627)
        /*
        if (count==620) {
                printf("%d %d %d", reg.origen, reg.destino, reg.hora);
                break;
        }
        */

        // La variables origen y dest permitirá seleccionar los registros con este mismo valor para el origen
        origen = reg.origen;
        dest = reg.destino;

        // Se guarda la actual posicion del cursor para poder volver y se puedan ordenar todos los registros
        fgetpos(infile, &pos);

        // Estructura para leer del archivo binario en el loop interno
        struct viaje nextreg;

        while (true) {
            // Lectura de un registro
            size_t ret = fread(&nextreg, sizeof(struct viaje), 1, infile);
            // Manejo del error del fread
            if (ret != 1) {
               // fprintf(stderr, "fread() failed: %zu\n", ret);
               // printf("%d %d\n", count, dest_next);
               // exit(EXIT_FAILURE);
               x = true; // Activamos este flag para que el resto del código se entere que se llegó al fin del archivo
                         // sin encontrar una coincidencia de mismo origen y dest en el archivo más abajo
               break;
            }

            // lleva la cuenta del número de saltos hasta la próxima coincidencia
            // Se descarta más abajo si se ejecuta el anterior bloque de manejo de error
            dest_next++;

            // Se verifica que el origen coincida con el deseado
            if (nextreg.destino == dest && nextreg.origen == origen) {
                // printf("%d %d %d\n", nextreg.destino, count, dest_next);
                // x = true;
                break;
            }

        }

        // la estructura para escribir tomará los datos del registro analizado en esta iteración
        struct w newreg;
        newreg.origen = reg.origen;
        newreg.destino = reg.destino;
        newreg.hora = reg.hora;
        newreg.tvm = reg.media;
        newreg.dstv = reg.desviacion;
        newreg.mgtv = reg.med_geo;
        newreg.dsgtv = reg.desv_geo;
        // Se agrega un campo que tiene el número de saltos.
        // Nótese que es cero si se detecta que se manejó el error
        // Significará que no habrán más saltos porque hasta el EOF no hay más coincidencias
        // en origen y destino
        if (x)
            newreg.dest_next = 0;
        else
            newreg.dest_next = dest_next;

        // printf("%d %d %d %d\n", newreg.origen, newreg.destino, newreg.hora, newreg.dest_next);





        // Se escribe el registro en el archivo binario de salida
        fwrite(&newreg, sizeof(struct w), 1, outfile);
        count++; // útil para debugging. Puede comentarse.

        // El siguiente código sirve para verificar que la escritura esté siendo correcta

        // fclose(outfile);

        /*outfile = fopen ("indexed-by-dest", "rb");
        struct w neww;
        fread(&neww, sizeof(struct w), 1, outfile);
        printf("%d %d %d %d\n", neww.origen, neww.destino, neww.hora, neww.dest_next);
        */

        // Volvemos el cursor a la posicion guardada para continuar con el ordenamiento
        fsetpos(infile, &pos);

        // if (x) break;
    }

    // Cierre de los archivos usados
    fclose(infile);
    fclose(outfile);


}
