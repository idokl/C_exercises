/****************************************************************************************
 * BS"D
 *
 * Student name: Ido Klein
 * Student ID: 203392964
 * Course Exercise Group: 03
 * Exercise name: ex4
 ****************************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <stdbool.h>
#include <errno.h>

#define SHM_SIZE 4096

/****************************************************************************************
 * function name: main
 * the input: none
 * the output: 0 (if no errors)
 * the function operation:
 * manage the client side (get input from the user and send them to the server)
 ****************************************************************************************/
int main() {
    key_t key, keySem;
    //shared memory id, semaphore id:
    int shmid, semid;
    char *sharedMemory;
    struct sembuf sops[1];
    char character;


    //make the key:
    if ((key = ftok("203392964.txt", 'k')) == -1) {
        perror("ftok");
        exit(1);
    }
    //connect to the segment:
    if ((shmid = shmget(key, SHM_SIZE, 0644)) == -1) {
        perror("shmget");
        exit(1);
    }
    //attach to the segment to get a pointer to it:
    sharedMemory = shmat(shmid, NULL, 0);
    if (sharedMemory == (char *)(-1)) {
        perror("shmat");
        exit(1);
    }

    //initializing sembuf fields:
    sops->sem_num = 0;
    sops->sem_flg = 0;

    //make the key to the semaphore:
    if ((keySem = ftok("semaphore.txt", 's')) == -1) {
        perror("ftok (keySem)");
        exit(1);
    }

    if (-1 == (semid = semget(keySem, 0, 0666))) {
        perror("semget");
        exit(1);
    }

    //ask the user to enter characters of the jobs that he wants to perform in the server
    while(true) {
        printf("Please enter request code\n");
        scanf(" %c", &character);

        //i character - terminating the client
        if ('i' == character || 'I' == character) {
            break;
        }

        /*
         * the server frees 3 'resources' by the semaphore in every iteration in order to indicate that
         * the shared memory is available.
         * the client (that reaches it first) allocate 2 resources before it begins to use the shared memory
         * and allocate the third resource when it finishes using it.
         */
        sops->sem_op = -2;
        if (semop(semid, sops, 1) == -1) {
            //if errno == 22 or 43, it indicates that the server terminated. so the client has to terminate, too.
            if (22 == errno || 43 == errno) {
                break;
            }
            perror("semop");
            exit(1);
        }

        //write the character to the shared memory:
        sprintf(sharedMemory, "%c", character);

        //allocate the third resource
        sops->sem_op = -1;
        if (semop(semid, sops, 1) == -1) {
            if (22 == errno || 43 == errno) {
                break;
            }
            perror("semop");
            exit(1);
        }
    } //end of the loop

    //detach from the shared memory:
    if (shmdt(sharedMemory) == -1) {
        perror("shmdt");
        exit(1);
    }

    return 0;
}
