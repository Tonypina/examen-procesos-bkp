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
#include <fcntl.h>

// #define sizeof(dirp) 512
#define LISTA_NOMBRE "lista_bkp.txt"
#define FIN_MSG "FIN"

void generarLista(char *archivo);
int crear_lista_bkp(char *BASE_DIR);
int crearcar(void);
int total_archivos(char *directory);
void removerCaracteres(char *cadena, char *caracteres);

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
    
    int tDir, nArch,cont;


    FILE *file;
    file = fopen(LISTA_NOMBRE, "w");

    dp_2 = opendir(argv[1]);
    dp_1 = opendir(argv[1]);

    if ((pid = fork()) == 0) { // hijo
        close(a[1]); /* cerramos el lado de escritura del pipe */
        close(b[0]); /* cerramos el lado de lectura del pipe */

        printf("Estoy leyendo el numero de archivos a crear la copia\n");
        int n=read(a[0],buf,sizeof(buf));

        nArch=atoi(buf);
        printf("\n Bites %d",n);
        printf("El numero total de archivos a respaldar es: %d\n",nArch);
        cont = nArch;
        cont++;
        
        
        while( read(a[0], buf, sizeof(buf)) ){
            char caracteres[] ="\n";
            removerCaracteres(buf, caracteres);


            if( strcmp(buf, FIN_MSG) == 0 )
                break;

            if ( (strcmp(buf, ".")) == 0 || (strcmp(buf, "..")) == 0 )
                continue;

            printf("Nombre del archivo recibido: %s \t Archivos restantes:(%d/%d)\n", buf,cont,nArch);

            char comando[250] = "cp ";
            strcat(comando, argv[1]);
            strcat(comando, "/");
            strcat(comando, buf);
            strcat(comando, " ./DirecBKP");

            system(comando);

            if (cont >=0)
            {
                /* code */
                cont--;
            }
            

        }

        close(a[0]);

        write(b[1], FIN_MSG, sizeof(FIN_MSG));
        close(b[1]);
    
    } else { // padre
        close(a[0]); /* cerramos el lado de lectura del pipe */
        close(b[1]); /* cerramos el lado de escritura del pipe */    

        crearcar(); //Creamos el directorio   
        //crear_lista_bkp( argv[1] );
            ///////////////////////////////
        sprintf(buf,"%d",total_archivos(argv[1]));
        write(a[1], buf, sizeof(buf));
        
        
        while ((dirp_1 = readdir(dp_1)) != NULL){

            if ( (strcmp(dirp_1->d_name, "..") == 0) || (strcmp(dirp_1->d_name, ".") == 0) ) {
                continue;
            }

            printf("nombre: %s \n",buf);

            fputs(strcat(dirp_1->d_name, "\n"), file);
            
            strcpy(buf,dirp_1->d_name);
            write(a[1], buf, sizeof(buf));//Pasandolo hijo

        }

        write(a[1], FIN_MSG, sizeof(FIN_MSG));

        fclose(file);

        if( read(b[0], buf, sizeof(buf)) ) {

            file = fopen("lista_bkp.txt", "r");
            if(!file) {
                exit(-1);
            }

            char *mostrar;
            size_t len = 0;
            printf("\nLista de los archivos respaldados: \n\n");

            while( getline(&mostrar, &len, file) != -1 ){
                printf("%s", mostrar);
            }

            fclose(file);
            free(mostrar);
        }
        close(a[1]);
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

void removerCaracteres(char *cadena, char *caracteres) {
  int indiceCadena = 0, indiceCadenaLimpia = 0;
  int deberiaAgregarCaracter = 1;
  // Recorrer cadena carácter por carácter
  while (cadena[indiceCadena]) {
    // Primero suponemos que la letra sí debe permanecer
    deberiaAgregarCaracter = 1;
    int indiceCaracteres = 0;
    // Recorrer los caracteres prohibidos
    while (caracteres[indiceCaracteres]) {
      // Y si la letra actual es uno de los caracteres, ya no se agrega
      if (cadena[indiceCadena] == caracteres[indiceCaracteres]) {
        deberiaAgregarCaracter = 0;
      }
      indiceCaracteres++;
    }
    // Dependiendo de la variable de arriba, la letra se agrega a la "nueva
    // cadena"
    if (deberiaAgregarCaracter) {
      cadena[indiceCadenaLimpia] = cadena[indiceCadena];
      indiceCadenaLimpia++;
    }
    indiceCadena++;
  }
  // Al final se agrega el carácter NULL para terminar la cadena
  cadena[indiceCadenaLimpia] = 0;
}