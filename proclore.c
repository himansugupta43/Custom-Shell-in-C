#include "headers.h"

int print_info_pid(char** argument_tokens, int no_of_arguments, int ip) {
    char* pid = (char*) calloc(50, sizeof(char));
    if (no_of_arguments > 1) {
        fprintf(stderr, "\033[1;31mproclore : Invalid Arguments: Excess of arguments provided\033[1;0m\n");
        return 0;
    }
    
    if (no_of_arguments == 0 && ip == 0) {         // if no arguments are passed than print the details of the termianl process
        int curr_pid = getpid();        // getting the pid of the current running process
        sprintf(pid, "%d", curr_pid);   // writing the pid in the form of a string to pass
    } else if (no_of_arguments == 0 && ip == 1) {
        char inp_buff[999999] = {0};

        int bytes_read = read(STDIN_FILENO, inp_buff, 999998);
        if (bytes_read < 0) {
            fprintf(stderr, "\033[1;31mError in reading\033[1;0m\n");
            return 0;
        } else {
            if (inp_buff[strlen(inp_buff) - 1] == '\n') {
                inp_buff[strlen(inp_buff) - 1] = '\0';
            }
            strcpy(pid, inp_buff);
        }

    } else { // if pid of the process is provided
        strcpy(pid, argument_tokens[1]);
    }
    proclore(pid);
    free(pid);
    return 1;
}

void proclore(char* pid) {
    char path_stat[256] = {0};
    sprintf(path_stat, "/proc/%d/stat", atoi(pid));

    char path_maps[256] = {0};
    sprintf(path_maps, "/proc/%d/maps", atoi(pid));

    char path_exe[256] = {0};
    sprintf(path_exe, "/proc/%d/exe", atoi(pid));

    char* status;
    char* tty_nr;
    char* process_group;
    unsigned long virtual_address;
    char executable_path[MAX_LEN];

    FILE *fptr = fopen(path_stat, "r");
    if (fptr == NULL) {
        fprintf(stderr, "\033[1;31mNo such process with process id %s running\033[1;0m\n", pid);
        return;
    } else {
        char data[100000];
        fscanf(fptr, " %[^\n]", data);
        char** data_array = generate_tokens(data, ' ');
        status = data_array[2];
        tty_nr = data_array[6];
        bg_process = 0;
        if (strcmp(tty_nr, "0") == 0) {
            bg_process = 1;
        }
        process_group = data_array[4];
        fclose(fptr);
        free_tokens(data_array);
    }

    FILE* fptr2 = fopen(path_maps, "r");
    if (fptr2 != NULL) {
        char buffer[50000];
        unsigned long start;
        unsigned long end;
        fgets(buffer, 50000, fptr2);
        sscanf(buffer, "%lx-%lx", &start, &end);
        virtual_address = start;
    } else {
        fprintf(stderr, "\033[1;31mproclore: cannot open /proc/pid/maps\033[1;0m\n");
        return;
    }

    printf("pid : %s\n", pid);

    printf("Process status : %s", status);

    int background_process = 0;
    LL_Node trav = LL->first;
    while (trav != NULL) {
        if (trav->pid == atoi(pid)) {
            background_process = 1;
            break;
        }
        trav = trav->next;
    }
    if (background_process == 1) {
        printf("\n");
    } else {
        printf("+\n");
    }

    printf("Process group : %s\n", process_group);

    printf("Virtual memory : %lu\n", virtual_address);
    
    ssize_t length = readlink(path_exe, executable_path, sizeof(executable_path) - 1);
    executable_path[length] = '\0';

    printf("Executable path : %s\n", executable_path);
}

