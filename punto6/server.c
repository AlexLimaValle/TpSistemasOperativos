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
    pid_t pidBloqueante;
    int lock;
};

int msgid;

//hadler de SIGINT
void handlerSIGINT(int);

int main(int arcg,char **argv){
    signal(SIGINT,handlerSIGINT);
    //creacion de la cola de mensajes (ver cola con el comando IPCS)
    msgid = msgget(0xa,IPC_CREAT|0600);
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
        fseek(archivo,pedido.selecRegistro*sizeof(struct registro),SEEK_SET);
        fread(&reg,sizeof(struct registro),1,archivo);
        if (strcmp(pedido.descripcion,"lock") == 0) {
    // Si el registro está bloqueado
            if (reg.lock == 1) {
                snprintf(pedido.descripcion, LARGO_MENSAJE_MAX, "el registro ya esta en LOCK\n");
                msgsnd(msgid,&pedido,sizeof(struct mensaje)-sizeof(long),0);
            } else {
                reg.lock = 1;
                reg.pidBloqueante = pedido.idProc;
                printf("proceso %d, hace lock sobre registro %i\n", pedido.idProc, pedido.selecRegistro);
                snprintf(pedido.descripcion, LARGO_MENSAJE_MAX, "<registro:%i> bloqueado correctamente", pedido.selecRegistro);
                msgsnd(msgid,&pedido,sizeof(struct mensaje)-sizeof(long),0);
            }
        }else if(strcmp(pedido.descripcion,"unlock") == 0){
            if(reg.lock == 1 && reg.pidBloqueante == pedido.idProc){
                reg.lock = 0;
                reg.pidBloqueante = 0;
                snprintf(pedido.descripcion,LARGO_MENSAJE_MAX,"<registro: %i> desbloqueado",pedido.selecRegistro);
                msgsnd(msgid,&pedido,sizeof(struct mensaje)-sizeof(long),0);
            }else if(reg.lock == 0){
                snprintf(pedido.descripcion,LARGO_MENSAJE_MAX,"<registro: %i> nunca tuco lock previo",pedido.selecRegistro);
                msgsnd(msgid,&pedido,sizeof(struct mensaje)-sizeof(long),0);
            }else if(reg.pidBloqueante != pedido.idProc){
                snprintf(pedido.descripcion,LARGO_MENSAJE_MAX,"<registro: %i> Usted no puede desbloquear este registro",pedido.selecRegistro);
                msgsnd(msgid,&pedido,sizeof(struct mensaje)-sizeof(long),0);
            }
        }else if(reg.lock == 1 && reg.pidBloqueante != pedido.idProc){
            pedido.tipo = pedido.idProc; 
            snprintf(pedido.descripcion,LARGO_MENSAJE_MAX,"El registro: %i ya lockeado por pid %d",pedido.selecRegistro,pedido.idProc);
            msgsnd(msgid,&pedido,sizeof(struct mensaje)-sizeof(long),0);
        }else{
            if (strcmp(pedido.descripcion, "eliminar") == 0) {
            // Eliminación de registro
                if (reg.estado == 0) {
                    pedido.tipo = pedido.idProc;
                    pedido.retorno = 0;
                    printf("fallo: el registro: %i esta vacio (estado 0)\n", pedido.selecRegistro);
                    snprintf(pedido.descripcion, LARGO_MENSAJE_MAX, "Fallo: registro vacio");
                    msgsnd(msgid,&pedido,sizeof(struct mensaje)-sizeof(long),0);
                } else {
                    pedido.retorno = 1;
                    reg.estado = 2;
                    memset(pedido.descripcion, '\0', LARGO_MENSAJE_MAX);
                    printf("Server: registro %i, cambiado por cliente %i\n", pedido.selecRegistro, pedido.idProc);
                    snprintf(pedido.descripcion, LARGO_MENSAJE_MAX, "registro %i esta vacio", pedido.selecRegistro);
                    msgsnd(msgid,&pedido,sizeof(struct mensaje)-sizeof(long),0);
                }
            } else if (strcmp(pedido.descripcion, "leer") == 0) {
                // Leer registro
                printf("Server: el cliente:%d, lee registro %i\n", pedido.idProc, pedido.selecRegistro);
                printf("Server: <%d>,<%i>,<%s>\n", pedido.idProc, pedido.selecRegistro, pedido.descripcion);
                pedido.tipo = pedido.idProc;
                pedido.retorno = 1;
                strcpy(pedido.descripcion, reg.descripcion);
                msgsnd(msgid,&pedido,sizeof(struct mensaje)-sizeof(long),0);
            } else{
                reg.estado = 1;
                pedido.tipo = pedido.idProc; 
                pedido.retorno = 1;
                printf("Server: el cliente:%d, cambia registro %i\n", pedido.idProc, pedido.selecRegistro);
                printf("Server: <%d>,<%i>,<escribir: %s>\n", pedido.idProc, pedido.selecRegistro, pedido.descripcion);
                strcpy(reg.descripcion, pedido.descripcion);
                msgsnd(msgid,&pedido,sizeof(struct mensaje)-sizeof(long),0);
            }
        }

        fseek(archivo,sizeof(struct registro)*pedido.selecRegistro,SEEK_SET);
        fwrite(&reg,sizeof(struct registro),1,archivo);
    }
    fclose(archivo);
    return 0;   
}

void handlerSIGINT(int signal){
    printf("se ejecuto la senial %i\n",signal);
    msgctl(msgid,IPC_RMID,0);
    printf("cola de mensajes eliminada\n");
    exit(0);
}