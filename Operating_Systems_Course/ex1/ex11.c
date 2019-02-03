/****************************************************
 * Student name: ido klein
 * Student ID: 203392964
 * Course Exercise Group: 03
 * Exercise name: ex1 task 1 (ex11)
 * CREDITS:
 * Thanks to friends that told me about the functions that
 * I have to use in every part of the exercise.
 * Thanks to the tirgul presentations.
 ***************************************************/

#include <stdio.h>
#include <sys/fcntl.h>
#include <errno.h>
#include<stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>

#define ONE_CHAR 1
#define DIFFERENT_FILES 3
#define SIMILAR_FILES 2
#define SAME_FILES 1

/*****************************************************************
 * function name: main
 * the input:
 * @param argc - number of args in the argv array - we expected to 3
 * @param argv - array of args:
 *        arg[0] - should be the file path, e.g. ./comp.out
 *        argv[1] - should be the path of the first file to compare
 *        argv[2] - should be the path of the second file to compare
 * the output:
 * 3 if the files are different
 * 2 if the files are similar
 * 1 if the files are the same
 * the function operation:
 * we initialize the return value to SAME_FILES.
 * then we open the 2 files and read them char after char.
 * if we recognize a letter that is not the same, we change the
 * return value to SIMILAR_FILES.
 * then we ignore spaces and newlines and ignore case sensitive.
 * we continue the comparison and if the files still different,
 * we update the return value to DIFFERENT_FILES.
 ****************************************************************/
int main(int argc, char* argv[]) {
    int i;
    int fdin1;           /* file1 descriptor */
    int fdin2;          /* file2 descriptor */
    char buf1[ONE_CHAR+1]; /* buffer of file1*/
    char buf2[ONE_CHAR+1]; /* buffer of file2 */
    int charsr1;          /* how many chars were actually read */
    int charsr2;          /* how many chars were actually read */
    int returnValue = SAME_FILES;

    strcpy(buf1, " ");
    strcpy(buf2, " ");

    if (argc < 3) {
        fprintf(stderr, "usage: %s file1 file2\n", argv[0]);
        exit(1);
    }

    fdin1 = open(argv[1],O_RDONLY);
    if (fdin1 < 0) /* means file open did not take place */ {
        perror("after open file1 ");   /* text explaining why */
        exit(-1);
    }
    fdin2 = open(argv[2], O_RDONLY);
    if (fdin2 < 0)	/* means file open did not take place */ {
        perror("after open file2 ");   /* text explaining why */
        exit(-1);
    }

    do {
        charsr1 = read(fdin1,buf1,ONE_CHAR);
        charsr2 = read(fdin2,buf2,ONE_CHAR);
        if (returnValue == SAME_FILES && charsr2 != charsr1) {
            returnValue = SIMILAR_FILES;
        }
        if (returnValue == SAME_FILES && strcmp(buf1,buf2) != 0) {
            returnValue = SIMILAR_FILES;
        }
        if (strcmp(buf1,buf2) != 0 && returnValue == SIMILAR_FILES) {
            while ((*buf1 == ' ' || *buf1 == '\n') && (charsr1 == ONE_CHAR)) {
                charsr1 = read(fdin1,buf1,ONE_CHAR);
            }
            while ((*buf2 == ' ' || *buf2 == '\n') && (charsr2 == ONE_CHAR)) {
                charsr2 = read(fdin2,buf2,ONE_CHAR);
            }
            if (toupper(*buf1) != toupper(*buf2)) {
                returnValue = DIFFERENT_FILES;
            }
        }
    } while ((charsr1 == ONE_CHAR) && (charsr2 == ONE_CHAR));
    if (charsr1 == ONE_CHAR && returnValue != DIFFERENT_FILES) {
        while ((*buf1 == ' ' || *buf1 == '\n') && (charsr1 == ONE_CHAR))
            charsr1 = read(fdin1, buf1, ONE_CHAR);
        if (*buf1 != ' ' && *buf1 != '\n')
            returnValue = DIFFERENT_FILES;
    }
    if (charsr2 == ONE_CHAR && returnValue != DIFFERENT_FILES) {
        while ((*buf2 == ' ' || *buf2 == '\n') && (charsr2 == ONE_CHAR))
            charsr2 = read(fdin2, buf2, ONE_CHAR);
        if (*buf2 != ' ' && *buf2 != '\n')
            returnValue = DIFFERENT_FILES;
    }

    close(fdin1);		/* free allocated structures */
    close(fdin2);		/* free allocated structures */

    return returnValue;
}
