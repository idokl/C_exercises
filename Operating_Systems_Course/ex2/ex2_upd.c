/****************************************************************************************
 * BS"D
 * Student name: ido klein
 * Student ID: 203392964
 * Course Exercise Group: 03
 * Exercise name: ex2 - ex2_upd
 * GRATITUDE:
 * - Thanks to friends for some good ideas about the recommended way to implement some abilities.
 * - Thanks to friend that showed to me this website:
 *   https://www.leaseweb.com/labs/2014/03/text-mode-2048-game-c-algorithm-explained/
 *   that introduces pseudo code to the algorithm of 2048.
 *   (although that I chose another way to implement it and I think that my algorithm is better...)
 * - Thanks to the "google doc" file that contains answers of the instructors to the students.
 *   (although that I think the new requirement to "Game Over" or "Congaratulations" message is
 *   redundant).
 * - Thanks to the tirgul presentations.
 ****************************************************************************************/

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>

typedef enum {SUCCESS, NO_2_NULL_SQUARES} Result;
int gameBoard[4][4];
pid_t pid1;

/****************************************************************************************
 * function name: setNewWaitingTime
 * the input: none
 * the output: none
 * the function operation:
 * set random integer of seconds (1-5) and request SIGALARM after this time will pass
 ****************************************************************************************/
void setNewWaitingTime() {
    int waitingTime;
    waitingTime = rand() % 5 + 1;
    alarm(waitingTime);
}

/****************************************************************************************
 * function name: zeroing
 * the input:
 * @param board
 * the output: none
 * the function operation:
 * zeroing the given board
 ****************************************************************************************/
void zeroing(int board[4][4]) {
    int i, j;
    for(i = 0; i < 4; i++) {
        for(j = 0; j < 4; j++) {
            board[i][j] = 0;
        }
    }
}

/****************************************************************************************
 * function name: printBoardInLineFormat
 * the input:
 * @param board
 * the output: none
 * the function operation:
 * print the given board in line format
 ****************************************************************************************/
void printBoardInLineFormat(int board[4][4]) {
    int lineLength;
    char line[100];
    lineLength = sprintf(line,"%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n",
                         board[0][0],board[0][1],board[0][2],board[0][3],
                         board[1][0],board[1][1],board[1][2],board[1][3],
                         board[2][0],board[2][1],board[2][2],board[2][3],
                         board[3][0],board[3][1],board[3][2],board[3][3]);
    write(STDOUT_FILENO,line,lineLength);
}

/****************************************************************************************
 * function name: printBoardInLineFormat
 * the input:
 * @param board
 * the output:
 * @return SUCCESS if there are 2 null squares in the board that were set to 2,
 *         NO_2_NULL_SQUARES otherwise.
 * the function operation:
 * find 2 null squares in the board (randomally) and change their value from 0 to 2.
 */
Result set2SquaresTo2(int board[4][4]) {
    int randSquareIdx1, randSquareIdx2;
    int i, nullSquaresCounter = 0;

    //check existence of 2 null squares in board
    for (i=0;i<16;i++) {
        if (board[0][i] == 0)
            nullSquaresCounter++;
    }
    if (nullSquaresCounter < 2)
        return NO_2_NULL_SQUARES;

    do {
        randSquareIdx1 = rand() % 16;
    } while (board[0][randSquareIdx1] != 0);

    do {
        randSquareIdx2 = rand() % 16;
    } while (board[0][randSquareIdx2] != 0 || randSquareIdx1 == randSquareIdx2);

    board[0][randSquareIdx1] = 2;
    board[0][randSquareIdx2] = 2;
    return SUCCESS;
}

//slide squares from s1 toward s4
/****************************************************************************************
 * function name: slideForward
 * the input:
 * @param s1 - pointer to the value of the first square in the row/colunn.
 *             It is pushed towards s2 (and towards s3 and s4).
 * @param s2 - second in the row/colunn. It is pushed towards s3 (and s4)
 * @param s3 - third in the row/colunn. It is pushed towards s4.
 * @param s4 - the last in the row/colunn. s1, s2 and s3 are pushed to its side.
 * the output: none
 * the function operation:
 * slide square according to 2048 game rules.
 * example:
 * if the values of these squares were: s1 = 2, s2 = 2, s3 = 0, s4 = 8,
 * then change them to be: s1 = 0, s2 = 0, s3 = 4, s4 = 8.
 ****************************************************************************************/
