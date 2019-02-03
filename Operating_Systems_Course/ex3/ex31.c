/****************************************************************************************
 * BS"D
 *
 * Student name: ido klein
 * Student ID: 203392964
 * Course Exercise Group: 03
 * Exercise name: ex3
 *
 * GRATITUDE:
 * Thanks to friends for ideas of ways to implement some requirements (like checking when 
 * the second client wrote its pid in the fifo (thanks to Raz) or checking the format of the
 * input (thanks to Elisheva)).
 *
 * COMMENT: 
 * I think there were some exaggerated changes of this exercise requirements after
 * it was published. In the original exercise there wasn't requirement to check existence
 * of potential valid steps in order to check game ending. and there wasn't a requirement
 * to check the format of the input. I implemented it, but I think these additions of
 * requirements were unnecessary. They don't relevant to the learned material of the course
 * and they are not so important.
 * Furthermore, I noticed that there are some strange requirements to this exercise:
 * 1. the second player can't see the first step of the first player, so how can he know
 * where should he put his token? 2. In the 'google doc' file, we were required to write
 * "Please choose another squa" in case of invalid square format, but the word "squa"
 * isn't correct word...
 * I think that these mistakes are legitimate, it is just an exercise. But, in my humble 
 * opinion, I think that addition of requirements after publishing of exercise is not correct.
 * For this reason, I think that some of the students didn't notice to some of these 
 * things, and I hope that you will consider it and will not decrease their grade. Thanks.
 ****************************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <signal.h>

#define SHM_SIZE 1024  /* make it a 1K shared memory segment */

typedef enum {NONE = 0, WHITE = 1, BLACK = 2} Color;
typedef enum {INVALID, VALID} Validity;
typedef enum {FALSE, TRUE} Bool;

typedef struct {
    //use: cell[row][col] = NONE / WHITE / BLACK
    Color cell[8][8];
} Board;

typedef struct {
    Color color;
    int row;
    int col;
} Step;

//read one step from the shared memory, if possible
Step readSharedMemory(char** dataPtr) {
    Step step;
    char colorChar = (*dataPtr)[0];
    char colNum = (*dataPtr)[1];
    char rowNum = (*dataPtr)[2];

    switch(colorChar) {
        case '\0':
            //no new data to read
            step.color = NONE;
            step.col = -1;
            step.row = -1;
            return step;
        case 'b':
            step.color = BLACK;
            break;
        case 'w':
            step.color = WHITE;
            break;
        default:
            printf("debug: mistake in readSharedMemory()");
    }
    step.col = colNum - '0';
    step.row = rowNum - '0';
    *dataPtr = *dataPtr + 4;
    return step;
}

void initializeBoard(Board* board) {
    int i, j;
    for(i = 0; i < 8; i++) {
        for(j = 0; j < 8; j++) {
            board->cell[i][j] = NONE;
        }
    }
    board->cell[3][3] = BLACK;
    board->cell[4][4] = BLACK;
    board->cell[3][4] = WHITE;
    board->cell[4][3] = WHITE;
}

//rowOrColIndex - row index or column index of cell in the Reversi board
Validity increaseIndex(int* rowOrColIndex) {
    if(*rowOrColIndex < 7) {
        (*rowOrColIndex)++;
        return VALID;
    } else {
        return INVALID;
    }
}

Validity decreaseIndex(int* rowOrColIndex) {
    if(*rowOrColIndex > 0) {
        (*rowOrColIndex)--;
        return VALID;
    } else {
        return INVALID;
    }
}

Validity dontChangeIndex(int* rowOrColIndex) {
    return VALID;
}

/*
 * tokenColor - color of the token that we want to put in a square on the board
 * row - row of empty square that we want to put the token in
 * col - column of empty square that we want to put the token in
 */
Validity checkAbilityToInvertTokens(Board* board, Color tokenColor, int col, int row,
                                    Validity (*nextColFunc)(int*), Validity (*nextRowFunc)(int*)) {
    Color oppositeColor = 3 - tokenColor;
    //check existence of token with the opposite color in the near square
    Validity accessibilityOfRow = nextRowFunc(&row);
    Validity accessibilityOfCol = nextColFunc(&col);
    if (INVALID == accessibilityOfRow || INVALID == accessibilityOfCol || oppositeColor != board->cell[row][col]) {
        return INVALID;
    }
    //check existence of token with the color of the addaed token in the continuation of the line
    accessibilityOfRow = nextRowFunc(&row);
    accessibilityOfCol = nextColFunc(&col);
    while (VALID == accessibilityOfRow && VALID == accessibilityOfCol && oppositeColor == board->cell[row][col]) {
        accessibilityOfRow = nextRowFunc(&row);
        accessibilityOfCol = nextColFunc(&col);
    }
    if (INVALID == accessibilityOfRow || INVALID == accessibilityOfCol) {
        return INVALID;
    } else if (NONE == board->cell[row][col]) {
        return INVALID;
    } else { //(tokenColor ==  board->cell[row][col])
        return VALID;
    }
}

