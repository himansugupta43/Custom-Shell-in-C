#include "headers.h"

// Searches for a given file/directory recursively
int find_file_dir(char** argument_tokens, int no_of_arguments, int ip) {
    char base_dir[MAX_LEN] = {0};   // stores the path to base dir
    char file_name[MAX_LEN] = {0};  // stores the name of the file to be searched

    // flags
    int d = 0;
    int f = 0;
    int e = 0;

    int base_dir_flag = 0;  // 1 if base directory path (relative to cwd) is provided, 0 otherwise
    int valid_flags = 1;    // 1 if flags provided are all valid, 0 otherwise
    int file_name_flag = 0; // 1 if file name is provided, 0 otherwise

    int success = 1;

    if (no_of_arguments == 0) {
        // file/directory name to be searched should be provided
        fprintf(stderr, "\033[1;31mseek: missing arguments\033[1;0m\n");
        return 0;
    } else {
        for (int i = 1; i <= no_of_arguments; i++) {
            char* curr_argument = argument_tokens[i];

            if (curr_argument[0] == '-') {  // if argument starts with a '-' it is considered to be a flag
                if (base_dir_flag == 1) {   // if base directory is already provided
                    // flags should be provided before providing the path to the base directory
                    fprintf(stderr, "\033[1;31mseek: Invalid Arguments\033[1;0m\n");
                    return 0;
                } else {
                    // checking for flags present
                    if (strcmp(curr_argument, "-d") == 0) {
                        d = 1;
                    } else if (strcmp(curr_argument, "-f") == 0) {
                        f = 1;
                    } else if (strcmp(curr_argument, "-e") == 0) {
                        e = 1;
                    } else if (strcmp(curr_argument, "-de") == 0 || strcmp(curr_argument, "-ed") == 0) {
                        d = 1;
                        e = 1;
                    } else if (strcmp(curr_argument, "-fe") == 0 || strcmp(curr_argument, "-ef") == 0) {
                        f = 1;
                        e = 1;
                    } else {
                        // if any other flag is provided other than the seven mentioned
                        fprintf(stderr, "\033[1;31mseek: Invalid Flag\033[1;0m\n");
                        return 0;
                    }
                }
            } else if (curr_argument[0] == '.') { // if path relative to current directory is provided
                // assuming the relative path always starts with a .
                base_dir_flag = 1;  // base_directory path provided
                if (ip == 0) {
                    strcpy(base_dir, curr_argument);
                } else if (ip == 1) {   // if input is to be taken from a file
                    char inp_buff[999999] = {0};
                        if (read(STDIN_FILENO, inp_buff, 999998) < 0) {
                            fprintf(stderr, "\033[1;31mError in reading\033[1;0m\n");
                            return 0;
                        } else {
                            // removing the last endline character
                            if (inp_buff[strlen(inp_buff) - 1] == '\n') {
                                inp_buff[strlen(inp_buff) - 1] = '\0';
                            }
                            strcpy(base_dir, inp_buff);
                        }
                }
            } else if (curr_argument[0] == '/') { // absolute path is provided
                strcpy(base_dir, curr_argument);
                base_dir_flag = 1;
            } else {
                if (file_name_flag == 1) {
                    fprintf(stderr, "\033[1;31mseek: invalid arguments\033[1;0m\n");
                    return 0;
                } else {
                    file_name_flag = 1;
                    strcpy(file_name, curr_argument);
                }
            }
        }
    }

    if (valid_flags == 1 && file_name_flag == 1) {
        if (d == 1 && f == 1) {
            // both d and f flags cannot be provided simultaneously
            fprintf(stderr, "\033[1;31mInvalid flags\033[1;0m\n");
            return 0;
        } else {
            // iterating through all the files in the path
            linked_list_head paths = create_linked_list_head();
            char* path_to_base_dir;
            if (base_dir_flag) {
                if (base_dir[0] == '/') {
                    path_to_base_dir = (char*) calloc(MAX_LEN, sizeof(char));
                    strcpy(path_to_base_dir, base_dir);
                } else {
                    path_to_base_dir = generate_new_path(base_dir);
                }
            } else {
                path_to_base_dir = (char*) calloc(MAX_LEN, sizeof(char));
                strcpy(path_to_base_dir, cwd);
            }

            // seeking in each file/folder recursively
            seek(path_to_base_dir, file_name, paths);

            if (paths->number_of_nodes == 0) {
                printf("No match found!\n");
            } else {
                int no_of_files = 0;
                int no_of_dir = 0;

                // iterating through the list and calculating the number of files and directories
                linked_list_node temp = paths->first;
                while (temp != NULL) {
                    struct stat dir_stat_temp;
                    lstat(temp->path, &dir_stat_temp);
                    if (S_ISDIR(dir_stat_temp.st_mode) != 0) {
                        no_of_dir++;
                    } else {
                        no_of_files++;
                    }   
                    temp = temp->next;
                }

                if (e) {
                    if ((d && (no_of_dir > 1)) || (f && (no_of_files > 1))) {
                        // do nothing
                    } else {
                        if (d) {
                            linked_list_node trav = paths->first;
                            while (trav != NULL) {
                                struct stat dir_stat;
                                lstat(trav->path, &dir_stat);
                                if (S_ISDIR(dir_stat.st_mode) != 0) { // checking if it's a dir
                                    if (dir_stat.st_mode & S_IXUSR) {
                                        printf("\033[1;34m%s\033[1;0m\n", relative_path(trav->path, path_to_base_dir));
                                        strcpy(prev_dir, cwd);
                                        strcpy(cwd, trav->path);
                                    } else {
                                        fprintf(stderr, "\033[1;31mMissing permissions for task!\033[1;0m\n");
                                    }
                                }
                                trav = trav->next;
                            }
                        } else if (f) {
                            linked_list_node trav = paths->first;
                            while (trav != NULL) {
                                struct stat dir_stat;
                                lstat(trav->path, &dir_stat);
                                if (S_ISDIR(dir_stat.st_mode) == 0) { // checking if it's a file
                                    if (dir_stat.st_mode & S_IRUSR) {
                                        printf("\033[1;32m%s\033[1;0m\n", relative_path(trav->path, path_to_base_dir));
                                        char buffer[100000];
                                        int fd = open(trav->path, O_RDONLY);
                                        if (fd < 0) {
                                            fprintf(stderr, "\033[1;31mopen: Could not open file for reading\033[1;0m\n");
                                            return 0;
                                        } else {
                                            if (read(fd, buffer, 100000 - 1) < 0) {
                                                fprintf(stderr, "\033[1;31mread: could not read data\033[1;0m\n");
                                                close(fd);
                                                return 0;
                                            } else {
                                                printf("%s\n", buffer);
                                                close(fd);
                                            }
                                        }
                                    } else {
                                        printf("Missing permissions for task!\n");
                                    }
                                }
                                trav = trav->next;
                            }
                        } else {
                            if (paths->number_of_nodes > 1) {
                                // do nothing
                            } else {
                                struct stat dir_stat;
                                lstat(paths->first->path, &dir_stat);
                                if (S_ISDIR(dir_stat.st_mode) == 0) { // checking if it's a file
                                    if (dir_stat.st_mode & S_IRUSR) {
                                        printf("\033[1;32m%s\033[1;0m\n", relative_path(paths->first->path, path_to_base_dir));
                                        char buffer[100000];
                                        int fd = open(paths->first->path, O_RDONLY);
                                        if (fd < 0) {
                                            fprintf(stderr, "\033[1;31mopen: Could not open file for reading\033[1;0m\n");
                                            return 0;
                                        } else {
                                            if (read(fd, buffer, 100000 - 1) < 0) {
                                                fprintf(stderr, "\033[1;31mread: could not read data\033[1;0m\n");
                                                close(fd);
                                                return 0;
                                            } else {
                                                printf("%s\n", buffer);
                                                close(fd);
                                            }
                                        }
                                    } else {
                                        printf("Missing permissions for task!\n");
                                    }
                                } else {
                                    if (dir_stat.st_mode & S_IXUSR) {
                                        printf("\033[1;34m%s\033[1;0m\n", relative_path(paths->first->path, path_to_base_dir));
                                        strcpy(prev_dir, cwd);
                                        strcpy(cwd, paths->first->path);
                                    } else {
                                        fprintf(stderr, "\033[1;31mMissing permissions for task!\033[1;0m\n");
                                    }
                                }
                            }
                        }
                    }
                } else {
                    if (f) {
                        traverse_and_print(paths, 1, 0, path_to_base_dir);
                    } else if (d) {
                        traverse_and_print(paths, 0, 1, path_to_base_dir);
                    } else {
                        traverse_and_print(paths, 1, 1, path_to_base_dir);
                    }
                }
            }
            free(path_to_base_dir);
            free_linked_list(paths);
        }
    }

    return 1;
}

void seek(char* path_to_base_dir, char* file_name, linked_list_head paths) {
    struct stat dir_stat;
    stat(path_to_base_dir, &dir_stat);

    if (S_ISDIR(dir_stat.st_mode) == 0) {
        return;
    }

    DIR *dr;
    struct dirent *en;

    dr = opendir(path_to_base_dir);
    if (dr) {
        while ((en = readdir(dr)) != NULL) {
            if (en->d_name[0] != '.') {
                char* next_file_path = (char*) calloc(MAX_LEN, sizeof(char));
                strcpy(next_file_path, path_to_base_dir);
                update_path(next_file_path, en->d_name);

                seek(next_file_path, file_name, paths);

                char* new_file_name = remove_extension(file_name);
                char* curr_file_name = remove_extension(en->d_name);
                if (strcmp(new_file_name, curr_file_name) == 0) {
                    insert_in_linked_list(paths, next_file_path);
                }
                free(curr_file_name);
                free(next_file_path);
                free(new_file_name);
            }
        }
        closedir(dr);
    }
    return;
}

