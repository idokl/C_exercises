/****************************************************************************************
 * BS"D
 *
 * Student name: Ido Klein
 * Student ID: 203392964
 * Course Exercise Group: 03
 * Exercise name: ex4
 *
 * GRATITUDE:
 * Thanks to the Roi Yehoshua (instructor of Advanced Programming course) for his example to thread pool
 * implementation in c++ (his code was published in the Piazza in the first semester this year).
 * Thanks to some friends for their advices.
 * Thanks to the instructors for the code files of the Tirgul.
 ****************************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <fcntl.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/sem.h>
#include <stdbool.h>
#include <memory.h>

// make it a 4K shared memory segment
#define SHM_SIZE 4096
#define MEMORY_INCREASING 15

int internal_count;
char buffer[1024];

//struct that is delivered to every thread in the thread pool
typedef struct {
    char* queue;
    //number of characters that were entered to the queue:
    int size;
    //size of the dynamic array:
    int allocatedMemorySize;
    //the index of the character of the next job that has to be executed:
    int firstJobIndex;
    //for access to queue:
    pthread_mutex_t queueLock;
    //for access to the file 203392964.txt:
    pthread_mutex_t fileLock;
    //for access to internal_count:
    pthread_mutex_t countLock;
    //the flag is raised when 'h' character was read:
    int stopFlag;
    //fd of 203392964.txt:
    int fileDescriptor;
} JobQueue;

/****************************************************************************************
 * function name: doJob
 * the input: character - request code
 *            jobQueue - jobQueue struct
 * the output: none
 * the function operation:
 * execute the action that is corresponding to the character (according to the exercise requirements)
 ****************************************************************************************/
void doJob(char character, JobQueue* jobQueue) {
    struct timespec sleepingTime;
    switch (character) {
        //cases A - E: sleep a little bit and increase internal_count
        case 'a':
        case 'A':
            sleepingTime.tv_sec = 0;
            sleepingTime.tv_nsec = (rand() % 91 + 10);
            nanosleep(&sleepingTime, NULL);
            if (0 != pthread_mutex_lock(&(jobQueue->countLock))) {
                perror("lock error\n");
                exit(1);
            }
            internal_count = internal_count + 1;
            if (0 != pthread_mutex_unlock(&(jobQueue->countLock))) {
                perror("unlock error\n");
                exit(1);
            }
            break;
        case 'b':
        case 'B':
            sleepingTime.tv_sec = 0;
            sleepingTime.tv_nsec = (rand() % 91 + 10);
            nanosleep(&sleepingTime, NULL);
            if (0 != pthread_mutex_lock(&(jobQueue->countLock))) {
                perror("lock error\n");
                exit(1);
            }
            internal_count = internal_count + 2;
            if (0 != pthread_mutex_unlock(&(jobQueue->countLock))) {
                perror("unlock error\n");
                exit(1);
            }
            break;
        case 'c':
        case 'C':
            sleepingTime.tv_sec = 0;
            sleepingTime.tv_nsec = (rand() % 91 + 10);
            nanosleep(&sleepingTime, NULL);
            if (0 != pthread_mutex_lock(&(jobQueue->countLock))) {
                perror("lock error\n");
                exit(1);
            }
            internal_count = internal_count + 3;
            if (0 != pthread_mutex_unlock(&(jobQueue->countLock))) {
                perror("unlock error\n");
                exit(1);
            }
            break;
        case 'd':
        case 'D':
            sleepingTime.tv_sec = 0;
            sleepingTime.tv_nsec = (rand() % 91 + 10);
            nanosleep(&sleepingTime, NULL);
            if (0 != pthread_mutex_lock(&(jobQueue->countLock))) {
                perror("lock error\n");
                exit(1);
            }
            internal_count = internal_count + 4;
            if (0 != pthread_mutex_unlock(&(jobQueue->countLock))) {
                perror("unlock error\n");
                exit(1);
            }
            break;
        case 'e':
        case 'E':
            sleepingTime.tv_sec = 0;
            sleepingTime.tv_nsec = (rand() % 91 + 10);
            nanosleep(&sleepingTime, NULL);
            if (0 != pthread_mutex_lock(&(jobQueue->countLock))) {
                perror("lock error\n");
                exit(1);
            }
            internal_count = internal_count + 5;
            if (0 != pthread_mutex_unlock(&(jobQueue->countLock))) {
                perror("unlock error\n");
                exit(1);
            }
            break;
        //case F: write details to 203392964.txt
        case 'f':
        case 'F':
            if (0 != pthread_mutex_lock(&(jobQueue->fileLock))) {
                perror("lock error\n");
                exit(1);
            }
            char string[100];
            sprintf(string, "thread identifier is %u and internal_count is %d\n", (unsigned int)pthread_self(), internal_count);
            write(jobQueue->fileDescriptor, string, strlen(string) * sizeof(char));
            if (0 != pthread_mutex_unlock(&(jobQueue->fileLock))) {
                perror("unlock error\n");
                exit(1);
            }
            break;
    }
}

