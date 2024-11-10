#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/wait.h>

void P(int semid, int sem);
void V(int semid, int sem);

int main(int argc, char **argv) {
    if (argc != 2) {
        printf("Forma de uso: $ chat <nro tipo mensaje>\n");
        return 1;
    }
    
    int nro = atoi(argv[1]);

    // Creo memoria compartida
    int shmid1 = shmget(0xd, 512, 0777 | IPC_CREAT);
    if (shmid1 == -1) {
        perror("Error al crear/obtener memoria compartida");
        exit(1);
    }

    char *msg1 = shmat(shmid1, 0, 0); // Asignar el segmento de memoria compartida
    if (msg1 == (char *)-1) {
        perror("Error de asignación de memoria compartida");
        exit(1);
    }

    char *msg2 = msg1 + 256; // Segundo bloque de memoria para el segundo proceso

    // Inicializar los buffers de mensajes
    memset(msg1, 0, 256); 
    memset(msg2, 0, 256);

    // Crear semáforos
    int semid = semget(0xd, 2, IPC_CREAT | IPC_EXCL | 0600);
    if (semid == -1) {
        if ((semid = semget(0xd, 2, 0600)) == -1) {
            perror("Error al crear semáforo");
            exit(1);
        }
    }
    
    // Inicializar los semáforos
   semctl(semid, 0, SETVAL, 0);
   semctl(semid, 1, SETVAL, 1);

    pid_t pid = fork(); 

    if (pid > 0) { // Código padre (escritor)
        char buffer[256];
        char *miMsg;

        if (nro == 1) {
            miMsg = msg1;
        } else {
            miMsg = msg2;
        }
           
        do {
            P(semid, 0); // Esperar para escribir
            fgets(buffer, 256, stdin);
            strcpy(miMsg, buffer); // Escribir el mensaje
            V(semid, 1); // Notificar al hijo que puede leer

        } while (strncmp(buffer, "bye", 3) != 0);

        wait(0); // Esperar a que el hijo termine

        // Desasignar memoria compartida
        shmdt(msg1); 
        shmdt(msg2); 

        //elimino semaforos
        semctl(semid, 0, IPC_RMID, 0);
        semctl(semid, 1, IPC_RMID, 0);

        //elimino memoria compartida
        shmctl(shmid1,IPC_RMID,0);
        printf ("Programa Terminado\n");
        exit(0);

    } else { // Código hijo (lector)
        char buffer[256];
        char *miMsg;
        int otroNro;
        if (nro == 1) {
            miMsg = msg2;
            otroNro = 2;
        } else {
            miMsg = msg1;
            otroNro=1;
        }

        do {
            P(semid, 1);// Esperar para leer
            strcpy(buffer, miMsg);// Leer el mensaje             
            if(buffer[0] == '\0'){ 
                V(semid,0);
            }else{
            printf("P%d:%s\n",otroNro,buffer); // Mostrar el mensaje
            memset(miMsg, 0, 256); // Limpiar el mensaje en memoria compartida
            V(semid, 0);// Notifico al padre que puede escribir
            } 
        } while (strncmp(buffer, "bye", 3) != 0);

        shmdt(msg1); // Desasignar memoria compartida
        shmdt(msg2);
        exit(0);
    }

    return 0;
}
//Funcion P y V 
void P(int semid, int sem) {
    struct sembuf buf;
    buf.sem_num = sem;
    buf.sem_op = -1;
    buf.sem_flg = 0;
    semop(semid, &buf, 1);
}

void V(int semid, int sem) {
    struct sembuf buf;
    buf.sem_num = sem;
    buf.sem_op = 1;
    buf.sem_flg = 0;
    semop(semid, &buf, 1);
}