void slideForward(int* s1, int* s2, int* s3, int* s4) {
    //part 1 - moving
    //move every square to the next square, if available (first time)
    if (*s4 == 0) {
        *s4 = *s3;
        *s3 = 0;
    }
    if (*s3 == 0) {
        *s3 = *s2;
        *s2 = 0;
    }
    if (*s2 == 0) {
        *s2 = *s1;
        *s1 = 0;
    }
    //move every square to the next square, if available (second time)
    if (*s4 == 0) {
        *s4 = *s3;
        *s3 = 0;
    }
    if (*s3 == 0) {
        *s3 = *s2;
        *s2 = 0;
    }
    //move every square to the next square, if available (third time)
    if (*s4 == 0) {
        *s4 = *s3;
        *s3 = 0;
    }

    //part 2 - combining:
    //example: original board: 4 2 1 1
    if (*s4 == *s3) {
        *s4 = *s4 + *s3;
        *s3 = *s2;
        *s2 = *s1;
        *s1 = 0;
        //example: original board was: 2 2 1 1
        if (*s3 == *s2) {
            *s3 = *s3 + *s2;
            *s2 = 0;
        }
        return;
    }
    //example: original board: 0 2 2 1
    if (*s3 == *s2) {
        *s3 = *s3 + *s2;
        *s2 = *s1;
        *s1 = 0;
        return;
    }
    //example: original board: 4 4 2 1
    if (*s2 == *s1) {
        *s2 = *s2 + *s1;
        *s1 = 0;
        return;
    }
}

/****************************************************************************************
 * function name: moveUp
 * the input:
 * @param board - board of 2048 game
 * the output: none
 * the function operation:
 * slide squares (of every column) up.
 ****************************************************************************************/
void moveUp(int board[4][4]) {
    slideForward(&(board[3][0]),&(board[2][0]),&(board[1][0]),&(board[0][0]));
    slideForward(&(board[3][1]),&(board[2][1]),&(board[1][1]),&(board[0][1]));
    slideForward(&(board[3][2]),&(board[2][2]),&(board[1][2]),&(board[0][2]));
    slideForward(&(board[3][3]),&(board[2][3]),&(board[1][3]),&(board[0][3]));
}

/****************************************************************************************
 * function name: moveDown
 * the input:
 * @param board - board of 2048 game
 * the output: none
 * the function operation:
 * slide squares (of every column) down.
 ****************************************************************************************/
void moveDown(int board[4][4]) {
    slideForward(&(board[0][0]),&(board[1][0]),&(board[2][0]),&(board[3][0]));
    slideForward(&(board[0][1]),&(board[1][1]),&(board[2][1]),&(board[3][1]));
    slideForward(&(board[0][2]),&(board[1][2]),&(board[2][2]),&(board[3][2]));
    slideForward(&(board[0][3]),&(board[1][3]),&(board[2][3]),&(board[3][3]));
}

/****************************************************************************************
 * function name: moveRight
 * the input:
 * @param board - board of 2048 game
 * the output: none
 * the function operation:
 * slide squares (of every row) to the right.
 ****************************************************************************************/
void moveRight(int board[4][4]) {
    slideForward(&(board[0][0]),&(board[0][1]),&(board[0][2]),&(board[0][3]));
    slideForward(&(board[1][0]),&(board[1][1]),&(board[1][2]),&(board[1][3]));
    slideForward(&(board[2][0]),&(board[2][1]),&(board[2][2]),&(board[2][3]));
    slideForward(&(board[3][0]),&(board[3][1]),&(board[3][2]),&(board[3][3]));
}

/****************************************************************************************
 * function name: moveLeft
 * the input:
 * @param board - board of 2048 game
 * the output: none
 * the function operation:
 * slide squares (of every row) to the left.
 ****************************************************************************************/
