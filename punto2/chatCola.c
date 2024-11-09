#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <signal.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#include <sys/wait.h>



struct mensaje{
    long tipo;
    char texto [256];
};

int main (int argc, char **argv){
    if (argc != 2){
        printf("Forma de uso\n$ chat <nro tipo mensaje>\n");
        return 1;
    }
    //creo cola de mensajes con referencia a la direccion 0xc
    int msgid = msgget(0xc,IPC_CREAT|0777);
    long miTipo= atol (argv[1]);
    pid_t pid = fork();

   if (pid > 0){ //proceso Padre Escritor
        struct mensaje msg;
        msg.tipo=atol(argv[1]);
      do {
        //printf ("%ld>",msg.tipo);
        fgets(msg.texto,256,stdin);
        msgsnd(msgid,&msg,256,0);
        }while (strncmp(msg.texto,"bye",3) != 0);
        wait(0);
    }  

    else{ //Proceso hijo Lector
        long tipo= atol (argv[1]); 
        if (tipo == miTipo){
            if (miTipo == 1)
              tipo = 2;
            else 
              tipo = 1;
        }

        struct mensaje msg; 
        do {
        memset(&msg,0,sizeof(struct mensaje));// Inicializo variable msgcon valores nulos
        msgrcv(msgid,&msg,256,tipo,0);
        printf("Mensaje recibido: %s",msg.texto);

        }while (strncmp(msg.texto,"bye",3)!=0);
        exit(0);
    }

   return(0);
}
