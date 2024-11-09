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

void escribirEnChat(char,envio);
void leerChat(char);

int main(int argc,char **argv){
    envio usuario;
    char nombreUsuario[40];
    printf("ingrese nombre de usuario: \n");
    scanf("%s",nombreUsuario);
    strcpy(usuario.user,nombreUsuario);
    int eleccion;
    while(eleccion != 3){
        printf("1) enviar mensaje\n");
        printf("2) mostrar mensaje\n");
        printf("3) salir\n");
        printf("Ingrese su eleccion: \n");
        scanf("%i",&eleccion);
        switch (eleccion){
            case 1:
                break;
            case 2:
                break;
            case 3:
                break;
        }
    }

    return 0;
}