/****************************************************************************************
 * function name: doJobs
 * the input: jobQueuePtr - pointer to jobQueue struct
 * the output: none
 * the function operation:
 * executed by one of the threads of the thread pool. take jobs from the queue and do them.
 ****************************************************************************************/
void *doJobs(void* jobQueuePtr) {
    char character;
    JobQueue* jobQueue = (JobQueue*)jobQueuePtr;
    //continue until h character will be read and all jobs will be taken
    while(!jobQueue->stopFlag || jobQueue->firstJobIndex < jobQueue->size) {
        pthread_mutex_lock(&(jobQueue->queueLock));
        if(jobQueue->firstJobIndex < jobQueue->size) {
            character = jobQueue->queue[jobQueue->firstJobIndex];
            (jobQueue->firstJobIndex)++;
            pthread_mutex_unlock(&(jobQueue->queueLock));
            doJob(character, jobQueue);
        } else {
            pthread_mutex_unlock(&(jobQueue->queueLock));
        }
    }
    //when h character was read and the threads stop:
    pthread_mutex_lock(&(jobQueue->fileLock));
    char string[100];
    sprintf(string, "thread identifier is %u and internal_count is %d\n", (unsigned int)pthread_self(), internal_count);
    write(jobQueue->fileDescriptor, string, strlen(string) * sizeof(char));
    pthread_mutex_unlock(&(jobQueue->fileLock));
}

/****************************************************************************************
 * function name: main
 * the input: none
 * the output: 0 (if no errors)
 * the function operation:
 * manage the server side (see ex4.pdf requirements)
 ****************************************************************************************/
