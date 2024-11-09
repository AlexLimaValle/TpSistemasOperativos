#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include <string.h>

//estructura simple de envio, para asociar usuario con mensaje
typedef struct envio{
    char user[40];
    char mensaje[256];
}envio;

void escribirEnChat(const char*,envio*);
void leerHistorial(const char*,envio*);

int main(int argc,char **argv){
    envio *usuario = malloc(sizeof(envio));
    char nombreUsuario[40];
    const char *nombre = "chatArch.txt";
    printf("ingrese nombre de usuario: \n");
    scanf("%s",nombreUsuario);
    strcpy(usuario->user,nombreUsuario);
    int eleccion;
    while(eleccion != 3){
        printf("****** CHAT ENTRE USUARIOS ******\n");
        printf("1) enviar mensaje\n");
        printf("2) mostrar historial mensajes\n");
        printf("3) salir\n");
        printf("Ingrese su eleccion: \n");
        scanf("%i",&eleccion);
        getchar();
        switch (eleccion){
            case 1:
                escribirEnChat(nombre,usuario);
                break;
            case 2:
                leerHistorial(nombre,usuario);
                break;
            case 3:
                break;
        }
    }
    free(usuario);
    return 0;
}

void escribirEnChat(const char *nombre,envio *perfil){
    FILE *archivo;
    do{
        //abrir archivo creado
        archivo = fopen(nombre,"a");
        printf("[%s] -> ",perfil->user);
        fgets(perfil->mensaje,sizeof(perfil->mensaje),stdin);
        perfil->mensaje[strlen(perfil->mensaje)-1] = '\0';
        if(strncmp(perfil->mensaje,"salir",5) != 0){
            fprintf(archivo,"[%s] => %s\n",perfil->user,perfil->mensaje);
        }else{
            fprintf(archivo,"[%s] => --- SALIO DE CONVERSACION ---\n",perfil->user);
        }
        fclose(archivo);
    }while(strncmp(perfil->mensaje,"salir",5));
}

void leerHistorial(const char* nombre,envio *perfil){
    FILE *archivo;
    archivo = fopen(nombre,"r");
    char parrafos[256];
    while(fgets(parrafos,sizeof(parrafos),archivo)){
        printf("%s\n",parrafos);
    }
    fclose(archivo);
}