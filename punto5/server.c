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
    int retorno;
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
    printf("el msgiq es %i\n",msgid);
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
        if (msgrcv(msgid, &pedido, sizeof(pedido) - sizeof(long), 1,0) == -1) {
            perror("Error al recibir el mensaje");
            exit(EXIT_FAILURE);
        }
        //<pid>,<registro>,<descripcion>
        printf("Server: <%d>,<%i>,<%s>\n",pedido.idProc,pedido.selecRegistro,pedido.descripcion);
        printf("registro recibido: %i\n",pedido.selecRegistro);
        fseek(archivo,pedido.selecRegistro*sizeof(struct registro),SEEK_SET);
        fread(&reg,sizeof(struct registro),1,archivo);
        if(strcmp(pedido.descripcion,"eliminar")==0){
            if(reg.estado == 0){
                pedido.tipo = pedido.idProc;
                pedido.retorno = 0;
                printf("fallo: el registro: %i esta vacio (estado 0)\n",pedido.selecRegistro);
                snprintf(pedido.descripcion,LARGO_MENSAJE_MAX,"Fallo: registro vacio");
                msgsnd(msgid,&pedido,sizeof(struct mensaje)-sizeof(long),0);
            }
            pedido.retorno = 1;
            reg.estado = 2;
            memset(pedido.descripcion,'\0',LARGO_MENSAJE_MAX);
            printf("Server: registro %i, cambiado por cliente %i\n",pedido.selecRegistro,pedido.idProc);
            if(reg.estado == 2){
                snprintf(pedido.descripcion,LARGO_MENSAJE_MAX,"eliminado correctamente");
                msgsnd(msgid,&pedido,sizeof(struct mensaje)-sizeof(long),0);
            }
        }else if(strcmp(pedido.descripcion,"leer")){
            
        }

        fseek(archivo,sizeof(struct registro)*pedido.selecRegistro,SEEK_SET);
        fwrite(&reg,sizeof(struct registro),1,archivo);
    }
    fclose(archivo);
    return 0;   
}

void handlerSIGINT(int signal){
    printf("se ejecuto la senial %i\n",signal);
}