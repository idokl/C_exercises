/****************************************************************************************
 * BS"D
 * Student name: ido klein
 * Student ID: 203392964
 * Course Exercise Group: 03
 * Exercise name: ex2
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

/****************************************************************************************
 * function name: main
 * the input:
 * @param argc - number of args in the argv array - we expected to 2
 * @param argv - array of args:
 *        arg[0] - should be the file path, e.g. ./ex2.out
 *        argv[1] - should be number of seconds for the 2048 game
 * the output: 0 (if no errors)
 * the function operation:
 * manage communication between ex2_inp.out process (the view) and ex2_upd.out process (the logic)
 * in order to execute the 2048 game according to the requirements of ex2.pdf
 ****************************************************************************************/
int main(int argc, char* argv[0]) {
    char *argvForInpExec[2];
    char *argvForUpdExec[3];
    char pidString[15];
    int returnVal;
    int     fd[2];
    pid_t   pid1, pid2;

    if (argc < 2) {
        perror("the program expects to argument - number of seconds of the game\n");
        exit(-1);
    }
    if (pipe(fd) < 0) {
        perror("pipe error");
        exit(-1);
    }
    if ((pid1 = fork()) < 0) {
        perror("fork error");
        exit(-1);
    }
    else if (pid1 > 0) { /* parent */
        if ((pid2 = fork()) < 0) {
            perror("fork error");
            exit(-1);
        }
        else if (pid2 > 0) { /* parent */
            sleep(atoi(argv[1]));
            kill(pid1, SIGINT);
            kill(pid2, SIGINT);
            close(fd[0]);
            close(fd[1]);
            exit(0);
        } else { /* second child */
            dup2(fd[1], STDOUT_FILENO);
            argvForUpdExec[0] = "./ex2_upd.out";
            sprintf(pidString, "%d", pid1);
            argvForUpdExec[1] = pidString;
            argvForUpdExec[2] = NULL;
            returnVal = execvp(argvForUpdExec[0], argvForUpdExec);
            if (returnVal == -1) {
                perror("exec failed");
                exit(-1);
            }
            close(fd[1]);
        }
    } else { /* first child */
        close(fd[1]);
        dup2(fd[0], STDIN_FILENO);
        argvForInpExec[0] = "./ex2_inp.out";
        argvForInpExec[1] = NULL;
        returnVal = execvp(argvForInpExec[0], argvForInpExec);
        if (returnVal == -1) {
            perror("exec failed");
            exit(-1);
        }
        close(fd[0]);
    }
    return 0;
}