Validity checkAbilityToInvertTokenInAnyDirection(Board* board, Color tokenColor, int col, int row) {
    //check ability to invert tokens in the directions: down, up, right, left
    if (VALID == checkAbilityToInvertTokens(board, tokenColor, col, row, dontChangeIndex, increaseIndex)) {
        return VALID;
    }
    if (VALID == checkAbilityToInvertTokens(board, tokenColor, col, row, dontChangeIndex, decreaseIndex)) {
        return VALID;
    }
    if (VALID == checkAbilityToInvertTokens(board, tokenColor, col, row, increaseIndex, dontChangeIndex)) {
        return VALID;
    }
    if (VALID == checkAbilityToInvertTokens(board, tokenColor, col, row, decreaseIndex, dontChangeIndex)) {
        return VALID;
    }
    //check ability to invert tokens in diagonal directions
    if (VALID == checkAbilityToInvertTokens(board, tokenColor, col, row, increaseIndex, increaseIndex)) {
        return VALID;
    }
    if (VALID == checkAbilityToInvertTokens(board, tokenColor, col, row, increaseIndex, decreaseIndex)) {
        return VALID;
    }
    if (VALID == checkAbilityToInvertTokens(board, tokenColor, col, row, decreaseIndex, increaseIndex)) {
        return VALID;
    }
    if (VALID == checkAbilityToInvertTokens(board, tokenColor, col, row, decreaseIndex, decreaseIndex)) {
        return VALID;
    }
    return INVALID;
}

//(use this function only if checkAbilityToInvertTokens returned VALID)
void invertTokens(Board* board, Color addedTokenColor, int col, int row,
                  Validity (*nextColFunc)(int*), Validity (*nextRowFunc)(int*)) {
    Color oppositeColor = 3 - addedTokenColor;
    for (nextColFunc(&col), nextRowFunc(&row); board->cell[row][col] == oppositeColor;
         nextColFunc(&col), nextRowFunc(&row)) {
        board->cell[row][col] = addedTokenColor;
    }
}

void putToken(Board *board, Color tokenColor, int col, int row) {
    //invert tokens in the directions: down, up, right, left
    if (VALID == checkAbilityToInvertTokens(board, tokenColor, col, row, dontChangeIndex, increaseIndex)) {
        invertTokens(board, tokenColor, col, row, dontChangeIndex, increaseIndex);
    }
    if (VALID == checkAbilityToInvertTokens(board, tokenColor, col, row, dontChangeIndex, decreaseIndex)) {
        invertTokens(board, tokenColor, col, row, dontChangeIndex, decreaseIndex);
    }
    if (VALID == checkAbilityToInvertTokens(board, tokenColor, col, row, increaseIndex, dontChangeIndex)) {
        invertTokens(board, tokenColor, col, row, increaseIndex, dontChangeIndex);
    }
    if (VALID == checkAbilityToInvertTokens(board, tokenColor, col, row, decreaseIndex, dontChangeIndex)) {
        invertTokens(board, tokenColor, col, row, decreaseIndex, dontChangeIndex);
    }
    //invert tokens in diagonal directions
    if (VALID == checkAbilityToInvertTokens(board, tokenColor, col, row, increaseIndex, increaseIndex)) {
        invertTokens(board, tokenColor, col, row, increaseIndex, increaseIndex);
    }
    if (VALID == checkAbilityToInvertTokens(board, tokenColor, col, row, increaseIndex, decreaseIndex)) {
        invertTokens(board, tokenColor, col, row, increaseIndex, decreaseIndex);
    }
    if (VALID == checkAbilityToInvertTokens(board, tokenColor, col, row, decreaseIndex, increaseIndex)) {
        invertTokens(board, tokenColor, col, row, decreaseIndex, increaseIndex);
    }
    if (VALID == checkAbilityToInvertTokens(board, tokenColor, col, row, decreaseIndex, decreaseIndex)) {
        invertTokens(board, tokenColor, col, row, decreaseIndex, decreaseIndex);
    }
    //put the added token
    board->cell[row][col] = tokenColor;
}

