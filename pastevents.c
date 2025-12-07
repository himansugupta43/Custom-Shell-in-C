#include "headers.h"

int history(char** argument_tokens, int no_of_arguments, int* pastevents_present, int ip, char* curr_command, int store, int* pastevents_execute_present, char* input_string) {
    *pastevents_present = 1;    // flag to denote pastevents command is executed
    if (no_of_arguments == 0) { // no arguments are passed than just print the pastevents
        return pastevents();
    } else {                    // if some argument is present
        if (strcmp(argument_tokens[1], "purge") == 0) {
            if (no_of_arguments > 1) {
                fprintf(stderr, "\033[1;31mpastevents: invalid arguments\033[1;0m\n");
                return 0;
            }
            // clears the stored list
            return purge();
        } else if (strcmp(argument_tokens[1], "execute") == 0) { // execute some pastevent whose event is given
            if (no_of_arguments == 1 && ip == 0) {
                // if no index is given which command to execute then show error
                fprintf(stderr, "\033[1;31mpastevents: missing argument in \"%s\"\033[1;0m\n", curr_command);
                return 0;
            } else if (no_of_arguments == 1 && ip == 1) {
                char number[MAX_LEN] = {0};
                int num = 0;    // variable to store the index of command to execute
                int flag = 1;   // valid index is passed between 1 and 15 (both inclusive)

                char inp_buff[999999] = {0};

                if (read(STDIN_FILENO, inp_buff, 999998) < 0) {
                    fprintf(stderr, "\033[1;31mpastevents : Error in reading\033[1;0m\n");
                    return 0;
                }

                if (inp_buff[strlen(inp_buff) - 1] == '\n') {
                    inp_buff[strlen(inp_buff) - 1] = '\0';
                }
                strcpy(number, inp_buff);
                // converting char to int
                // doing it the long way since some invalid string can also be given in argument so i had to handle it manually and not use atoi()
                convert_to_int(number, &num, &flag);
                if (flag) {
                    // executing the command at the given index
                    if (execute(num, store, curr_command, pastevents_execute_present, input_string) == 0) {
                        return 0;
                    }
                }
            } else if (no_of_arguments == 2) {
                char* number = argument_tokens[2];
                int num = 0;    // variable to store the index of command to execute
                int flag = 1;   // valid index is passed between 1 and 15 (both inclusive)
                
                // converting char to int
                // doing it the long way since some invalid string can also be given in argument so i had to handle it manually and not use atoi()
                convert_to_int(number, &num, &flag);
                if (flag) {
                    // executing the command at the given index
                    if (execute(num, store, curr_command, pastevents_execute_present, input_string) == 0) {
                        return 0;
                    }
                }
            } else {
                // excess of arguments are passed
                fprintf(stderr, "\033[1;31mpastevents : excess arguments in \"%s\"\033[1;0m\n", curr_command);
                return 0;
            }
        } else {
            // invalid arguments are passed
            fprintf(stderr, "\033[1;31mpastevents : invalid arguments in \"%s\"\033[1;0m\n", curr_command);
            return 0;
        }
    }
    return 1;
}

// Stores the input command in past_commands.txt file
int store_command(char* command) {
    char past_commands_path[MAX_LEN];
    strcpy(past_commands_path, home_directory);
    strcat(past_commands_path, "/");
    strcat(past_commands_path, "past_commands.txt");
    FILE *fptr;
    fptr = fopen(past_commands_path, "r");

    if (fptr == NULL) { // if past_commands.txt file does not exist and we are creating it for the first time
        fptr = fopen(past_commands_path, "w");
        fprintf(fptr, "%s\n", command);
        fclose(fptr);
    } else {
        // reading 15 commands at max from past_commands.txt
        char** past_commands = (char**) calloc(15, sizeof(char*));
        int write = 1;
        for (int i = 0; i < 15; i++) {
            past_commands[i] = (char*) calloc(MAX_LEN, sizeof(char));
            fscanf(fptr, " %[^\n]", past_commands[i]);

            if (i == 0) {
                remove_leading_and_trailing_spaces(command);
                remove_leading_and_trailing_spaces(past_commands[0]);
                if (strcmp(command, past_commands[0]) == 0) {
                    write = 0;
                    break;
                }
            }
        }

        if (write == 0) {
            fclose(fptr);
            for (int i = 0; i < 15; i++) {
                free(past_commands[i]);
            }
            free(past_commands);
        } else {
            fclose(fptr);

            FILE *fptr2;
            fptr2 = fopen(past_commands_path, "w");
            if (fptr2 == NULL) {
                fprintf(stderr, "\033[1;31mpastevents : error opening file for writing\033[1;0m\n");
                return 0;
            } else {
                fprintf(fptr2, "%s\n", command);
                for (int i = 0; i < 14; i++) {
                    if (past_commands[i][0] != '\0') {
                        fprintf(fptr2, "%s\n", past_commands[i]);
                    } else {
                        break;
                    }
                }
                fclose(fptr2);
            }
            for (int i = 0; i < 15; i++) {
                free(past_commands[i]);
            }
            free(past_commands);
        }
    }
    return 1;
}

