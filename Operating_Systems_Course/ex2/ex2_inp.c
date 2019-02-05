/****************************************************************************************
 * BS"D
 * Student name: ido klein
 * Student ID: 203392964
 * Course Exercise Group: 03
 * Exercise name: ex2 - ex2_inp
 ****************************************************************************************/

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>

#define SPACE_STR "    "

/****************************************************************************************
 * function name: printLineAsMatrix
 * the input:
 * @param line - 16 numbers separated by commas (line format)
 * @param ints - ints array that this function will fill with the numbers from the line
 * @param matrix - string of numbers-matrix that corresponding to the given line
 * the output: none
 * the function operation:
 * convert line format to matrix format and print the matrix
 * example: if "line" = "2,4,0,0,2,2,0,16,0,0,4,0,16,0,16,0\n",
 * then after the function operation, "ints" will be array with
 * these numbers and "matrix" will be
 * | 0002 | 0004 |      |      |
 * | 0002 | 0002 |      | 0016 |
 * |      |      | 0004 |      |
 * | 0016 |      | 0016 |      |
  ****************************************************************************************/
void printLineAsMatrix(char *line, int *ints, char *matrix) {
    char chars[100];
    char numberStr[5];
    char* charsPtr = chars;
    char* linePtr = line;
    char* numberHead = chars;
    char gameOverMessage[14] = "Game Over!\n";
    char congratulationsMessage[20] = "Congratulations!\n";
    int i, j;
    int emptySquareExistsFlag = 0;
    int winFlag = 0;

    //parse line to ints array
    for(i = 0; i < 15; i++) {
        while(*linePtr != ',') {
            *charsPtr++ = *linePtr++;
        }
        *charsPtr++ = 0;
        linePtr++;
        ints[i] = atoi(numberHead);
        numberHead = charsPtr;
    }

    while(*linePtr != '\n') {
        *charsPtr++ = *linePtr++;
    }
    *charsPtr = 0;
    ints[15] = atoi(numberHead);

    //check whether it is the end of the game
    //check losing
    for(i=0, emptySquareExistsFlag = 0 ; (!emptySquareExistsFlag) && (i < 16) ;i++) {
        emptySquareExistsFlag = !(ints[i]);
    }
    //check winning
    for(i=0; (i<16) && (ints[i]!=2048); i++) {
    }
    if(ints[i] == 2048) {
        winFlag = 1;
    }

    //parse ints array to matrix
    strcpy(matrix,"");
    for(i=0;i<4;i++) {
        for(j=0;j<4;j++) {
            strcat(matrix,"| ");
            if(ints[4*i+j] == 0)
                strcat(matrix,SPACE_STR);
            else {
                sprintf(numberStr, "%4.4d", ints[4*i+j]);
                strcat(matrix,numberStr);
            }
            strcat(matrix," ");
        }
        strcat(matrix,"|\n");
    }
    //print the matrix
    write(STDOUT_FILENO,matrix,120);

    //if no null squares - print "Game Over!" and terminate
    /*(this new requirement isn't consistent with the old requirements.
    therefore, the case of single null square in the matrix isn't handled
    (no requirement to this case)) */
    if (!emptySquareExistsFlag) {
        write(STDOUT_FILENO,gameOverMessage,14);
        exit(0);
    }
    if(winFlag){
        write(STDOUT_FILENO,congratulationsMessage,20);
        exit(0);
    }
}

/****************************************************************************************
 * function name: handlerOfSigusr1
 * the input:
 * @param signum - (will be SIGUSR1)
 * the output: none
 * the function operation:
 * handle SIGUSR1 according to the requirements of ex2
 ****************************************************************************************/
void handlerOfSigusr1(int signum) {
    int ints2[16];
    char line2[100];
    char matrix2[120];

    read(STDIN_FILENO,line2,100);
    printLineAsMatrix(line2, ints2, matrix2);
}

/****************************************************************************************
 * function name: handlerOfSigint
 * the input:
 * @param signum - (will be SIGINT)
 * the output: none
 * the function operation:
 * handle SIGINT according to the requirements of ex2
 ****************************************************************************************/
void handlerOfSigint(int signum) {
    char byeMessage[10] = "BYE BYE\n";
    write(STDOUT_FILENO,byeMessage,10);
    exit(0);
}

/****************************************************************************************
 * function name: main
 * the input: none
 * the output: 0 (if no errors)
 * the function operation:
 * create the wanted responses to the signals SIGUSR1 and SIGINT according to ex2 requirements.
 ****************************************************************************************/
int main() {
    sigset_t blockMask, blockMaskBye;
    struct sigaction act, actBye;

    //sigaction of SIGUSR1 - (the handler reads line and prints it in matrix format)
    sigfillset(&blockMask);
    sigdelset(&blockMask, SIGINT);

    act.sa_mask = blockMask;
    act.sa_handler = handlerOfSigusr1;
    act.sa_flags = 0;

    if (sigaction(SIGUSR1, &act, NULL) == -1) {
        perror("error: sigaction\n");
        exit(-1);
    }

    //sigaction of SIGINT - (the handler prints BYE BYE)
    sigfillset(&blockMaskBye);
    actBye.sa_mask = blockMaskBye;
    actBye.sa_handler = handlerOfSigint;
    actBye.sa_flags = 0;

    if (sigaction(SIGINT, &actBye, NULL) == -1) {
        perror("error: sigaction\n");
        exit(-1);
    }

    while (1) {
        sleep(2);
    }
    return 0;
}