Bool isGameFinished(Board *board, Color colorOfCurrentTurn) {
    int r, c;
    Bool thereIsEmptySquare = FALSE;
    Bool thereIsBlackToken = FALSE;
    Bool thereIsWhiteToken = FALSE;
    Bool thereIsValidStep = FALSE;
    for(r = 0; r < 8; r++) {
        for(c = 0; c < 8; c++) {
            switch(board->cell[r][c]) {
                case NONE:
                    thereIsEmptySquare = TRUE;
                    if (FALSE == thereIsValidStep &&
                        TRUE == checkAbilityToInvertTokenInAnyDirection(board, colorOfCurrentTurn, c, r)) {
                        thereIsValidStep = TRUE;
                    }
                    break;
                case BLACK:
                    thereIsBlackToken = TRUE;
                    break;
                case WHITE:
                    thereIsWhiteToken = TRUE;
                    break;
            }
            if (thereIsEmptySquare && thereIsBlackToken && thereIsWhiteToken && thereIsValidStep) {
                return FALSE;
            }
        }
    }
    return TRUE;
}

Color whoDidWin(Board *board) {
    int r, c;
    int blackTokensNum = 0;
    int whiteTokensNum = 0;
    for(r = 0; r < 8; r++) {
        for(c = 0; c < 8; c++) {
            switch(board->cell[r][c]) {
                case NONE:
                    //do nothing
                    break;
                case BLACK:
                    blackTokensNum++;
                    break;
                case WHITE:
                    whiteTokensNum++;
                    break;
            }
        }
    }
    if (blackTokensNum > whiteTokensNum)
        return BLACK;
    else if (whiteTokensNum > blackTokensNum)
        return WHITE;
    else
        return NONE;
}

//follow the game by the shared memory in order to know who did win and when...
void followTheGame(char *currentData) {
    Board board;
    Step currentStep;
    Color currentTurnColor = BLACK;
    Color winnerColor;
    Bool gameFinished = FALSE;

    initializeBoard(&board);
    while(FALSE == gameFinished) {
        currentStep = readSharedMemory(&currentData);
        while (currentStep.color != currentTurnColor) {
            sleep(1);
            currentStep = readSharedMemory(&currentData);
        }
        putToken(&board, currentStep.color, currentStep.col, currentStep.row);
        currentTurnColor = 3 - currentTurnColor;
        gameFinished = isGameFinished(&board, currentTurnColor);
    }
    printf("GAME OVER\n");
    winnerColor = whoDidWin(&board);
    if (BLACK == winnerColor) {
        printf("Winning player: Black\n");
    } else if (WHITE == winnerColor) {
        printf("Winning player: White\n");
    } else {
        printf("No winning player\n");
    }
}

int main(int argc, char *argv[])
{
    key_t key;
    int shmid;
    char *data;
    int file;

    //unlink("fifo_clientTOserver");
    file = mkfifo("fifo_clientTOserver",0666);
    if(file<0) {
        perror("Unable to create a fifo");
        exit(-1);
    }

    //make the key:
    if ((key = ftok("ex31.c", 'k')) == -1) {
        perror("ftok");
        exit(1);
    }
    //connect to (and possibly create) the segment:
    if ((shmid = shmget(key, SHM_SIZE, 0644 | IPC_CREAT | IPC_EXCL)) == -1) {
        perror("shmget (probably because the flag IPC_EXCEL - there is shared memory that hasn't been released)");
        exit(1);
    }
    //attach to the segment to get a pointer to it:
    data = shmat(shmid, NULL, SHM_RDONLY);
    if (data == (char *)(-1)) {
        perror("shmat");
        exit(1);
    }

    int fd_read;
    pid_t firstPid, secondPid = 0;

    //waiting for first client pid
    fd_read=open("fifo_clientTOserver",O_RDONLY);
    read(fd_read,&firstPid,sizeof(pid_t));
    close(fd_read);

    //waiting for second client pid
    fd_read=open("fifo_clientTOserver",O_RDONLY);
    read(fd_read,&secondPid,sizeof(pid_t));
    while(secondPid == 0) {
        read(fd_read,&secondPid,sizeof(pid_t));
    }
    close(fd_read);
    unlink("fifo_clientTOserver");

    //send signal to first client
    kill(firstPid, SIGUSR1);

    while(*data == 0) {
        sleep(1);
    }

    //send signal to second client
    kill(secondPid, SIGUSR1);

    //follow the game and print who won
    followTheGame(data);

    /* detach from the segment: */
    if (shmdt(data) == -1) {
        perror("shmdt");
        exit(1);
    }

    if (shmctl(shmid,IPC_RMID, NULL) == -1) {
        perror("shmctl failed");
        exit(1);
    }

    return 0;
}

