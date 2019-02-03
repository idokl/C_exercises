/****************************************************
 * Student name: ido klein
 * Student ID: 203392964
 * Course Exercise Group: 03
 * Exercise name: ex1 task 2 (ex12)
 * CREDITS:
 * Thanks to friends that told me about the functions that
 * I have to use in every part of the exercise.
 * Thanks to some websites for examples of code to implement
 * some specific abilities.
 * Thanks to Alon for https://piazza.com/class/j0uxuou4sk82f8?cid=46
 * Thanks to the tirgul presentations.
 ***************************************************/

//defining enumurations and structs
typedef enum {
    FALSE, TRUE
} BOOL;

typedef enum {
    SAME = 1, SIMILAR = 2, DIFFERENT = 3
} FILES_COMPARISON_RESULT;

typedef struct Paths {
    char *rootDirectory;
    char *inputFile;
    char *outputFile;
} Paths;

typedef struct Result {
    BOOL cFileWasFound;
    BOOL multipleDirectories;
    BOOL compilationError;
    BOOL timeout;
    FILES_COMPARISON_RESULT comparisonResult;
    int depthOfCfile;
    BOOL errorOccured;
} Result;

//functions declerations
Paths ReadConfFile(char *confFile, char *buffer);
Result ScanDir(char *dirName, char *inputFile, char *outputFile,
               char *compFilePath);
void CheckCfile(char *cFileName, Result *resultPtr, char *inputFile,
                char *outputFile, char *compFilePath);

#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <memory.h>
#include <stdlib.h>
#include <fcntl.h>
#include <dirent.h>
#include <sys/wait.h>

/*****************************************************************
 * function name: main
 * the input:
 * @param argc - number of args in the argv array - we expected to 2
 * @param argv - array of args:
 *        arg[0] - should be the file path, e.g. ./a.out
 *        argv[1] - should be the path of the configuration file that
 *                  was defined in the exercise requirements
 * the output: 0 in success (in failure it exits with another return value)
 * the function operation:
 * this function call to ReadConfFile() for reading the configuration file
 * and finding the required files paths.
 * then, it looping through the root directory that contains the usernames
 * directories and it call to ScanDir() to check each of them. according to
 * the Result of scanDir() we write in the file result.csv a line that
 * corresponding to the scanning result of the username directory.
 ****************************************************************/
