#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include <string.h>

typedef struct envio{
    char user[40];
    char mensaje[256];
}envio;

void escribirEnChat(const char*,envio*);

int main(int argc,char **argv){
    envio *usuario = malloc(sizeof(envio));
    char nombreUsuario[40];
    const char *nombre = "chatArch.txt";
    printf("ingrese nombre de usuario: \n");
    scanf("%s",nombreUsuario);
    strcpy(usuario->user,nombreUsuario);
    int eleccion;
    while(eleccion != 3){
        printf("1) enviar mensaje\n");
        printf("2) mostrar mensaje\n");
        printf("3) salir\n");
        printf("Ingrese su eleccion: \n");
        scanf("%i",&eleccion);
        getchar();
        switch (eleccion){
            case 1:
                escribirEnChat(nombre,usuario);
                break;
            case 2:
                break;
            case 3:
                break;
        }
    }
    return 0;
}

void escribirEnChat(const char *nombre,envio *perfil){
    FILE *archivo;
    //abrir archivo creado
    char mensajeAEscribir[256];
    archivo = fopen(nombre,"a");
    printf("[%s] -> ",perfil->user);
    fgets(perfil->mensaje,sizeof(perfil->mensaje),stdin);
    perfil->mensaje[strlen(perfil->mensaje)-1] = '\0';
    fprintf(archivo,"[%s] => %s\n",perfil->user,perfil->mensaje);
    fclose(archivo);
}