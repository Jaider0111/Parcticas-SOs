#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
    FILE *infile, *hash;
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

    long* hash_table = malloc(1160*sizeof(long));
    fread(hash_table, 1160*sizeof(long), 1, hash);

    struct viaje reg;

    for (int i = 0; i < 10; i++){
        fread(&reg, sizeof(struct viaje), 1, infile);
        printf("%d,%d,%d,%f,%f,%f,%f\n", reg.origen, reg.destino,
            reg.hora, reg.media, reg.desviacion, reg.med_geo, reg.desv_geo);
        for (int j = 0; j < 10; j++){
            fread(&reg, sizeof(struct viaje), 1, infile);
            printf("%d,%d,%d,%f,%f,%f,%f\n", reg.origen, reg.destino,
                reg.hora, reg.media, reg.desviacion, reg.med_geo, reg.desv_geo);
        }
    }

    fseek(infile, sizeof(struct viaje)*hash_table[278], SEEK_SET);
    fread(&reg, sizeof(struct viaje), 1, infile);
    printf("\n%d,%d,%d,%f,%f,%f,%f\n", reg.origen, reg.destino,
        reg.hora, reg.media, reg.desviacion, reg.med_geo, reg.desv_geo);
}