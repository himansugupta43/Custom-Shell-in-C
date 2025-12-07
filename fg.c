#include "headers.h"

int bring_process_to_foreground(char** argument_tokens) {
    if (argument_tokens[1] == NULL) {
        fprintf(stderr, "\033[1;31mfg : No argument provided: pid required\033[1;0m\n");
        return 0;
    }
    int pid = atoi(argument_tokens[1]);

    int result = kill(pid, 0);

    if (result == 0) {
        int cstatus;
        if (tcsetpgrp(STDIN_FILENO, pid) == -1) {
            fprintf(stderr, "\033[1;31mfg : tcsetpgrp: could not change the group id\033[1;0m\n");
            return 0;
        }
        kill(pid, SIGCONT);
        waitpid(pid, &cstatus, WUNTRACED);
        tcsetpgrp(STDIN_FILENO, shell_pid);
    } else {
        fprintf(stderr, "\033[1;31mNo such process exists\033[1;0m\n");
        return 0;
    }
    return 1;
}