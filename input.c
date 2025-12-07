#include "headers.h"

int bg_process = 0; // Flag to mark if a process is background process or not
int start = 0;      // Variable to hold the start time of command execution
time_t tyme = 0;    // Variable to hold the time of execution

int global_fg_pid; // Stores the pid of the currently running foreground process

void input(char *command, int store, int ip)
{
    global_fg_pid = -1; // -1 represents no foreground process is initiated by my terminal

    tyme = time(NULL) - start; // current time - start time
    t = tyme;                  // global variable to hold the time of last executed command

    start = 0; // reset start time to zero
    tyme = 0;  // reset execution time to zero

    int overall_success = 1;    // flag to mark if the complete command ran without failing
    int pastevents_present = 0; // flag to mark if pastevents command is present
    int pastevents_execute_present = 0;

    char input_string[5000] = {0};

    if (command == NULL)
    {              // command is NULL if the input function is called from main and not from pastevents execute function
        store = 1; // flag to reflect if the command inputted is to be stored or not

        // Print appropriate prompt with username, systemname and directory before accepting input
        int exit_status = prompt();
        if (exit_status == 0)
        {
            fprintf(stderr, "\033[1;31mprompt: Could Not Print prompt\033[1;0m\n");
            return;
        }

        // when we press ctrl + d fgets returns NULL
        if (fgets(input_string, 4096, stdin) == NULL)
        {
            handle_ctrl_d();
        }
        input_string[strlen(input_string)] = '\0';

        // removing endline character
        if (input_string[strlen(input_string) - 1] == '\n')
        {
            input_string[strlen(input_string) - 1] = '\0';
        }

        // check through the linked list of all the background processes that were running and print the ones which are done
        check_and_print();

        if (bg_process_buffer[0] != '\0')
        {
            printf("%s", bg_process_buffer);
            bg_process_buffer[0] = '\0';
        }
    }
    else
    {              // input is called by pastevents execute
        store = 0; // don't store the command as it was called by pastevents execute
        strcpy(input_string, command);
    }

    last_command[0] = '\0';

    char **list_of_commands = get_list_of_commands(input_string); // contains the list of all commands as separate strings in the form of a 2D array

    start = time(NULL); // record the time before we start executing the commands

    int idx = 0; // index variable to hold the index of command to be executed
    char *curr_command = list_of_commands[idx];

    while (curr_command[0] != '\0')
    {
        // Checking if the command involves piping
        int pipe_flag = is_pipe_present(curr_command);

        if (pipe_flag == 1)
        {
            overall_success = handle_piping(curr_command);
        }
        else
        {
            // output redirection '>>'
            // app_cmd >> to_file
            char app_cmd[MAX_LEN] = {0};
            char to_file[MAX_LEN] = {0};
            int append_flag = is_append_present(curr_command, app_cmd, to_file);

            // output redirection '>'
            // NOTE that when even '>>' is present is_write_returns 1 so we need to recheck
            int write_flag = is_write_present(curr_command);

            // input redirection '<'
            int inp_flag = is_input_present(curr_command);

            if (inp_flag == 1)
            {
                char **files = generate_tokens(curr_command, '<');
                if (write_flag == 0 && append_flag == 0)
                {
                    char *cmd = files[0];

                    remove_leading_and_trailing_spaces(cmd);
                    // checking for pastevents
                    char **tokens = generate_tokens(cmd, ' ');
                    int k = 0;
                    while (tokens[k] != NULL)
                    {
                        if (strcmp(tokens[k], "pastevents") == 0)
                        {
                            pastevents_present = 1;
                            break;
                        }
                        k++;
                    }
                    free_tokens(tokens);

                    char *inp_file = files[1];

                    char inp_file_path[MAX_LEN] = {0};
                    if (inp_file[0] == '/')
                    {
                        strcpy(inp_file_path, inp_file);
                    }
                    else
                    {
                        strcpy(inp_file_path, cwd);
                        strcat(inp_file_path, "/");
                        strcat(inp_file_path, inp_file);
                    }

                    int fd_in = open(inp_file_path, O_RDONLY);
                    if (fd_in < 0)
                    {
                        fprintf(stderr, "\033[1;31mopen : File not found\033[1;0m\n");
                        return;
                    }
                    dup2(fd_in, STDIN_FILENO);
                    close(fd_in);

                    input(cmd, 1, 1);
                }
                else
                {
                    if (append_flag == 1)
                    {
                        // assuming of the form cmd < input_file >> output_file
                        char app_cmd2[MAX_LEN] = {0};
                        char to_file2[MAX_LEN] = {0};
                        is_append_present(files[1], app_cmd2, to_file2);

                        char *inp_file = app_cmd2;
                        char *out_file = to_file2;
                        char *cmd = files[0];

                        char inp_file_path[MAX_LEN] = {0};
                        if (inp_file_path[0] == '/')
                        {
                            strcpy(inp_file_path, inp_file);
                        }
                        else
                        {
                            strcpy(inp_file_path, cwd);
                            strcat(inp_file_path, "/");
                            strcat(inp_file_path, inp_file);
                        }

                        char out_file_path[MAX_LEN] = {0};
                        if (out_file[0] == '/')
                        {
                            strcpy(out_file_path, out_file);
                        }
                        else
                        {
                            strcpy(out_file_path, cwd);
                            strcat(out_file_path, "/");
                            strcat(out_file_path, out_file);
                        }

                        int fd_in = open(inp_file_path, O_RDONLY);
                        if (fd_in < 0)
                        {
                            fprintf(stderr, "\033[1;31mopen : File not found\033[1;0m\n");
                            return;
                        }

                        int fd_out = open(out_file_path, O_WRONLY | O_CREAT | O_APPEND, 0644);
                        if (fd_out < 0)
                        {
                            fprintf(stderr, "\033[1;31mopen : File not found\033[1;0m\n");
                            return;
                        }

                        dup2(fd_in, STDIN_FILENO);
                        dup2(fd_out, STDOUT_FILENO);
                        close(fd_in);
                        close(fd_out);

                        input(cmd, 1, 1);
                    }
                    else if (write_flag == 1)
                    {
                        // command of the form cmd < inp > out
                        char **tkns = generate_tokens(files[1], '>');

                        char *inp_file = tkns[0];
                        char *out_file = tkns[1];
                        char *cmd = files[0];

                        char inp_file_path[MAX_LEN] = {0};
                        if (inp_file_path[0] == '/')
                        {
                            strcpy(inp_file_path, inp_file);
                        }
                        else
                        {
                            strcpy(inp_file_path, cwd);
                            strcat(inp_file_path, "/");
                            strcat(inp_file_path, inp_file);
                        }

                        char out_file_path[MAX_LEN] = {0};
                        if (out_file[0] == '/')
                        {
                            strcpy(out_file_path, out_file);
                        }
                        else
                        {
                            strcpy(out_file_path, cwd);
                            strcat(out_file_path, "/");
                            strcat(out_file_path, out_file);
                        }

                        int fd_in = open(inp_file_path, O_RDONLY);
                        if (fd_in < 0)
                        {
                            fprintf(stderr, "\033[1;31mopen : File not found\033[1;0m\n");
                            return;
                        }

                        int fd_out = open(out_file_path, O_WRONLY | O_CREAT | O_TRUNC, 0644);
                        if (fd_out < 0)
                        {
                            fprintf(stderr, "\033[1;31mopen : File not found\033[1;0m\n");
                            return;
                        }

                        dup2(fd_in, STDIN_FILENO);
                        dup2(fd_out, STDOUT_FILENO);
                        close(fd_in);
                        close(fd_out);

                        input(cmd, 1, 1);
                    }
                }
                free_tokens(files);
            }
            else if (append_flag == 1)
            {
                // no input redirection and no append redirection (> and >> can't come together) only write redirection
                // just the >> command
                char *cmd = app_cmd;
                char *out_file = to_file;

                char out_file_path[MAX_LEN] = {0};
                if (out_file[0] == '/')
                {
                    strcpy(out_file_path, out_file);
                }
                else
                {
                    strcpy(out_file_path, cwd);
                    strcat(out_file_path, "/");
                    strcat(out_file_path, out_file);
                }

                int fd_out = open(out_file_path, O_WRONLY | O_CREAT | O_APPEND, 0644);
                if (fd_out < 0)
                {
                    fprintf(stderr, "\033[1;31mopen : File not found\033[1;0m\n");
                    return;
                }

                dup2(fd_out, STDOUT_FILENO);
                close(fd_out);

                input(cmd, 1, 1);
            }
            else if (write_flag == 1)
            {
                // Just the write flag is present
                char **tkns = generate_tokens(curr_command, '>');
                char *cmd = tkns[0];
                char *out_file = tkns[1];

                char out_file_path[MAX_LEN] = {0};
                if (out_file[0] == '/')
                {
                    strcpy(out_file_path, out_file);
                }
                else
                {
                    strcpy(out_file_path, cwd);
                    strcat(out_file_path, "/");
                    strcat(out_file_path, out_file);
                }

                int fd_out = open(out_file_path, O_WRONLY | O_CREAT | O_TRUNC, 0644);
                if (fd_out < 0)
                {
                    fprintf(stderr, "\033[1;31mopen : File not found\033[1;0m\n");
                    return;
                }

                dup2(fd_out, STDOUT_FILENO);
                close(fd_out);

                input(cmd, 1, 1);

                free_tokens(tkns);
            }
            else
            {
                // tokenize the command based on strings
                char **argument_tokens = generate_tokens(curr_command, ' ');

                // calculating the number of arguments passed to the command
                int no_of_arguments = 0;
                while (argument_tokens[no_of_arguments] != NULL)
                {
                    no_of_arguments++;
                }
                // Number of arguments is one less than the total number of tokens because,
                // the first token will be the command name and rest will be arguments
                no_of_arguments--;

                // Different commands
                // ===================================================================================
                // iMan
                /*
                    DESCRIPTION:
                        Reads the Man pages of the specified command from man.he.net website and prints it

                    SYNTAX :
                        iMan <name of command>
                */

                // checking if iMan command is present
                if (strcmp("iMan", argument_tokens[0]) == 0)
                {
                    if (iMan(argument_tokens, no_of_arguments, ip) == 0)
                        overall_success = 0;
                }
                // ===================================================================================
                // warp
                /*
                    DESCRIPTION:
                        Changes the current working directory to the relative/absolute path provided

                    SYNTAX:
                        warp [<relative/absolute path to directory>, ...]
                */

                // checking if warp command is present
                else if (strcmp("warp", argument_tokens[0]) == 0)
                {
                    if (change_cwd(argument_tokens, no_of_arguments, ip) == 0)
                        overall_success = 0;
                }
                // ===================================================================================
                // peek
                /*
                    DESCRIPTION:
                        Prints the files/directories present in the path of file provided
                        flags: -l: Displays extra information
                               -a: Displays all files, including hidden files

                    SYNTAX: peek <flags> <path/name>
                */

                // checking if peek command is present
                else if (strcmp("peek", argument_tokens[0]) == 0)
                {
                    if (look_into_file(argument_tokens, no_of_arguments) == 0)
                        overall_success = 0;
                }
                // ===================================================================================
                // pastevents
                /*
                DESCRIPTION:
                    pastevents             : Prints the last 15 commands executed
                    pastevents purge       : Clears History
                    pastevents execute <i> : Executes the last ith command executed (1 <= i <= 15)

                SYNTAX:
                    pastevents
                    pastevents purge
                    pastevents execute <int>
                */

                // checking if pastevents command is present
                else if (strcmp("pastevents", argument_tokens[0]) == 0)
                {
                    if (history(argument_tokens, no_of_arguments, &pastevents_present, ip, curr_command, store, &pastevents_execute_present, input_string) == 0)
                        overall_success = 0;
                }
                // ===================================================================================
                // proclore
                /*
                    DESCRIPTION:
                        Prints all the details about the process whose process id is passed as an argument.
                        If no pid is provided it prints the details about the current process (shell)

                    SYNTAX:
                        proclore <pid>
                */

                // checking if proclore command is present
                else if (strcmp("proclore", argument_tokens[0]) == 0)
                {
                    if (print_info_pid(argument_tokens, no_of_arguments, ip) == 0)
                        overall_success = 0;
                }
                // ===================================================================================
                // seek
                /*
                    DESCRIPTION:
                        Searches for a file/directory in the relative/absolute path provided
                        Searches the entire depth of the tree starting from the provided path
                        flags: -e: Checks if only one directory/file with the given name is found
                                (if file is found      : prints it's outpus)
                                (if directory is found : changes cwd to that directory)
                               -f: Checks only for files of specified name
                               -d: Checks only for directories of specified name
                        If no relative/absolute path to the base directory is provided it just searches in the cwd

                    SYNTAX:
                        seek <flags> <name of file to be searched> [<relative/absolute path to base directory>]
                */

                // checking if seek command is present
                else if (strcmp("seek", argument_tokens[0]) == 0)
                {
                    if (find_file_dir(argument_tokens, no_of_arguments, ip) == 0)
                        overall_success = 0;
                }
                // ===================================================================================
                // activities
                /*
                    DESCRIPTION:
                        Prints the list of all the background processes spawned by my shell which is currently
                        running or stopped in lexicographical order of pid

                    SYNTAX:
                        activities
                */

                // checking if activities command is present
                else if (strcmp("activities", argument_tokens[0]) == 0)
                {
                    if (print_active_processes_spawned_by_my_shell() == 0)
                        overall_success = 0;
                }
                // ===================================================================================
                // ping
                /*
                    DESCRIPTION:
                        Sends given signal signal to the process with the given pid

                    SYNTAX:
                        ping <pid> <signal number>
                */

                // checking if ping command is present
                else if (strcmp("ping", argument_tokens[0]) == 0)
                {
                    if (ping(argument_tokens, no_of_arguments) == 0)
                        overall_success = 0;
                }
                // ===================================================================================
                // bg
                /*
                    DESCRIPTION:
                        Changes the state of the background process whose pid is given to running if
                        it is blocked else leaves it as it is

                    SYNTAX:
                        bg <pid>
                */

                // checking if bg command is present
                else if (strcmp("bg", argument_tokens[0]) == 0)
                {
                    if (run_bg_process(argument_tokens, no_of_arguments) == 0)
                        overall_success = 0;
                }
                // ===================================================================================
                // fg
                /*
                    DESCRIPTION:
                        Brings the background process with the given pid to foreground

                    SYNTAX:
                        fg <pid>
                */

                // checking if fg command is present
                else if (strcmp("fg", argument_tokens[0]) == 0)
                {
                    if (bring_process_to_foreground(argument_tokens) == 0)
                        overall_success = 0;
                }
                // ===================================================================================
                // neonate
                /*
                    DESCRIPTION:
                        Prints the pid of the most recently created process in the system at regular
                        time intervals (provided as argument) and exits on pressing 'x' on keyboard

                    SYNTAX:
                        neonate -n <int>
                */

                // checking if neonate is present
                else if (strcmp("neonate", argument_tokens[0]) == 0)
                {
                    if (print_pid_of_latest_process_in_interval(argument_tokens) == 0)
                        overall_success = 0;
                }
                // ===================================================================================
                // exit
                /*
                    DESCRIPTION:
                        Exits the shell after ending all the running background processes spawned by the shell

                    SYNTAX:
                        exit
                */

                // checking if exit command is present
                else if (strcmp("exit", argument_tokens[0]) == 0)
                {
                    if (no_of_arguments > 1)
                    {
                        fprintf(stderr, "\033[1;31mInvalid command\033[1;0m\n");
                    }
                    else
                    {
                        LL_Node trav = LL->first;
                        while (trav != NULL)
                        {
                            kill(trav->pid, SIGKILL);
                            trav = trav->next;
                        }
                        if (store_command(curr_command) == 1)
                        {
                            exit(0);
                        }
                        else
                        {
                            fprintf(stderr, "\033[1;31mexit : Error occured while storing the command\033[1;0m\n");
                            return;
                        }
                    }
                }
                // ===================================================================================
                // system commands
                /*
                    DESCRIPTION:
                        Runs all other system commands using execvp
                */
                else
                {
                    if (system_command(argument_tokens, no_of_arguments, curr_command) == 0)
                        overall_success = 0;
                }
                // ===================================================================================
                if (overall_success == 1)
                {
                    // storing the name of commands in the last command array
                    if (strlen(last_command) == 0)
                    {
                        if (strcmp(argument_tokens[0], "pastevents") != 0)
                        {
                            strcpy(last_command, argument_tokens[0]);
                        }
                    }
                    else
                    {
                        if (strcmp(argument_tokens[0], "pastevents") != 0)
                        {
                            strcat(last_command, ", ");
                            strcat(last_command, argument_tokens[0]);
                        }
                    }
                }
                free_tokens(argument_tokens);
            }
        }
        idx++;
        curr_command = list_of_commands[idx];
    }

    if (store == 0 || (pastevents_present == 1 && pastevents_execute_present == 0) || overall_success == 0 || ip == 1)
    {
        // do not store
    }
    else
    {
        if (store_command(input_string) == 0)
        {
            free_commands_list(list_of_commands);
            return;
        }
    }
    free_commands_list(list_of_commands);
    return;
}