// Prints the latest 15 (or less) pastevents
int pastevents() {
    char past_commands_path[MAX_LEN] = {0};
    strcpy(past_commands_path, home_directory);
    strcat(past_commands_path, "/past_commands.txt");

    FILE *fptr;
    fptr = fopen(past_commands_path, "r");

    if (fptr == NULL) {
        fprintf(stderr, "\033[1;31mpastevents : fopen: Error while opening\033[1;0m\n");
        return 0;
    } else {
        char** past_commands = (char**) calloc(15, sizeof(char*));
        int write = 1;
        for (int i = 0; i < 15; i++) {
            past_commands[i] = (char*) calloc(MAX_LEN, sizeof(char));
            fscanf(fptr, " %[^\n]", past_commands[i]);
        }

        for (int i = 14; i >= 0; i--) {
            if (past_commands[i][0] != '\0'){
                printf("%s\n", past_commands[i]);
            }
        }

        for (int i = 0; i < 15; i++) {
            free(past_commands[i]);
        }
        free(past_commands);
    }
    return 1;
}

// Clears the past_commands.txt file
int purge() {
    FILE *fptr;
    // Generating 'past_commands.txt' file path
    char past_commands_path[MAX_LEN] = {0};
    strcpy(past_commands_path, home_directory);
    strcat(past_commands_path, "/past_commands.txt");

    // Opening file in write mode and then closing it (it will clear out all the previous contents in the file)
    fptr = fopen(past_commands_path, "w");
    if (fptr == NULL) {
        fprintf(stderr, "\033[1;31mpastevents : Error while opening file for writing\033[1;0m\n");
        return 0;
    }
    fclose(fptr);
    return 1;
}

// Executes a particular command from the history based on index [1, 15]
int execute(int num, int store, char* curr_command, int* pastevents_execute_present, char* input_string) {
    FILE *fptr;
    char past_commands_path[MAX_LEN] = {0};
    strcpy(past_commands_path, home_directory);
    strcat(past_commands_path, "/past_commands.txt");

    fptr = fopen(past_commands_path, "r");

    if (fptr == NULL) {
        fprintf(stderr, "\033[1;31mpastevents : fopen: File not foound\033[1;0m\n");
        return 0;
    } else {
        char** past_commands = (char**) calloc(15, sizeof(char*));

        for (int i = 0; i < 15; i++) {
            past_commands[i] = (char*) calloc(MAX_LEN, sizeof(char));
            fscanf(fptr, " %[^\n]", past_commands[i]);
        }
        fclose(fptr);

        if (past_commands[num - 1][0] == '\0') {
            fprintf(stderr, "\033[1;31mpastevents : Invalid Argument\033[1;0m\n");
            return 0;
        } else {
            char** tkns = generate_tokens(input_string, ' ');
            int y = 0;
            int pastevents_idx = -1;
            while (tkns[y] != NULL) {
                if (strcmp(tkns[y], "pastevents") == 0 && strcmp(tkns[y + 1], "execute") == 0) {
                    pastevents_idx = y;
                    break;
                }
                y++;
            }
            if (pastevents_idx == -1) {
                fprintf(stderr, "\033[1;31mpastevents : Error occured in replacement\033[1;0m\n");
                return 0;
            } else {
                char temp[MAX_LEN] = {0};
                for (int u = 0; u < y; u++) {
                    strcat(temp, " ");
                    strcat(temp, tkns[u]);
                }
                strcat(temp, " ");
                strcat(temp, past_commands[num - 1]);
                for (int u = y + 3; tkns[u] != NULL; u++) {
                    strcat(temp, " ");
                    strcat(temp, tkns[u]);
                }
                strcpy(input_string, temp);
            }
            *pastevents_execute_present = 1;
            input(past_commands[num - 1], store, 0);
        }

        for (int i = 0; i < 15; i++) {
            free(past_commands[i]);
        }
        free(past_commands);
    }
    return 1;
}