void moveLeft(int board[4][4]) {
    slideForward(&(board[0][3]),&(board[0][2]),&(board[0][1]),&(board[0][0]));
    slideForward(&(board[1][3]),&(board[1][2]),&(board[1][1]),&(board[1][0]));
    slideForward(&(board[2][3]),&(board[2][2]),&(board[2][1]),&(board[2][0]));
    slideForward(&(board[3][3]),&(board[3][2]),&(board[3][1]),&(board[3][0]));
}

/****************************************************************************************
 * function name: handlerOfAlarm
 * the input:
 * @param signum - (will be SIGALARM)
 * the output: none
 * the function operation:
 * handle SIGALARM according to the requirements of ex2
 ****************************************************************************************/
void handlerOfAlarm(int signum) {
    //if game over
    if (set2SquaresTo2(gameBoard) == NO_2_NULL_SQUARES) {
        exit(0);
    }
    //else
    setNewWaitingTime();
    printBoardInLineFormat(gameBoard);
    kill(pid1, SIGUSR1);
}

/****************************************************************************************
 * function name: handlerOfSigint
 * the input:
 * @param signum - (will be SIGINT)
 * the output: none
 * the function operation:
 * handle SIGINT according to the requirements of ex2, i.e. exit.
 ****************************************************************************************/
void handlerOfSigint(int signum) {
    exit(0);
}

/****************************************************************************************
 * function name: main
 * the input:
 * @param argc - number of args in the argv array - we expected to 2
 * @param argv - array of args:
 *        arg[0] - should be the file path, e.g. ./ex2_upd.out
 *        argv[1] - should be the process id of ./ex2_inp.out running
 * the output: 0 (if no errors)
 * the function operation:
 * manage 2048 game according to the requirements of ex2.pdf
 ****************************************************************************************/
int main(int argc, char* argv[]) {
    char pressedKey;
    sigset_t blockMaskToAlarm, blockMaskToSigint;
    struct sigaction actToAlarm, actToSigint;

    if (argc < 2) {
        perror("the program expect to argument - process id of ex2_inp.out process\n");
        exit(-1);
    }
    pid1 = atoi(argv[1]);

    //sigaction to SIGALRM
    sigfillset(&blockMaskToAlarm);
    actToAlarm.sa_mask = blockMaskToAlarm;
    actToAlarm.sa_handler = handlerOfAlarm;
    actToAlarm.sa_flags = 0;
    if (sigaction(SIGALRM, &actToAlarm, NULL) == -1) {
        perror("error: sigaction\n");
        exit(-1);
    }

    //sigaction to SIGINT
    sigfillset(&blockMaskToSigint);
    actToSigint.sa_mask = blockMaskToSigint;
    actToSigint.sa_handler = handlerOfSigint;
    actToAlarm.sa_flags = 0;
    if (sigaction(SIGINT, &actToSigint, NULL) == -1) {
        perror("error: sigaction\n");
        exit(-1);
    }

    //beginning of the program:
    zeroing(gameBoard);
    setNewWaitingTime();
    set2SquaresTo2(gameBoard);
    printBoardInLineFormat(gameBoard);
    kill(pid1, SIGUSR1);

    //handle keyboard pressing
    while (1) {
        system("stty cbreak -echo");
        pressedKey = getchar();
        system("stty cooked echo");
        switch (pressedKey) {
            case 'A':
                moveLeft(gameBoard);
                break;
            case 'D':
                moveRight(gameBoard);
                break;
            case 'W':
                moveUp(gameBoard);
                break;
            case 'X':
                moveDown(gameBoard);
                break;
            case 'S':
                zeroing(gameBoard);
                set2SquaresTo2(gameBoard);
                break;
        }

        switch (pressedKey) {
            case 'A':
            case 'D':
            case 'W':
            case 'X':
            case 'S':
                setNewWaitingTime();
                printBoardInLineFormat(gameBoard);
                kill(pid1, SIGUSR1);
                break;
        }
    }
    return 0;
}