#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <ctype.h>
#include <errno.h>

#define RED "\033[0;31m"
#define MAX_CHARS 1000  // max charcaters entered by user in a line
FILE *debug;
FILE *logger;

void handler() {
    int status;
    wait(&status); // clean the child :)
    fprintf(logger, "Child process in terminated with status  = %d  \n ", status);
    fflush(logger) ;
//    printf("Child process in terminated with status  = %d  \n ", status);
    //signal(SIGINT , handler) ;
}

int main() {
    /*
     * Declaring main arrays and log file
     */
    debug = fopen("debug.txt", "w");
    logger = fopen("logger.log", "w");
    char text[MAX_CHARS];
    char *args[MAX_CHARS];
    printf("\n=========== Shell gamed gdn :) ================= \n");
    //======================
    while (1) {
        int background = 0;
        printf(">>> ");
        fgets(text, MAX_CHARS, stdin); // fgets include the '\n' in the end of the input0.

        /**
         * Check if the input is all whitespace (otherwise you may encounter core dump error)
         **/
        int bad_input = 1; // indicates whether all the input is white characters
        for (int i = 0; i < strlen(text); i++) {
            if (!isspace(text[i])) {
                bad_input = 0;
            }
        }
        if (bad_input) {
            fprintf(debug, "Bad input encountered\n");
            continue;
        }
        /**
         * Replace the '\n' at the end with a null character and split the input
         **/
        text[strlen(text) - 1] = '\0';// replace the \n with a null character
        args[0] = strtok(text, " ");
        int ptr = 0;
        while (args[ptr] != NULL) {
            args[++ptr] = strtok(NULL, " ");
        }

        /**
         * If there is exit at the beginning then we terminate the shell
         **/
        if (strcmp(args[0], "exit") == 0) {
            printf("Goodbie :) \n");
            break;
        }
        if (strcmp(args[ptr - 1], "&") == 0) {
            fprintf(debug , "\nWARNING : Found & in the command \n");
            args[ptr - 1] = NULL;
            background = 1;
        }
        signal(SIGCHLD, handler);
        if (background == 1)
            fprintf(debug, "\nWARNING : background process detected \n");
        else
            fprintf(debug, "\n No Background processes detected \n");
        pid_t child_pid;
        child_pid = fork();
        fprintf(debug ,"\n**We are in process with pid = %d and ppid = %d \n", getpid(), getppid());
        if (child_pid == 0) {
            if (execvp(args[0], args) < 0) {
                fprintf(debug, "Wrong command -> execvp returned '-1' in the child process \n");
            }
            // shuldn't it return ?
            printf("Error in the command :( \n");
            fprintf(debug, "Exitting Now the child because 'execvp' returned -1 \n");
            _exit(EXIT_FAILURE);
        } else if (child_pid < 0) {
            fprintf(debug, "Error in Forking -> child_pid returned -1\n");
            //TODO : Should I exit here ?
        } else {
            if (!background) {
                fprintf(debug, "Waiting for the child process to end \n");
                int status;
                fprintf(debug , "\n**We are in parent now with pid = %d and It is goint to wait for the child **\n", getpid());
                while (wait(&status) != child_pid) {
                }
            } else {
                fprintf(debug , "\n** We are in the parent now with pid = %d but we aren't going to wait for background child \n",
                       getpid());
                fprintf(debug, "Parent will not wait deliberately for the child  \n");
            }

        }
        fflush(debug) ;
    }
    fprintf(debug, "Main loop ended \n");
    fprintf(debug, "Closing the file.. \n Returning..\n");
    fclose(debug);
    return 0;
}
