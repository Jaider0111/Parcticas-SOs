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
    int pos ; 
};
  
int main(int argc, char *argv[]){
    FILE *infile, *hash;
    infile = fopen ("data_indexing", "r");
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
    int org = atoi(argv[1]), des = atoi(argv[2]), hor = atoi(argv[3]);
    printf("%d %d %d\n", org, des, hor);

    fseek(infile, sizeof(struct viaje)*hash_table[org-1], SEEK_SET);

    fread(&reg, sizeof(struct viaje), 1, infile);
	printf ( "%ld\n",hash_table[org-1] );
	printf ( "%d\n",reg.origen);
    while (reg.origen == org){
        if(reg.destino == des && reg.hora == hor) break;
        if ( reg.pos == -1 ){
			break ; 
		}
        fseek(infile, sizeof(struct viaje)*reg.pos, SEEK_SET);
        fread(&reg, sizeof(struct viaje), 1, infile);
        
       
    }
    
    if(reg.origen == org)
        printf("%d,%d,%d,%.2f,%.2f,%.2f,%.2f\n", reg.origen, reg.destino,
            reg.hora, reg.media, reg.desviacion, reg.med_geo, reg.desv_geo);
    else
        printf("Registro no encontrado\n");
}
