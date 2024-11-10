#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/msg.h>
#define SALIR 6
#define CARACTER_MAX 100

struct solicitud{
    long tipo;
    char descripcion[CARACTER_MAX];
    int selecRegistro;
    pid_t idProc;
    int retorno;
};

void agregarRegistro(struct solicitud*);
void borrarRegistro(struct solicitud*);
void leerRegistro(struct solicitud*);
void lockRegistro(struct solicitud*);
void unlockRegistro(struct solicitud*);

int main(int argc,char **argv){
    struct solicitud pedir;
    pedir.idProc = getpid();
    int msgid = msgget(0xa,IPC_EXCL|0600);
    int eleccion;
    while(1){
        printf("************************\n");
        printf("Bienvenido Sr. cliente\n");
        printf("1) Modificar archivo\n");
        printf("2) Eliminar archivo\n");
        printf("3) Leer archivo\n");
        printf("4) lock registro\n");
        printf("5) unlock registro\n");
        printf("6) Salir\n");
        printf("seleccione: ");
        scanf("%i",&eleccion);
        if(eleccion == SALIR){
            break;
        }
        printf("seleccion el registro que desee (0-999)\n");
        scanf("%i",&pedir.selecRegistro);
        printf("registro seleccionado %i\n",pedir.selecRegistro);
        pedir.tipo = 1;
        getchar();
        switch (eleccion)
        {
            case 1:
                agregarRegistro(&pedir);
                break;
            case 2:
                borrarRegistro(&pedir);
                break;
            case 3:
                leerRegistro(&pedir);
                break;
            case 4:
                lockRegistro(&pedir);
                break;
            case 5:
                unlockRegistro(&pedir);
                break;
        }

        //envio solicitud al server
        int msgsndID = msgsnd(msgid,&pedir,sizeof(struct solicitud)-sizeof(long),0);
        if(msgsndID == -1){
            perror("fallo al escribi la cola de mensajes");
        }

        //recibo respueta del server
        msgrcv(msgid,&pedir,sizeof(struct solicitud),pedir.idProc,0);
        if(strncmp(pedir.descripcion,"fallo",5) == 0){
            printf("Cliente: %s\n",pedir.descripcion);
        }else{
            printf("Cliente: <%i>,<%i>,<%s>\n",pedir.retorno,pedir.selecRegistro,pedir.descripcion);
        }
    }
    return 0;
}

void agregarRegistro(struct solicitud *pedido){
    printf("ingrese descripcion del registro\n");
    fgets(pedido->descripcion,sizeof(pedido->descripcion),stdin);
    pedido->descripcion[strlen(pedido->descripcion)-1] = '\0';
}

void borrarRegistro(struct solicitud *pedido){
    snprintf(pedido->descripcion,CARACTER_MAX,"eliminar");
}

void leerRegistro(struct solicitud *pedido){
    snprintf(pedido->descripcion,CARACTER_MAX,"leer");
}

void lockRegistro(struct solicitud *pedido){
    snprintf(pedido->descripcion,CARACTER_MAX,"lock");
}

void unlockRegistro(struct solicitud *pedido){
    snprintf(pedido->descripcion,CARACTER_MAX,"unlock");
}