int main(int argc, char *argv[]) {
    struct Paths filesPaths;
    struct stat stat_p;
    DIR *pDir;
    struct dirent *usernameDirent;
    char buffer[1024];
    char compFilePath[256];
    char cwd[256];
    struct Result result;
    int resultsFD;
    char studentResult[1024];
    char gradeString[8];
    char gradeReasons[512];
    int grade;

    if (getcwd(cwd, sizeof(cwd)) == NULL)
        perror("getcwd() error");

    strcpy(compFilePath, cwd);
    strcat(compFilePath, "/comp.out");
    if (stat(compFilePath, &stat_p) == -1) {
        perror("The file comp.out is not in the current directory");
        exit(1);
    }

    if (argc < 2) {
        fprintf(stderr, "usage: %s configurationFile\n", argv[0]);
        exit(1);
    }
    filesPaths = ReadConfFile(argv[1], buffer);

    if ((pDir = opendir(filesPaths.rootDirectory)) == NULL)
        exit(1);

    if ((resultsFD = open("results.csv", O_CREAT | O_TRUNC | O_WRONLY, 0644))
                     < 0) {
        perror("open results.csv failed"); /* open failed */
        exit(1);
    }

    if (0 != chdir(filesPaths.rootDirectory)) {
        perror("chdir() error (rootDirectory in confFile can't be opened)");
    }

    // looping through the directory
    while ((usernameDirent = readdir(pDir)) != NULL) {
        if (strcmp(usernameDirent->d_name, ".") == 0
            || strcmp(usernameDirent->d_name, "..") == 0
            || strcmp(usernameDirent->d_name, "results.csv") == 0)
            continue;
        // declare the 'stat' structure
        if (stat(usernameDirent->d_name, &stat_p) == -1) {
            perror("Error occurred attempting to stat\n");
            continue;
        }
        if (!S_ISDIR(stat_p.st_mode)) {
            //usernameDirent->d_name is not a directory
            continue;
        }

        //call to ScanDir()
        result = ScanDir(usernameDirent->d_name, filesPaths.inputFile,
                         filesPaths.outputFile, compFilePath);

        if (TRUE == result.errorOccured)
            continue;
        strcpy(studentResult, usernameDirent->d_name);
        if (FALSE == result.cFileWasFound &&
            FALSE == result.multipleDirectories) {
            strcpy(gradeReasons, ",NO_C_FILE\n");
            grade = 0;
        } else if (TRUE == result.compilationError) {
            strcpy(gradeReasons, ",COMPILATION_ERROR\n");
            grade = 0;
        } else if (TRUE == result.timeout) {
            strcpy(gradeReasons, ",TIMEOUT\n");
            grade = 0;
        } else if (DIFFERENT == result.comparisonResult &&
                0 == result.depthOfCfile) {
            strcpy(gradeReasons, ",BAD_OUTPUT\n");
            grade = 0;
        } else if (DIFFERENT == result.comparisonResult &&
                0 < result.depthOfCfile) {
            strcpy(gradeReasons, ",BAD_OUTPUT,WRONG_DIRECTORY\n");
            grade = 0;
        } else if (SIMILAR == result.comparisonResult &&
                0 == result.depthOfCfile) {
            strcpy(gradeReasons, ",SIMILLAR_OUTPUT\n");
            grade = 70;
        } else if (SIMILAR == result.comparisonResult &&
                0 < result.depthOfCfile) {
            strcpy(gradeReasons, ",SIMILLAR_OUTPUT,WRONG_DIRECTORY\n");
            grade = 70 - (10 * result.depthOfCfile);
        } else if (TRUE == result.multipleDirectories) {
            strcpy(gradeReasons, ",MULTIPLE_DIRECTORIES\n");
            grade = 0;
        } else if (SAME == result.comparisonResult &&
                0 == result.depthOfCfile) {
            strcpy(gradeReasons, ",GREAT_JOB\n");
            grade = 100;
        } else if (SAME == result.comparisonResult &&
                0 < result.depthOfCfile) {
            strcpy(gradeReasons, ",WRONG_DIRECTORY,GREAT_JOB\n");
            grade = 100 - (10 * result.depthOfCfile);
        }
        if (grade < 0)
            grade = 0;
        sprintf(gradeString, ",%d", grade);
        strcat(studentResult, gradeString);
        strcat(studentResult, gradeReasons);
        write(resultsFD, studentResult, strlen(studentResult));
    }

    close(resultsFD);
    closedir(pDir);
    return 0;
}

/********************************************************************
 * function name: ScanDir
 * the input:
 * @param dirName - directory of student (i.e. username) for scanning
 * @param inputFile - input file for checking the c program (that is expected
 * to be in this directory) with.
 * @param outputFile - expected printings of the c program. we compare the
 * program output to this file content.
 * @param compFilePath - the path of the comp.out program that we need for
 * the files comparison.
 * the output:
 * @return struct Result that indicates the checking's results of the
 * student's directory and maintain the reasons that have to be considered
 * when we set the student's grade.
 * the function operation:
 * (according to the pseudo code in:
 * https://piazza.com/class/j0uxuou4sk82f8?cid=46 ):
 * if C file was found - keep processing that file according to requirements.
   Else
        If only one directory was found - keep searching recursively
                                          (keep track of leveling)
        else (multiple directories were found and no C files) -
                stop searching and return MULTIPLE DIRECTORIES result.
 *******************************************************************/
