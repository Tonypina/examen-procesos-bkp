/**
* Nombres*
* Roldán Sanchez Alexis
* Piña Rossette Marco Antonio
* Colón Palacios Emmanuel
* Mancera Erick Samuel
**/

#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <errno.h>
#include <sys/stat.h>

// #define sizeof(dirp) 512
#define LISTA_NOMBRE "lista_bkp.txt"

void generarLista(char *archivo);
int crear_lista_bkp(char *BASE_DIR);
int crearcar(void);

int main(int argc, char **argv) {
    pid_t pid;
    int a[2], b[2], readbytes;
    // char dirp[sizeof(dirp)];
        
    // char* s_dir = argv[1];
    pipe(a);
    pipe(b);

    DIR *dp_1;
    struct dirent *dirp_1;
    
    dp_1 = opendir(argv[1]);

    if ((pid = fork()) == 0) { // hijo
        close(a[1]); /* cerramos el lado de escritura del pipe */
        close(b[0]); /* cerramos el lado de lectura del pipe */

        while ((readbytes = read(a[0], dirp_1, sizeof(dirp_1))) > 0){
            printf("Nombre del archivo recibido: %s", dirp_1->d_name);
        }
            write(1, dirp_1, readbytes);
        close(a[0]);

        // strcpy(dirp, "Soy tu hijo hablandote por la otra tuberia.\n " );
        write(b[1], dirp_1, sizeof(dirp_1)); close(b[1]);
    
    } else { // padre
        close(a[0]); /* cerramos el lado de lectura del pipe */
        close(b[1]); /* cerramos el lado de escritura del pipe */    

        crearcar(); //Creamos el directorio   
        //crear_lista_bkp( argv[1] );
            ///////////////////////////////

        int count = 0;

        DIR *dp_2;
        struct dirent *dirp_2;

        FILE *file;
        file = fopen(LISTA_NOMBRE, "w");
  
        dp_2 = opendir(argv[1]);

        while ((dirp_2 = readdir(dp_2)) != NULL){
            fputs(strcat(dirp_2->d_name, "\n"), file);
            count++;
            printf("Sigo \n");

        }
        printf("Sí salí \n");

        fclose(file);
        // sleep(1);
        printf("Paso del file \n");
        closedir(dp_2);
        printf("Dentro de la funcion\n");


        printf("No son mamadas\n");
        while ((dirp_1 = readdir(dp_1)) != NULL){
            printf("Estoy en el while");

            if ( (strcmp(dirp_1->d_name, ".")) != 0 && (strcmp(dirp_1->d_name, "..")) != 0 ) {
                printf("Hola");
                write(a[1], dirp_1, sizeof(dirp_1));
            }
        }
        close(a[1]);
        
        
        while ((readbytes = read(b[0], dirp_1, sizeof(dirp_1))) > 0)
            write(1, dirp_1, readbytes);
        close(b[0]);
        
    }
    waitpid(pid, NULL, 0);
    exit(0);
}

int crear_lista_bkp( char *BASE_DIR ){
    
    int count = 0;

    DIR *dp;
    struct dirent *dirp_1;

    FILE *file;
    file = fopen(LISTA_NOMBRE, "w");

    dp = opendir(BASE_DIR);

    while ((dirp_1 = readdir(dp)) != NULL){
        fputs(strcat(dirp_1->d_name, "\n"), file);
        count++;
        printf("Sigo \n");
    
    }
    printf("Sí salí \n");
    
    fclose(file);
    // sleep(1);
    printf("Paso del file \n");
     closedir(dp);
    printf("Dentro de la funcion\n");
    return count;
}

int crearcar (void){
    errno = 0;
    int ret = mkdir("DirecBKP", S_IRWXU);
    if (ret == -1) {
        switch (errno) {
            case EACCES :
                printf("No podemos crear el directorio \n");
                break;
            case EEXIST:
                printf("El directorio ya existe... Lo eliminaré y lo crearé again\n");
                system("rm -r DirecBKP");
                crearcar();
                break;

            case ENAMETOOLONG:
                printf("No funciona esa direccion\n");
                break;

            default:
                perror("mkdir");
                break;
                //HOLA 
        }
    }
}