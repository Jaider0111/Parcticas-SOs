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
    FILE *infile, *outfile, *hash;
    int origen, cont = 0;
    fpos_t pos;
    long *hash_table = malloc(1160*sizeof(long));
    char c = 'a';
    if (hash_table == NULL){
        printf("Error malloc");
        exit(-1);
    }
    memset(hash_table, -1, 1160*sizeof(long));

    infile = fopen ("bogota-cadastral-2020-1-All-HourlyAggregate.csv", "r");
    if (infile == NULL){
        printf("Error archivo de lectura");
        exit(-1);
    }
    outfile = fopen ("data_blocks", "w");
    if (outfile == NULL){
        printf("Error archivo de lectura");
        exit(-1);
    }
    hash = fopen ("tabla_hash", "w");
    if (outfile == NULL){
        printf("Error archivo tabla hash");
        exit(-1);
    }
    while (c != '\n')
        c = fgetc(infile);

    struct viaje reg;
    while (feof(infile) == 0){
        while (feof(infile) == 0){
            fscanf(infile, "%d,%d,%d,%f,%f,%f,%f\n", &reg.origen, &reg.destino,
                &reg.hora, &reg.media, &reg.desviacion, &reg.med_geo, &reg.desv_geo);
            if(hash_table[reg.origen-1] == -1){
                hash_table[reg.origen-1] = cont;
                break;
            }
        }
        if(feof(infile)) break;
        fwrite(&reg, sizeof(struct viaje), 1, outfile);
        cont++;
        origen = reg.origen;
        fgetpos(infile, &pos);
        while (feof(infile) == 0){
            fscanf(infile, "%d,%d,%d,%f,%f,%f,%f\n", &reg.origen, &reg.destino, &reg.hora,
                &reg.media, &reg.desviacion, &reg.med_geo, &reg.desv_geo);
            if(reg.origen == origen){
                fwrite(&reg, sizeof(struct viaje), 1, outfile);
                cont++;
            }
        }
        fsetpos(infile, &pos);
    }
    fwrite(hash_table, 1160*sizeof(long), 1, hash);
    fclose(infile);
    fclose(outfile);
    fclose(hash);
    free(hash_table);
}