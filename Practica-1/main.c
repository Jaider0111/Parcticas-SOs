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


//Funcion que imprime el menu
void menu(){
    printf("\
    1. Ingresar origen\n\
    2. Ingresar destino\n\
    3. Ingresar hora\n\
    4. Buscar tiempo de viaje medio\n\
    5. Salir\n");
}

int main(){
    //Apuntador a los valores de origen, destino y hora a consultar
    int *data = malloc(3*sizeof(int));
    memset(data, -1, 3*sizeof(int));

    //variable para recibir el resultado de la consulta
    char response[RES_SIZE];
    //variable para envia los datos de consulta
    char request[REQ_SIZE];
    
    //tuberia para el envio de los datos de consulta
    char *pipea = "/usr/pipea";
    //tuberia para recibir el resultado de la consulta
    char *pipeb = "/usr/pipeb";
    
    //pw y pr descriptores de archivo para las 2 tuberias anteriores
    int pw, pr, r, option;

    //Inicio de la interaccion con el usuario
    printf("Bienvenido\n");
    menu();
    while(1){
        printf("\nIngresa una opción del menú: ");

        //Se verifica que se halla recibido un numero
        if(scanf("%d", &option) == 0){
            for( int c = getchar(); c != EOF && c != ' ' && c != '\n' ; c = getchar());
            option = -1;
        }

        //Se verifica cada una de las opciones del menu y sus respectivas funciones
        //Se verifica que la opcien este entre 1 y 5
        if(option > 5 || option < 1){
            printf("\nSeleciona una opcion valida\n");
            continue;
        }
        //Opcion ingresar origen
        else if (option == 1){
            do{
                printf("\nIngrese ID del origen: ");
                if(scanf("%d", (data+0)) == 0){
                    for( int c = getchar(); c != EOF && c != ' ' && c != '\n' ; c = getchar());
                    data[0] = -1;
                }
                if(data[0] > 1160 || data[0] < 1){
                    printf("\nSeleciona un valor entre 1 y 1160\n");
                    data[0] = -1;
                }
            }while(data[0] > 1160 || data[0] < 1);
            continue;
        }
        //Opcion ingresar destino
        else if (option == 2){
            do{
                printf("\nIngrese ID del destino: ");
                if(scanf("%d", (data+1)) == 0){
                    for( int c = getchar(); c != EOF && c != ' ' && c != '\n' ; c = getchar());
                    data[1] = -1;
                }
                if(data[1] > 1160 || data[1] < 1){
                    printf("\nSeleciona un valor entre 1 y 1160\n");
                    data[1] = -1;
                }
            }while(data[1] > 1160 || data[1] < 1);
            continue;
        }
        //Opcion ingresar hora del dia
        else if (option == 3){
            do{
                printf("\nIngrese hora del día: ");
                if(scanf("%d", (data+2)) == 0){
                    for( int c = getchar(); c != EOF && c != ' ' && c != '\n' ; c = getchar());
                    data[2] = -1;
                }
                if(data[2] > 23 || data[2] < 0){
                    printf("\nSeleciona un valor entre 0 y 23\n");
                    data[2] = -1;
                }
            }while(data[2] > 23 || data[2] < 0);
            continue;
        }
        //Opcion buscar tiempo de viaje medio
        else if (option == 4){
            int c = 0;
            if(data[0] == -1){
                printf("Se debe seleccionar el origen\n");
                c++;
            }
            if(data[1] == -1){
                printf("Se debe seleccionar el destino\n");
                c++;
            }
            if(data[2] == -1){
                printf("Se debe seleccionar la hora de día\n");
                c++;
            }
            if(c > 0) continue;
        }
        //Opcion salir
        else if(option == 5) 
        //el origen, destino y hora se ponen en 0 para avisar al otro proceso que termino la ejecucion
            memset(data, 0, 3*sizeof(int));

        //Pasamos el origen, destino y hora a la variable de envio    
        sprintf(request, "%4d %4d %2d", data[0], data[1], data[2]);

        //abrimos la tuberia de envio
        pw = open(pipea, O_WRONLY);
        //escribimos la variable de envio en la tuberia
        write(pw, request, REQ_SIZE);
        //cerramos la tuberia de envio
        close(pw);

        //luego de enviar verificamos si debemos salir de ejecucion
        if(option == 5) break;

        //abrimos la tuberia de resultados
        pr = open(pipeb, O_RDONLY);
        //leemos el contenido de la tuberia en la variable de resultado
        r = read(pr, response, RES_SIZE);
        //cerramos la tuberia de resultados
        close(pr);
        //asignamos 0(caracter nulo) al ultimo byte de la cadena recibida con el resultado para evitar errores
        response[r] = 0; 

        //se muestra el resultado al usuario
        printf("\nTiempo de viaje medio: %s\n\n", response);
        
        //cambiamos el valor de origen, destino y hora a -1 para iniciar una nueva consulta
        memset(data, -1, 3*sizeof(int));

        //Esperamos la accion del usuario para continuar
        printf("Presione ENTER para continuar");
        getchar();
        getchar();

        //lispiamos la pantalla
        system("clear");

        //Se muestra el menu nuevamente
        printf("\nBienvenido\n");
        menu();
    }
}