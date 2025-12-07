#include "headers.h"

int system_command(char** argument_tokens, int no_of_arguments, char* curr_command) {
    if (strcmp(argument_tokens[0], "echo") == 0) {
        for (int i = 1; i <= no_of_arguments; i++) {
            if (argument_tokens[i] != NULL) {
                if (argument_tokens[i][0] == '"') {
                    for (int j = 0; j < strlen(argument_tokens[i]) - 1; j++) {
                        argument_tokens[i][j] = argument_tokens[i][j + 1];
                    }
                    argument_tokens[i][strlen(argument_tokens[i]) - 1] = '\0';
                } 
                if (argument_tokens[i][strlen(argument_tokens[i]) - 1] == '"') {
                    argument_tokens[i][strlen(argument_tokens[i]) - 1] = '\0';
                }
            } else {
                break;
            }
        }
    }

    bg_process = 0;

    if (curr_command[strlen(curr_command) - 1] == '&') {
        bg_process = 1;
        argument_tokens[no_of_arguments][strlen(argument_tokens[no_of_arguments]) - 1] = '\0';
    } else {
        bg_process = 0;
    }

    int ppid = getpid();
    int pid = fork();

    if (pid == 0) {
        if (bg_process == 1)  {
            setpgid(0, 0);
        }
        execvp(argument_tokens[0], argument_tokens);
        // execvp failed
        fprintf(stderr, "\033[1;31m%s : %s\033[1;0m\n", argument_tokens[0], strerror(errno));
        kill(getpid(), SIGTERM);
    } else if (pid > 0) {
        if (bg_process == 0) {
            global_fg_pid = pid;
            strcpy(fg_command_name, curr_command);

            int cstatus;
            waitpid(pid, &cstatus, WUNTRACED);

            global_fg_pid = -1;
            fg_command_name[0] = '\0';
        } else {
            bg_gpid = pid;
            printf("%d\n", pid);
            insert_in_LL(pid, -1, argument_tokens);
        }
    } else {
        fprintf(stderr, "\033[1;31mfork: could not fork\033[1;0m\n");
        return 0;
    }
    return 1;
}