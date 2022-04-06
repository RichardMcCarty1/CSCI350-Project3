#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <limits.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>

int main(int argc, char *argv[]) {
    char error_message[30] = "An error has occurred\n";
    char *line = NULL;
    size_t length = 0; 
    ssize_t lineSize = 0;

    pid_t pid;
    char foo[4096];


    //Interactive mode
    if(argc == 1) {
        printf("\nccsh> ");
        while(getline(&line, &length, stdin)) {
            if(feof(stdin)) {
                exit(0);
            }
            char path[50];
            pid_t childPID;
            int status;
            char* command = NULL;
            int hasArgs = 0;
            strcpy(path, "/bin/");
            ///bin and /usr/bin, try access("/bin/ls", X_OK). if that fails, try "usr/bin/ls", if that also fails its an error

            //Trim whitespace from both sides
            while(isspace(*line)) {
                line++;
            }
            char* end = line + strlen(line);
            while(isspace(*--end)) {}
            *(end+1) = '\0';

            //Isolates the command
            char* firstSpace = strchr(line, ' ');
            //If a space exists, grab command
            if(firstSpace != NULL && strcmp(firstSpace, " ") != 0) {
                size_t length = firstSpace - line;
                command = (char*)malloc((length + 1) * sizeof(char));
                strncpy(command, line, length);
                line += length + 1;
                hasArgs = 1;
            } 
            //Otherwise just rip the line because no arguments
            else {
                command = (char*)malloc((strlen(line) + 1)* sizeof(char));
                command = strdup(line);
            }

            //Check for program in /bin
            strcat(path, command);

            if(access(path, X_OK) == 0) {
                pid = fork();
                if(pid == 0) {
                    //If no arguments, run it
                    if(hasArgs == 0) {
                        char *args[] = {path, 0};
                        execv(path, args);
                    } else {
                        //otherwise separate line (arguments) by spaces, push to args arr
                        char **args = NULL;
                        char *val = strtok(line, " ");
                        int spaceCount = 0;

                        args = realloc(args, ++spaceCount * sizeof(char*));
                        args[spaceCount - 1] = path;

                        while (val) {
                            args = realloc(args, ++spaceCount * sizeof(char*));
                            args[spaceCount - 1] = val;
                            val = strtok(NULL, " ");
                        }
                        args = realloc(args, (spaceCount + 1) * sizeof(char*));
                        args[spaceCount] = 0;
                        args[0] = args[0] + 1;
                        execv(path, args);

                    }
                    //Shouldnt procced this far, if it does, an error occured
                    printf("\n%s\n", error_message);
                    exit(0);

                } else {
                    wait(NULL);                    
                }
            } else {
                printf("%s", error_message);
            }
            //Re-loop
            free(command);
            printf("ccsh> ");
        }
        exit(0);
    } 
    //Batch mode
    else if(argc == 2) {

        FILE *file = fopen(argv[1], "r");
        char *line = (char*)malloc(256 * sizeof(char));
        size_t length = 0;
        while(getline(&line, &length, file) != -1) {
            char* dupped = strdup(line);
            int isLastLine = dupped[strlen(dupped) - 1] == '\n' ? 0 : 1;
            char path[50];
            pid_t childPID;
            int status;
            char* command = NULL;
            int hasArgs = 0;
            strcpy(path, "/bin/");

            if(!isLastLine) {
                //replace \n with null terminator for later parsing
                line[strlen(line) - 1] = '\0';
            }


            //Isolates the command
            char* firstSpace = strchr(line, ' ');
            //If a space exists, grab command
            if(firstSpace != NULL && strcmp(firstSpace, " ") != 0) {
                size_t length = firstSpace - line;
                command = (char*)malloc((length + 1) * sizeof(char));
                strncpy(command, line, length);
                line += length + 1;
                hasArgs = 1;
            } 
            //Otherwise just rip the line because no arguments
            else {
                command = (char*)malloc((strlen(line) + 1)* sizeof(char));
                command = strdup(line);
            }

            //Check for program in /bin
            strcat(path, command);

            if(access(path, X_OK) == 0) {
                pid = fork();
                if(pid == 0) {
                    //If no arguments, run it
                    if(hasArgs == 0) {
                        char *args[] = {path, 0};
                        execv(path, args);
                    } else {
                        //otherwise separate line (arguments) by spaces, push to args arr
                        char **args = NULL;
                        char *val = strtok(line, " ");
                        int spaceCount = 0;

                        args = realloc(args, ++spaceCount * sizeof(char*));
                        args[spaceCount - 1] = path;

                        while (val) {
                            args = realloc(args, ++spaceCount * sizeof(char*));
                            args[spaceCount - 1] = val;
                            val = strtok(NULL, " ");
                        }
                        args = realloc(args, (spaceCount + 1) * sizeof(char*));
                        args[spaceCount] = 0;
                        args[0] = args[0] + 1;
                        execv(path, args);

                    }
                    //Shouldnt procced this far, if it does, an error occured
                    printf("\n%s\n", error_message);
                    exit(0);

                } else {
                    wait(NULL);                    
                }
            } else {
                printf("%s", error_message);
            }
            printf("\n");
            free(command);
        }
        

        return 0;
    }
    //Too many args
    else {
        printf("Too many arguments specified.\n");
        exit(1);
    }
    
}