Result ScanDir(char *dirName, char *inputFile, char *outputFile,
               char *compFilePath) {
    struct stat stat_p;
    char msg[64];
    DIR *pDir;
    struct dirent *entryInDir;
    char *entryName;
    int lengthOfEntryName;
    int numberOfDirectoriesInThisDir = 0;
    struct Result result;

    result.cFileWasFound = FALSE; //flag
    result.compilationError = FALSE;  //flag
    result.timeout = FALSE; //flag
    result.comparisonResult = SAME;
    result.multipleDirectories = FALSE; //flag
    result.errorOccured = FALSE; //flag
    result.depthOfCfile = 0;

    //open the directory for reading its content (folders/files)
    if (stat(dirName, &stat_p) == -1) {
        sprintf(msg, "Error occurred attempting to stat %s\n", dirName);
        perror(msg);
        result.errorOccured = TRUE;
        return result;
    }
    if (0 != chdir(dirName)) {
        perror("chdir() error");
        result.errorOccured = TRUE;
        return result;
    }
    if ((pDir = opendir(".")) == NULL) {
        perror("opendir() error");
        exit(1);
    }

    //search a file with .c extension in the current directory
    while ((entryInDir = readdir(pDir)) != NULL) {
        entryName = entryInDir->d_name;
        lengthOfEntryName = strlen(entryName);
        if (strcmp(entryName, ".") == 0 || strcmp(entryName, "..") == 0)
            continue;
        if (stat(entryName, &stat_p) == -1) {
            perror("Error occurred attempting to stat\n");
            continue;
        }

        if (entryName[lengthOfEntryName - 2] == '.' &&
            entryName[lengthOfEntryName - 1] == 'c') {
            //printf("\tdebug: This is a c file\n");
            result.cFileWasFound = TRUE;
            //check c file
            break;
        }

        if (S_ISDIR(stat_p.st_mode)) {
            //This is a directory
            numberOfDirectoriesInThisDir++;
        }
    }

    if (TRUE == result.cFileWasFound) {
        //check the c file according to the requirements
        CheckCfile(entryName, &result, inputFile, outputFile, compFilePath);
    } else if (numberOfDirectoriesInThisDir > 1) {
        result.multipleDirectories = TRUE;
    } else if (numberOfDirectoriesInThisDir == 1) {
        closedir(pDir);
        if ((pDir = opendir(".")) == NULL) {
            perror("opendir() error");
            exit(1);
        }

        while ((entryInDir = readdir(pDir)) != NULL) {
            entryName = entryInDir->d_name;
            if (strcmp(entryName, ".") == 0 || strcmp(entryName, "..") == 0)
                continue;
            if (stat(entryName, &stat_p) == -1) {
                perror("Error occurred attempting to stat\n");
                continue;
            }
            if (S_ISDIR(stat_p.st_mode)) {
                //This is a single directory, then call to scanDir recursively
                result = ScanDir(entryName, inputFile, outputFile,
                                 compFilePath);
                continue;
            }
        }
        result.depthOfCfile++;
    } else {
        //no c file in this directory
    }

    closedir(pDir);

    if (0 != chdir("..")) {
        perror("chdir(..) error");
        exit(1);
    }
    return result;
}

/********************************************************************
 * function name: CheckCfile
 * the input:
 * @param cFileName - name of the .c file that we have to check
 * @param resultPtr - pointer to Result struct that this function updates
 *                    its values.
 * @param inputFile - file of input for checking the c program with.
 * @param outputFile - expected printings of the c program. we compare the
 * program output to this file content.
 * @param compFilePath - the path of the comp.out program that we need for
 * the files comparison.
 * the output: none.
 * the function operation:
 * a. execute gcc on the c file
 * b. if the compilation succeed, execute the execution file
 *    and give to it the inputFile as input. write its output to file.
 * c. compare the output file of the program with the correct output
 *    by the program comp.out.
 * update the Result fields according to the results of these checkings.
 *******************************************************************/