int main() {
    key_t key, keySem;
    int shmid, semid;
    char *sharedMemory;
    struct sembuf sops[1];
    pthread_t threads[5];
    JobQueue jq;
    JobQueue* jobQueue;
    int i;
    char character;
    void* threadReturn;

    jobQueue = &jq;
    internal_count = 0;

    if ((jobQueue->fileDescriptor = open("203392964.txt", O_CREAT | O_TRUNC | O_WRONLY, 0666)) < 0) {
        // open failed
        perror("open 203392964.txt failed");
        exit(1);
    }

    //make the key:
    if ((key = ftok("203392964.txt", 'k')) == -1) {
        perror("ftok");
        exit(1);
    }
    //connect to (and possibly create) the shared memory:
    if ((shmid = shmget(key, SHM_SIZE, 0644 | IPC_CREAT | IPC_EXCL)) == -1) {
        perror("shmget (probably because the flag IPC_EXCEL - there is shared memory that hasn't been released)");
        exit(1);
    }
    //attach to the shared memory to get a pointer to it:
    sharedMemory = shmat(shmid, NULL, SHM_RDONLY);
    if (sharedMemory == (char *)(-1)) {
        perror("shmat");
        exit(1);
    }

    //initializing queue:
    jobQueue->firstJobIndex = 0;
    jobQueue->size = 0;
    jobQueue->allocatedMemorySize = MEMORY_INCREASING;
    jobQueue->stopFlag = 0;
    jobQueue->queue = (char*)malloc(jobQueue->allocatedMemorySize * sizeof(char));
    if (NULL == jobQueue->queue) {
        perror("malloc failed ");
        exit(-1);
    }
    //initializing semaphores fields:
    sops->sem_num = 0;
    sops->sem_flg = 0;
    //initializing mutexes:
    pthread_mutex_init(&(jobQueue->queueLock), NULL);
    pthread_mutex_init(&(jobQueue->fileLock), NULL);
    pthread_mutex_init(&(jobQueue->countLock), NULL);
    for (i = 0; i < 5; i++) {
        pthread_create(threads + i, NULL, doJobs, jobQueue);
    }
    //create semaphore:
    if ((open("semaphore.txt", O_CREAT | O_TRUNC | O_RDONLY, 0666)) < 0) {
        // open failed
        perror("open semaphore.txt failed");
        exit(1);
    }
    if ((keySem = ftok("semaphore.txt", 's')) == -1) {
        perror("ftok (keySem)");
        exit(1);
    }
    if (-1 == (semid = semget(keySem, 1, IPC_CREAT | IPC_EXCL | 0666))) {
        perror("semget");
        exit(1);
    }

    /*
     * the server frees 3 'resources' by the semaphore in every iteration in order to indicate that
     * the shared memory is available.
     * the client (that reaches it first) allocate 2 resources before it begins to use the shared memory
     * and allocate the third resource when it finishes using it.
     */
    sops->sem_op = 3;
    if (semop(semid, sops, 1) == -1) {
        perror("semop");
        exit(1);
    }
    while(true) {
        //wait to allocating of all 3 'resources' by one of the clients
        sops->sem_op = 0;
        if (semop(semid, sops, 1) == -1) {
            perror("semop");
            exit(1);
        }

        //read character from the shared memory
        character = *sharedMemory;

        //if g was read cancel threads
        if('g' == character || 'G' == character) {
            for (i = 0; i < 5; i++) {
                pthread_cancel(threads[i]);
            }
            break;
        }
        //if h was read join threads
        if ('h' == character || 'H' == character) {
            jobQueue->stopFlag = 1;
            for (i = 0; i < 5; i++) {
                pthread_join(threads[i], &threadReturn);
            }
            break;
        }

        //free the allocated 'resources' to the clients
        sops->sem_op = 3;
        if (semop(semid, sops, 1) == -1) {
            perror("semop");
            exit(1);
        }

        //lock mutex:
        if (0 != pthread_mutex_lock(&(jobQueue->queueLock))) {
            perror("lock error\n");
            exit(1);
        }
        //add character to the queue (resize the queue if needed)
        if(jobQueue->allocatedMemorySize <= jobQueue->size) {
            jobQueue->allocatedMemorySize += MEMORY_INCREASING;
            jobQueue->queue = (char*)realloc(jobQueue->queue, jobQueue->allocatedMemorySize * sizeof(char));
            if (NULL == jobQueue->queue) {
                perror("realloc failed ");
                exit(-1);
            }

        }
        (jobQueue->size)++;
        jobQueue->queue[(jobQueue->size) - 1] = character;
        //unlock mutex:
        if (0 != pthread_mutex_unlock(&(jobQueue->queueLock))) {
            perror("unlock error\n");
            exit(1);
        }
    }

    //after reading 'g' or 'h': write details to 203392964.txt
    sprintf(buffer, "thread identifier is %u and internal_count is %d\n", (unsigned int)pthread_self(), internal_count);
    write(jobQueue->fileDescriptor, buffer, strlen(buffer) * sizeof(char));

    // detach from the shared memory:
    if (shmdt(sharedMemory) == -1) {
        perror("shmdt");
        exit(1);
    }
    // I don't understand why the requirements of the exercise don't require to clean shared memory
    // when 'g' is read. so I destroy the shared memory in this case, too.
    //if (character != 'g' && character != 'G') {
        if (shmctl(shmid,IPC_RMID, NULL) == -1) {
            perror("shmctl failed");
            exit(1);
        }
    //}
    //destroy semaphore
    if (semctl(semid, 0, IPC_RMID, NULL) == -1) {
        perror("shmctl failed");
        exit(1);
    }
    unlink("semaphore.txt");
    //in the clarification that the instructors sent, they gave us the option not removing semaphores and mutexes.
    //So I don't use the function 'pthread_mutex_destroy'. Thanks.

    return 0;
}

