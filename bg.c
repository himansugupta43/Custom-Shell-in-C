#include "headers.h"

int run_bg_process(char** argument_tokens, int no_of_arguments) {
    if (no_of_arguments == 0) {
        // if no pid of the background process is 
        fprintf(stderr, "\033[1;31mbg : Invalid arguments: Please provide the bg process id\033[1;0m\n");
        return 0;
    }
    int pid = atoi(argument_tokens[1]);

    int result = kill(pid, 0);

    if (result == 0) {
        kill(pid, SIGCONT);

        char path_stat[256] = {0};
        sprintf(path_stat, "/proc/%d/stat", pid);
        char* status;

        FILE *fptr = fopen(path_stat, "r");
        if (fptr == NULL) {
            fprintf(stderr, "\033[1;31mbg : open: Could not open file to read\033[1;0m\n");
            return 0;
        }

        char data[100000];
        fscanf(fptr, " %[^\n]", data);
        char** data_array = generate_tokens(data, ' ');
        status = data_array[2];

        if (strcmp(status, "R") == 0 || strcmp(status, "S") == 0) {
            printf("Process with pid %d running in the background\n", pid);
        } else if (strcmp(status, "D") == 0 || strcmp(status, "T") == 0 || strcmp(status, "Z")) {
            fprintf(stderr, "\033[1;31mbg : Failed to run the background process\033[1;0m\n");
            return 0;
        }
        fclose(fptr);
        free_tokens(data_array);
    } else {
        fprintf(stderr, "\033[1;31mNo such process exists\033[1;0m\n");
        return 0;
    }
    return 1;
}