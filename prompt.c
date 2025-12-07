#include "headers.h"

int prompt() {
    // Do not hardcode the prmopt
    // Getting the name of the current user
    register struct passwd *pw;
    pw = getpwuid(getuid());
    char* username = pw->pw_name;

    if (username == NULL) { // if there is no username then show the error
        fprintf(stderr, "\033[1;31musername: No username available\033[1;0m\n");
        return 0;
    } else {
        // Extracting the hostname
        char* hostname = (char*) malloc(sizeof(char) * (MAX_LEN + 1));
        gethostname(hostname, MAX_LEN);
        printf("<");

        /*
           returns -1 if we are outside the home directory, 
           returns -2 if cwd == home directory else returns 
           the index from where the current directory path 
           starts to differ from our home directory
        */
        int index_of_difference = is_inside_home_directory();
        
        if (index_of_difference != -1 && index_of_difference != -2) { // if currently we are inside the home directory only
            // print the relative path
            //========================= Method 1 =========================
            char* relative_path = &cwd[index_of_difference];
            if (t < 2 || t > 1000000000) {
                printf("\033[1;34m%s\033[1;0m@\033[1;35m%s\033[1;0m:\033[1;36m~%s\033[1;0m", username, hostname, relative_path);
            } else {
                printf("\033[1;34m%s\033[1;0m@\033[1;35m%s\033[1;0m:\033[1;36m~%s\033[1;0m \033[1;33m%s : %ds\033[1;0m", username, hostname, relative_path, last_command, t);
                t = 0;
            }

            //========================= Method 2 =========================
            /*
            char* relative_path = (char*) malloc(sizeof(char) * strlen(cwd)); // relative path can never be longer than absolute path
            relative_path[0] = '~';
            relative_path[1] = '/';
            int i;
            for (i = 2; cwd[index_of_difference] != '\0'; i++) {
                relative_path[i] = cwd[index_of_difference++];
            }
            relative_path[i] = '\0';
            printf("\033[1;34m%s\033[1;0m@\033[1;35m%s\033[1;0m:\033[1;36m~%s\033[1;0m", username, relative_path);
            free(relative_path);
            */

        } else if (index_of_difference == -2) { // cwd == home_directory
            if (t < 2 || t > 1000000000) {
                printf("\033[1;34m%s\033[1;0m@\033[1;35m%s\033[1;0m:\033[1;36m~\033[1;0m", username, hostname);
            } else {
                printf("\033[1;34m%s\033[1;0m@\033[1;35m%s\033[1;0m:\033[1;36m~\033[1;0m \033[1;33m%s : %ds\033[1;0m", username, hostname, last_command, t);
                t = 0;
            }

        } else {                                // We are outside home_directory
            // print the absolute path
            if (t < 2 || t > 1000000000) {
                printf("\033[1;34m%s\033[1;0m@\033[1;35m%s\033[1;0m:\033[1;36m~%s\033[1;0m", username, hostname, cwd);
            } else {
                printf("\033[1;34m%s\033[1;0m@\033[1;35m%s\033[1;0m:\033[1;36m~%s\033[1;0m \033[1;33m%s : %ds\033[1;0m", username, hostname, cwd, last_command, t);
                t = 0;
            }
        }

        printf("> "); 
        free(hostname);
    }
    return 1;
}

// Checks if we are currently inside home_directory or not
int is_inside_home_directory() {
    /*
        returns -1 if we are outside the home directory, 
        returns -2 if cwd == home directory else returns 
        the index from where the current directory path 
        starts to differ from our home directory
    */

    int index = 0;
    while(cwd[index] != '\0' && home_directory[index] != '\0') { // continue till one of the strings ends
        if (cwd[index] != home_directory[index]) {               // if cwd deviates from home_directory address before any of the string ends implies we are currently outside the home directory
            break;
        }
        index++;
    }

    if (cwd[index] == '\0' && home_directory[index] == '\0') {          // both the strings are equal which implies we are in the home directory 
        return -2;
    } else if (cwd[index] != '\0' && home_directory[index] == '\0') {   // home_directory address ended before cwd implying that we are currently inside home directory
        return index;
    } else {                                                            // if the loops end before any of the string terminates or cwd string ends befor home directory we know we are outsid the home directory
        return -1;
    }
}

char** get_list_of_commands(char* input) {
    int no_of_commands = 0;
    int q_flag = 0;
    for (int i = 0; i < strlen(input); i++) {
        if (input[i] == '"' || input[i] == '\'') {
            if (q_flag == 0) q_flag = 1;
            else q_flag = 0;
        }
        if ((input[i] == ';' || input[i] == '&') && q_flag == 0) {
            no_of_commands++;
        }
    }
    if (input[strlen(input) - 1] != ';' && input[strlen(input) - 1] != '&')
        no_of_commands++;
    char** list_of_commands = (char**) calloc((no_of_commands + 1), sizeof(char*));
    for (int i = 0; i < no_of_commands + 1; i++) {
        list_of_commands[i] = (char*) calloc(5000, sizeof(char));
        list_of_commands[i][0] = '\0';
    }
    int input_str_index = 0;
    int no_of_commands_extracted = 0;
    int buffer_index = 0;
    int flag = 0;
    int quotes_flag = 0;
    while (input[input_str_index] != '\0' && no_of_commands_extracted < no_of_commands) {
        char curr_char = input[input_str_index++];
        if (curr_char == '&' && quotes_flag == 0) {
            // use store and reset function
            flag = 0;
            // buffer[buffer_index] = '&';
            if (list_of_commands[no_of_commands_extracted][buffer_index - 1] == ' ') {
                list_of_commands[no_of_commands_extracted][buffer_index - 1] = '&';
                list_of_commands[no_of_commands_extracted][buffer_index] = '\0';
            } else {
                list_of_commands[no_of_commands_extracted][buffer_index] = '&';
                buffer_index++;
                list_of_commands[no_of_commands_extracted][buffer_index] = '\0';
            }
            remove_leading_and_trailing_spaces(list_of_commands[no_of_commands_extracted]);
            buffer_index = 0;
            no_of_commands_extracted++;
        } else if (curr_char == ';' && quotes_flag == 0) {
            flag = 0;
            // buffer[buffer_index] = '\0';
            list_of_commands[no_of_commands_extracted][buffer_index] = '\0';
            buffer_index = 0;
            remove_leading_and_trailing_spaces(list_of_commands[no_of_commands_extracted]);
            no_of_commands_extracted++;
        } else {
            if ((curr_char == ' ' || curr_char == '\t') && quotes_flag == 0) {
                if (flag == 0) {
                    list_of_commands[no_of_commands_extracted][buffer_index] = ' ';
                    buffer_index++;
                    flag = 1;
                } else {
                    continue;
                }
            } else {
                flag = 0;
                list_of_commands[no_of_commands_extracted][buffer_index] = curr_char;
                buffer_index++;
                if (curr_char == '"' || curr_char == '\'') {
                    if (quotes_flag == 0) quotes_flag = 1;
                    else quotes_flag = 0;
                }
            }
        }
    }
    if (no_of_commands_extracted < no_of_commands) {
        remove_leading_and_trailing_spaces(list_of_commands[no_of_commands_extracted]);
        no_of_commands_extracted++;
    }
    return list_of_commands;
}

// Frees the 2D commands list
void free_commands_list(char** list_of_commands) {
    int idx = 0;
    char* curr_command = list_of_commands[idx];
    while (curr_command[0] != '\0') {
        free(curr_command);
        idx++;
        curr_command = list_of_commands[idx];
    }
    free(curr_command);
    free(list_of_commands);
}