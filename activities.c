#include "headers.h"

int print_active_processes_spawned_by_my_shell() {
    LL_Node trav = LL->first;
    int n = LL->no_of_nodes;

    int* pids = (int*) calloc(n, sizeof(int));
    char** process_names = (char**) calloc(n, sizeof(char*));
    char* process_states = (char*) calloc(n, sizeof(char));

    int idx = 0;

    while (trav != NULL) {

        int curr_pid = trav->pid;
        char* curr_process_name = trav->cmd;
        int curr_process_flag = trav->flag;

        pids[idx] = curr_pid;
        process_names[idx] = curr_process_name;

        char path_stat[256] = {0};
        sprintf(path_stat, "/proc/%d/stat", curr_pid);
        char* status;

        FILE *fptr = fopen(path_stat, "r");
        if (fptr == NULL) {
            fprintf(stderr, "\033[1;31mopen: Could not open file to read\033[1;0m\n");
            return 0;
        }

        char data[100000];
        fscanf(fptr, " %[^\n]", data);
        char** data_array = generate_tokens(data, ' ');
        status = data_array[2];

        if (strcmp(status, "R") == 0 || strcmp(status, "S") == 0) {
            process_states[idx++] = 'R';
        } else if (strcmp(status, "D") == 0 || strcmp(status, "T") == 0 || strcmp(status, "Z")) {
            process_states[idx++] = 'S';
        }

        fclose(fptr);
        free_tokens(data_array);

        trav = trav->next;
    }

    // bubble sort for lexicographically sorting the pids
    bubble_sort_processes(pids, process_names, process_states, n);
    
    for (int k = 0; k < n; k++) {
        printf("%d : %s - ", pids[k], process_names[k]);
        if (process_states[k] == 'R') {
            printf("Running\n");
        } else if (process_states[k] == 'S') {
            printf("Stopped\n");
        }
    }
    return 1;
}

void bubble_sort_processes(int* pids, char** process_names, char* process_states, int n) {
    for (int i = n - 2; i >= 0; i--) {
        for (int j = 0; j <= i; j++) {
            if (pids[j] > pids[j + 1]) {
                // swap
                int temp_pid = pids[j + 1];
                char* temp_process_name = process_names[j + 1];
                char temp_process_state = process_states[j + 1];

                pids[j + 1] = pids[j];
                process_names[j + 1] = process_names[j];
                process_states[j + 1] = process_states[j];

                pids[j] = temp_pid;
                process_names[j] = temp_process_name;
                process_states[j] = temp_process_state;
            }
        }
    }
}

