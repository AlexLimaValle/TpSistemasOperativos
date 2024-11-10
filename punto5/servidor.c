#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/msg.h>
#include <string.h>
#include <signal.h>
#define LARGO_MENSAJE_MAX 100
#define ARCHIVO "archivo.txt"
#define CANT_MAX_REGISTROS 1000

struct mensaje{
    long tipo;
    char descripcion[LARGO_MENSAJE_MAX];
    int selecRegistro;
    pid_t idProc;
};

struct registro{
    int estado;
    char descripcion[LARGO_MENSAJE_MAX];
};

//hadler de SIGINT
void handlerSIGINT(int);

int main(int arcg,char **argv){
    signal(SIGINT,handlerSIGINT);
    //creacion de la cola de mensajes (ver cola con el comando IPCS)
    int msgid = msgget(0xa,IPC_CREAT|0600);
    if(msgid == -1){
        printf("Error al crear Cola de mensajes");
        exit(msgid);
    }
    //estructura de los registros y los pedidos que hagan los clientes
    struct registro reg;
    struct mensaje pedido;
    //archivo binario donde se graban los registros
    //r+b: necesito verificar si existe el archivo
    FILE* archivo;
    archivo = fopen(ARCHIVO,"r+b");
    if(!archivo){
        //creacion en caso de que no exista
        archivo = fopen(ARCHIVO,"w+b");
        if(!archivo){
            printf("hubo un error con el archivo\n");
            exit(0);
        }
        for (int i = 0; i < CANT_MAX_REGISTROS; i++) {
            reg.estado = 0;
            memset(reg.descripcion,'\0',sizeof(reg.descripcion));
            fwrite(&reg,sizeof(struct registro),1,archivo);
        }
    }

    while(1){
        
    }
    return 0;   
}

void handlerSIGINT(int signal){
    printf("se ejecuto la senial %i\n",signal);
}