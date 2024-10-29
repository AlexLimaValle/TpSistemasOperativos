#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <string.h>
#include <sys/wait.h>
#include <fcntl.h>


int main(int argc,char *argv[]){
   if(argc < 3){
       printf("se deben ingresar dos varaibles <? fifon1 fifon2 \n");
       exit(0);
   }
   //los dos nombre tanto el escritor como el lector
   char *nombreFifo1 = argv[1];
   char *nombreFifo2 = argv[2];

   pid_t pid = fork();
   if(pid>0){
       char mensaje[40];
       int fd = open(nombreFifo1,O_WRONLY);
       do{
           memset(mensaje,'\0',sizeof(mensaje));
           printf("%s-->>\n",nombreFifo1);
           fgets(mensaje,sizeof(mensaje),stdin);
           mensaje[strcspn(mensaje, "\n")] = 0;
           write(fd,mensaje,strlen(mensaje) + 1);
       }while(strcmp(mensaje,"bye"));
       close(fd);
       wait(0);
   }else{
       char mensaje[40];
       int fd = open(nombreFifo2,O_RDONLY);
       do{
           memset(mensaje,'\0',sizeof(mensaje));
           read(fd,mensaje,sizeof(mensaje));
           printf("%s-->> %s\n",nombreFifo2,mensaje);
       }while(strcmp(mensaje,"bye"));
       close(fd);
   }
   return 0;
}
