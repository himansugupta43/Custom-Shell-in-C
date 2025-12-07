#include "headers.h"

int ping(char** argument_tokens, int no_of_arguments) {
    if (no_of_arguments < 2) {
        fprintf(stderr, "\033[1;31mping : Invalid Arguments\033[1;0m\n");
        return 0;
    } else {
        int pid = atoi(argument_tokens[1]);
        int sig = atoi(argument_tokens[2]);

        return ping_sig(pid, sig);
    }
    return 1;
}

int ping_sig(int pid, int sig) {
    // checking if the process exists
    int result = kill(pid, 0);

    if (result == 0) {
        // process exists
        int response = kill(pid, sig);
        if (response == 0) {
            printf("Sent signal %d to process with pid %d\n", sig, pid);

            LL_Node trav = LL->first;
            while (trav != NULL) {
                int cstatus;
                if (waitpid(trav->pid, &cstatus, WNOHANG) == trav->pid) {
                    free_node(trav);
                    break;
                }
                trav = trav->next;
            }
        } else {
            fprintf(stderr, "\033[1;31mping : kill: could not send signal\033[1;0m\n");
            return 0;
        }
    } else {
        // process does not exist
        fprintf(stderr, "\033[1;31mping : No such process exists\033[1;0m\n");
        return 0;
    }
    return 1;
}