void CheckCfile(char *cFileName, Result *resultPtr, char *inputFile,
                char *outputFile, char *compFilePath) {
    char *argvForGcc[3];
    char *argvForCprogram[2];
    char *argvForCompProgram[4];
    struct stat statForCompiledFile;
    int wstatus, wstatus2, wstatus3;
    int *wstatusPtr = &wstatus;
    int *wstatusPtr2 = &wstatus2;
    int *wstatusPtr3 = &wstatus3;
    pid_t pid, pid2, pid3, pid4, processOf5SecondsPid;
    int returnVal;
    int inputFileFD, programOutputFD, errorsFD;
    char fileForOutput[] = "file_for_output";
    char fileForErrors[] = "the_errors";
    int i = 0;


    //printf("%s\n", fileForOutput);
    //printf("%s\n", fileForErrors);

    argvForGcc[0] = "gcc";
    argvForGcc[1] = cFileName;
    argvForGcc[2] = NULL;
    if ((errorsFD = open(fileForErrors, O_CREAT | O_TRUNC | O_WRONLY, 0644))
                    < 0) {
        perror("open failed");
        exit(1);
    }
    pid = fork();
    if (0 == pid) {
        dup2(errorsFD, 2);
        returnVal = execvp(argvForGcc[0], argvForGcc);
        if (returnVal == -1) {
            perror("exec failed ");
            exit(-1);
        }
    }
    waitpid(pid, wstatusPtr, 0);
    close(errorsFD);
    unlink(fileForErrors);
    returnVal = WEXITSTATUS(wstatus);
    if ((1 == returnVal) || (stat("a.out", &statForCompiledFile) == -1)) {
        resultPtr->compilationError = TRUE;
    } else {
        argvForCprogram[0] = "./a.out";
        argvForCprogram[1] = NULL;
        if (((inputFileFD = open(inputFile, O_RDONLY)) < 0) ||
            ((programOutputFD = open(fileForOutput, O_CREAT|O_TRUNC|O_WRONLY,
                                     0644)) < 0) ||
            ((errorsFD = open(fileForErrors, O_CREAT|O_TRUNC|O_WRONLY, 0644))
                         < 0)) {
            perror("open failed"); /* open failed */
            exit(1);
        }
        pid2 = fork();
        if (0 == pid2) {
            dup2(inputFileFD, 0);
            dup2(errorsFD, 2);
            dup2(programOutputFD, 1);
            returnVal = execvp(argvForCprogram[0], argvForCprogram);
            if (returnVal == -1) {
                perror("exec failed");
                exit(-1);
            }
        }

        sleep(5);
        if(0 == waitpid(pid2, wstatusPtr2, WNOHANG)) {
            resultPtr->timeout = TRUE;
            kill(pid2, SIGKILL);
        }

        close(inputFileFD);
        close(programOutputFD);
        close(errorsFD);
        unlink(fileForErrors);
        unlink("a.out");

        if (resultPtr->timeout == FALSE) {
            argvForCompProgram[0] = compFilePath;
            argvForCompProgram[1] = outputFile;
            argvForCompProgram[2] = fileForOutput;
            argvForCompProgram[3] = NULL;
            pid4 = fork();
            if (0 == pid4) {
                returnVal = execvp(argvForCompProgram[0], argvForCompProgram);
                if (returnVal == -1) {
                    perror("exec failed");
                    exit(-1);
                }
            }
            wstatus = waitpid(pid4, wstatusPtr3, 0);
            resultPtr->comparisonResult = WEXITSTATUS(wstatus3);
            //now the comparisonResult maintain 1 or 2 or 3
            //(according to SAME, SIMILAR, DIFFERENT)
        }
        unlink(fileForOutput);
    }
}

/********************************************************************
 * function name: ReadConfFile
 * the input:
 * @param confFile - name of the configuration file to parse
 * @param buffer - buffer to maintain the paths that are read from the
 *                 configuration file
 * the output:
 * @return Paths struct that maintain pointers to the beggining
 *         of each path that is in the buffer.
 * the function operation:
 * read the configuration file to the buffer and set the pointers of
 * the Paths struct to point on the beginnings of each of the 3 lines.
 ********************************************************************/
Paths ReadConfFile(char *confFile, char *buffer) {
    Paths pathsFromFile;
    int fd;           /* file1 descriptor */
    int charsRead;    /* how many chars were actually read */
    int i;

    strcpy(buffer, " ");
    fd = open(confFile, O_RDONLY);
    if (fd < 0) /* means file open did not take place */ {
        perror("after open confFile ");   /* text explaining why */
        exit(-1);
    }

    charsRead = read(fd, buffer, 600);
    pathsFromFile.rootDirectory = buffer;
    for (i = 0; i < charsRead; i++) {
        if (buffer[i] == '\n') {
            buffer[i] = 0;
            pathsFromFile.inputFile = buffer + i + 1;
            break;
        }
    }
    for (; i < charsRead; i++) {
        if (buffer[i] == '\n') {
            buffer[i] = 0;
            pathsFromFile.outputFile = buffer + i + 1;
            break;
        }
    }
    for (; i < charsRead; i++) {
        if (buffer[i] == '\n') {
            buffer[i] = 0;
            break;
        }
    }

    //free allocated structures
    close(fd);

    return pathsFromFile;
}