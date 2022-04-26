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
#define FIN_MSG "FIN"

void generarLista(char *archivo);
int crear_lista_bkp(char *BASE_DIR);
int crearcar(void);
int total_archivos(char *directory);

int main(int argc, char **argv) {
    pid_t pid;
    int a[2], b[2], readbytes;
    char buf[256],*buf2;
    // char dirp[sizeof(dirp)];
        
    // char* s_dir = argv[1];
    pipe(a);
    pipe(b);
    
    DIR *dp_1;
    DIR *dp_2;
    struct dirent *dirp_1;
    struct dirent *dirp_2;
    
    int tDir, nArch;


    FILE *file;
    file = fopen(LISTA_NOMBRE, "w");

    dp_2 = opendir(argv[1]);
    dp_1 = opendir(argv[1]);

    if ((pid = fork()) == 0) { // hijo
        sleep(2);
        printf("Estoy en el hijo\n");
        close(a[1]); /* cerramos el lado de escritura del pipe */
        close(b[0]); /* cerramos el lado de lectura del pipe */

        printf("Estoy leyendo el numero de archivos a crear la copia\n");
        int n=read(a[0],buf,sizeof(buf));

        nArch=atoi(buf);
        printf("\n Bites %d",n);
        printf("El numero total de archivos a respaldar es: %d\n",nArch);
        
        while( read(a[0], buf, sizeof(buf)) ){

            printf("Nombre del archivo recibido: %s", buf);
            if(strcmp(buf, FIN_MSG) == 0) {
                break;
            }

            char comando[250] = "cp ";

            
        
            

            strcat(comando, argv[1]);
            strcat(comando, "/");
            strcat(comando, buf);
            strcat(comando, " ./DirecBKP");

            printf("El comando seria: %s\n", comando);

            //system(comando);
        }

        close(a[0]);

        write(b[1], dirp_1, sizeof(dirp_1)); close(b[1]);
    
    } else { // padre
        close(a[0]); /* cerramos el lado de lectura del pipe */
        close(b[1]); /* cerramos el lado de escritura del pipe */    

        crearcar(); //Creamos el directorio   
        //crear_lista_bkp( argv[1] );
            ///////////////////////////////
        sprintf(buf,"%d",total_archivos(argv[1]));
        write(a[1], buf, sizeof(buf));
        
        
        while ((dirp_1 = readdir(dp_1)) != NULL){
            printf("\n Print antes del copy %s \n",dirp_1->d_name);
            printf("nombre: %s \n",buf);

            
            fputs(strcat(dirp_1->d_name, "\n"), file);
            
            strcpy(buf,dirp_1->d_name);
            write(a[1], buf, sizeof(buf));//Pasandolo hijo
            printf("Sigo \n");

        }

        write(a[1], FIN_MSG, sizeof(FIN_MSG));
        printf("Sí salí \n");

        fclose(file);
        // sleep(1);
        /*
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

        */
        close(b[0]);
        
    }
    waitpid(pid, NULL, 0);
    exit(0);
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

int total_archivos(char *directory){
    int file_count = 0;
    DIR * dirp;
    struct dirent * entry;

    dirp = opendir(directory); /* There should be error handling after this */
    while ((entry = readdir(dirp)) != NULL) {
        if (entry->d_type == DT_REG) { /* If the entry is a regular file */
            file_count++;
        }
    }
    closedir(dirp);
    printf("Num: %d",file_count);
return file_count;
}