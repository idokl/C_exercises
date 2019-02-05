/****************************************************************************************
 * BS"D
 *
 * Student name: ido klein
 * Student ID: 203392964
 * Course Exercise Group: 03
 * Exercise name: ex3
 ****************************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <fcntl.h>
#include <unistd.h>
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

Bool gameOver = FALSE;

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

//write my step to the shared memory
void writeSharedMemory(char** dataPtr, Step myStep) {
    char colorChar;
    char colNum = myStep.col + '0';
    char rowNum = myStep.row + '0';

    switch(myStep.color) {
        case BLACK:
            colorChar = 'b';
            break;
        case WHITE:
            colorChar = 'w';
            break;
    }
    sprintf(*dataPtr, "%c%c%c", colorChar, colNum, rowNum);
    *dataPtr = *dataPtr + 4;
}

void printBoard(Board* board) {
    int r, c;
    printf("The board is:\n");
    for(r = 0; r < 8; r++) {
        for(c = 0; c < 7; c++) {
            printf("%d ", board->cell[r][c]);
        }
        printf("%d\n", board->cell[r][7]);
    }
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

//check the format of the input is: [x,y]
Validity checkFormat(char* input) {
    int index = 0;
    if (input[index]  != '[')
        return INVALID;
    index++;
    if (input[index] < '0' || input[index] > '9')
        return INVALID;
    while('0' <= input[index] && input[index] <= '9')
        index++;
    if (input[index]  != ',')
        return INVALID;
    index++;
    if (input[index] < '0' || input[index] > '9')
        return INVALID;
    while('0' <= input[index] && input[index] <= '9')
        index++;
    if (input[index]  != ']')
        return INVALID;
    return VALID;
}

Validity checkAccess(int col, int row) {
    if (col < 0 || col > 7 || row < 0 || row > 7)
        return  INVALID;
    else
        return VALID;
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

//put token of tokenColor on the board if it can invert tokens of the opponent
Validity tryToPutToken(Board* board,  Color tokenColor, int col, int row) {
    Validity validityOfThisStep = INVALID;
    if (NONE != board->cell[row][col]) {
        return INVALID;
    }
    //invert tokens in the directions: down, up, right, left
    if (VALID == checkAbilityToInvertTokens(board, tokenColor, col, row, dontChangeIndex, increaseIndex)) {
        invertTokens(board, tokenColor, col, row, dontChangeIndex, increaseIndex);
        validityOfThisStep = VALID;
    }
    if (VALID == checkAbilityToInvertTokens(board, tokenColor, col, row, dontChangeIndex, decreaseIndex)) {
        invertTokens(board, tokenColor, col, row, dontChangeIndex, decreaseIndex);
        validityOfThisStep = VALID;
    }
    if (VALID == checkAbilityToInvertTokens(board, tokenColor, col, row, increaseIndex, dontChangeIndex)) {
        invertTokens(board, tokenColor, col, row, increaseIndex, dontChangeIndex);
        validityOfThisStep = VALID;
    }
    if (VALID == checkAbilityToInvertTokens(board, tokenColor, col, row, decreaseIndex, dontChangeIndex)) {
        invertTokens(board, tokenColor, col, row, decreaseIndex, dontChangeIndex);
        validityOfThisStep = VALID;
    }
    //invert tokens in diagonal directions
    if (VALID == checkAbilityToInvertTokens(board, tokenColor, col, row, increaseIndex, increaseIndex)) {
        invertTokens(board, tokenColor, col, row, increaseIndex, increaseIndex);
        validityOfThisStep = VALID;
    }
    if (VALID == checkAbilityToInvertTokens(board, tokenColor, col, row, increaseIndex, decreaseIndex)) {
        invertTokens(board, tokenColor, col, row, increaseIndex, decreaseIndex);
        validityOfThisStep = VALID;
    }
    if (VALID == checkAbilityToInvertTokens(board, tokenColor, col, row, decreaseIndex, increaseIndex)) {
        invertTokens(board, tokenColor, col, row, decreaseIndex, increaseIndex);
        validityOfThisStep = VALID;
    }
    if (VALID == checkAbilityToInvertTokens(board, tokenColor, col, row, decreaseIndex, decreaseIndex)) {
        invertTokens(board, tokenColor, col, row, decreaseIndex, decreaseIndex);
        validityOfThisStep = VALID;
    }
    if (VALID == validityOfThisStep) {
        //put the added token
        board->cell[row][col] = tokenColor;
    }
    return validityOfThisStep;
}

//check whether game finished (possible reasons: the board is full or there isn't valid step)
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

//the main function that manage the logic of the game
void manageGame(char* currentData) {
    Board board;
    Step myStep, opponentStep;
    Color opponentColor;
    Color winnerColor;
    Validity squareFormat;
    Validity squareExistence;
    Validity reversiLogicValidity;
    int gameFinished = 0;
    char input[100];

    opponentStep = readSharedMemory(&currentData);
    initializeBoard(&board);
    if (NONE == opponentStep.color) {
        myStep.color = BLACK;
        opponentColor = WHITE;
    } else if (BLACK == opponentStep.color) {
        opponentColor = BLACK;
        myStep.color = WHITE;
        tryToPutToken(&board, opponentStep.color, opponentStep.col, opponentStep.row);
    }

    //printBoard(&board);
    while(!gameFinished) {
        printf("Please choose a square\n");
        do {
            //scanf(" [%d,%d]", &myStep.col, &myStep.row);
            scanf(" %s", input);
            squareFormat = checkFormat(input);
            if(INVALID == squareFormat) {
                printf("This square is invalid\nPlease choose another squa\n");
            } else {
                sscanf(input, " [%d,%d]", &myStep.col, &myStep.row);
                squareExistence = checkAccess(myStep.col, myStep.row);
                if (INVALID == squareExistence) {
                    printf("No such square\nPlease choose another square\n");
                } else {
                    reversiLogicValidity = tryToPutToken(&board, myStep.color, myStep.col, myStep.row);
                    if (INVALID == reversiLogicValidity) {
                        printf("This square is invalid\nPlease choose another square\n");
                    }
                }
            }
        } while (INVALID == squareFormat || INVALID == squareExistence || INVALID == reversiLogicValidity);

        printBoard(&board);
        if (TRUE == isGameFinished(&board, opponentColor)) {
            writeSharedMemory(&currentData, myStep);
            break;
        }
        printf("Waiting for the other player to make a move\n");
        writeSharedMemory(&currentData, myStep);

        sleep(1);
        opponentStep = readSharedMemory(&currentData);
        while (opponentStep.color != opponentColor) {
            printf("Waiting for the other player to make a move\n");
            sleep(1);
            opponentStep = readSharedMemory(&currentData);
        }
        tryToPutToken(&board, opponentStep.color, opponentStep.col, opponentStep.row);
        printBoard(&board);
        if (TRUE == isGameFinished(&board, myStep.color)) {
            break;
        }
    }
    winnerColor = whoDidWin(&board);
    if (BLACK == winnerColor) {
        printf("Winning player: Black\n");
    } else if (WHITE == winnerColor) {
        printf("Winning player: White\n");
    } else {
        printf("No winning player\n");
    }
    gameOver = TRUE;
}

//handle the signal from the server
void handlerOfUsr1(int signum) {
    int shmid;
    key_t key;
    char *data;

    // make the key:
    if ((key = ftok("ex31.c", 'k')) == -1) {
        perror("ftok");
        exit(1);
    }
    // connect to the segment:
    if ((shmid = shmget(key, SHM_SIZE, 0644)) == -1) {
        perror("shmget");
        exit(1);
    }
    // attach to the segment to get a pointer to it:
    data = shmat(shmid, NULL, 0);
    if (data == (char *)(-1)) {
        perror("shmat");
        exit(1);
    }

    //manage the game
    manageGame(data);

    //detach from the segment:
    if (shmdt(data) == -1) {
        perror("shmdt");
        exit(1);
    }
}


int main(int argc, char *argv[])
{
    sigset_t blockMaskToUsr1;
    struct sigaction actToUsr1;
    int fd_read;

    //sigaction to SIGUSR1
    sigfillset(&blockMaskToUsr1);
    actToUsr1.sa_mask = blockMaskToUsr1;
    actToUsr1.sa_handler = handlerOfUsr1;
    actToUsr1.sa_flags = 0;
    if (sigaction(SIGUSR1, &actToUsr1, NULL) == -1) {
        perror("error: sigaction\n");
        exit(-1);
    }

    pid_t myPid = getpid();
    //printf("my pid is %d\n",myPid);

    //open fifo to write
    fd_read=open("fifo_clientTOserver",O_WRONLY);
    write(fd_read,&myPid,sizeof(pid_t));
    close(fd_read);

    //wait to signal from the server
    while(FALSE == gameOver) {
        sleep(1);
    }

    return 0;
}
