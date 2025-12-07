#include "headers.h"

int handle_piping(char* curr_command) {
    char** list_of_commands_pipe = generate_tokens(curr_command, '|');

    int num_of_commands = 0;
    while (list_of_commands_pipe[num_of_commands] != NULL) {
        num_of_commands++;
    }

    int num_pipes = num_of_commands - 1;

    // 0 - read
    // 1 - write
    int pipe_fds[num_pipes][2];
    for (int i = 0; i < num_pipes; i++) {
        if (pipe(pipe_fds[i]) < 0) {
            fprintf(stderr, "033[1;31mError occured while piping\033[1;0m\n");
            return 0;
        }
    }

    for (int i = 0; i < num_of_commands; i++) { // running each sub command
        char* curr_pipe_command = list_of_commands_pipe[i];

        int pid_i = fork();                     // pid for ith command
        if (pid_i < 0) {
            fprintf(stderr, "033[1;31merror while fork\033[1;0m\n");
            return 0;
        } else if (pid_i == 0) {
            char** argument_tokens = generate_tokens(curr_pipe_command, ' ');

            char* curr_pipe_command_name = argument_tokens[0];

            if (i == 0) {
                for (int j = 1; j < num_pipes; j++) {
                    close(pipe_fds[j][0]);
                    close(pipe_fds[j][1]);
                }

                close(pipe_fds[0][0]);
                dup2(pipe_fds[0][1], STDOUT_FILENO);
                close(pipe_fds[0][1]);
            } else if (i > 0 && i < num_of_commands - 1) {
                for (int j = 0; j < num_pipes; j++) {
                    if (j == i - 1 || j == i) continue;
                    close(pipe_fds[j][0]);
                    close(pipe_fds[j][1]);
                }

                // pipe number to take input = i - 1
                close(pipe_fds[i - 1][1]); // closing the write end of pipe i - 1
                // pipe number to write output = i
                close(pipe_fds[i][0]);    // closing the read end of pipe i

                dup2(pipe_fds[i - 1][0], STDIN_FILENO);
                close(pipe_fds[i - 1][0]);

                dup2(pipe_fds[i][1], STDOUT_FILENO);
                close(pipe_fds[i][1]);
            } else if (i == num_of_commands - 1) {
                for (int j = 0; j < num_pipes - 1; j++) {
                    close(pipe_fds[j][0]);
                    close(pipe_fds[j][1]);
                }

                close(pipe_fds[num_pipes - 1][1]);
                dup2(pipe_fds[num_pipes - 1][0], STDIN_FILENO);
                close(pipe_fds[num_pipes - 1][0]);
            }

            // input redirection '<'
            int inp_flag = is_input_present(curr_command);

            // output redirection '>'
            // note that append can also be present when write_flag is 1
            int write_flag = is_write_present(curr_command);

            // output redirection '>>'
            char* app_cmd = (char*) calloc(MAX_LEN, sizeof(char));
            char* to_file = (char*) calloc(MAX_LEN, sizeof(char));
            int append_flag = is_append_present(curr_command, app_cmd, to_file);

            int redirection_present = (inp_flag || write_flag || append_flag);

            if (strcmp(curr_pipe_command_name, "iMan") == 0 || strcmp(curr_pipe_command_name, "warp") == 0 || strcmp(curr_pipe_command_name, "peek") == 0 || strcmp(curr_pipe_command_name, "pastevents") == 0 || strcmp(curr_pipe_command_name, "proclore") == 0 || strcmp(curr_pipe_command_name, "seek") == 0 || strcmp(curr_pipe_command_name, "exit") == 0 || strcmp(curr_pipe_command_name, "activities") == 0 || strcmp(curr_pipe_command_name, "ping") == 0 || strcmp(curr_pipe_command_name, "bg") == 0 || strcmp(curr_pipe_command_name, "fg") == 0 || strcmp(curr_pipe_command_name, "neonate") == 0 || redirection_present == 1) {
                input(curr_pipe_command, 0, 1);
                kill(getpid(), SIGTERM);
            } else {
                execvp(argument_tokens[0], argument_tokens);
                fprintf(stderr, "033[1;31merror : execvp\033[1;0m\n");
                kill(getpid(), SIGTERM);
            }
        }
    }

    for (int i = 0; i < num_pipes; i++) {
        close(pipe_fds[i][0]);
        close(pipe_fds[i][1]);
    }

    for (int i = 0; i < num_of_commands; i++) {
        // closing all pipes in parent execept the read of last pipe
        wait(NULL);
    }

    free_tokens(list_of_commands_pipe);
    return 1;
}