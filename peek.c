#include "headers.h"

int look_into_file(char** argument_tokens, int no_of_arguments) {
    char path[MAX_LEN] = {0}; // stores path of directory to peek into
                
    // copying cwd into path
    // if no argument is provided or input redirection is provided then just print the contents of the cwd
    // peek does not accept input redirection
    strcpy(path, cwd);

    int a = 0;  // 1 if 'a' flag is present, 0 otherwise
    int l = 0;  // 1 if 'l' flag is present, 0 otherwise

    if (no_of_arguments != 0) {
        // valid flags
        char minus_a[3] = "-a";
        char minus_l[3] = "-l";
        char minus_al[4] = "-al";
        char minus_la[4] = "-la";

        int path_flag = 0; // 1 if path to some directory is provided, 0 otherwise
        for (int i = 1; i <= no_of_arguments; i++) {
            // checking which all flags are present
            if (strcmp(argument_tokens[i], minus_a) == 0) {  // 'a' flag is present
                a = 1;
            } else if (strcmp(argument_tokens[i], minus_l) == 0) {  // 'l' flag is present
                l = 1;
            } else if (strcmp(argument_tokens[i], minus_la) == 0 || strcmp(argument_tokens[i], minus_al) == 0) {  // both 'a' and 'l' flags are passed
                a = 1;
                l = 1;
            } else {
                // any argument not starting with "-" is treated as some path given
                path_flag = 1;

                // copying the provided path into the path array replacing cwd
                strcpy(path, argument_tokens[i]);
            }
        }
    }
    // returns 1 if peeked successfully else returns 0
    return peek(path, a, l);
}

int peek(char* path, int a, int l) {
    char* new_path = (char*) calloc(MAX_LEN, sizeof(char));
    if (path[0] == '-') {
        fprintf(stderr, "\033[1;31mpeek: Invalid Flags\033[1;0m\n");
        return 0;
    } else if (path[0] == '\0') {
        for (int i = 0; i < strlen(cwd); i++) {
            path[i] = cwd[i];
        }
        path[strlen(cwd)] = '\0';
    } else if (path[0] == '/') {
        for (int i = 0; i < strlen(path); i++) {
            new_path[i] = path[i];
        }
        new_path[strlen(path)] = '\0';
    } else {
        new_path = generate_new_path(path);
        if (new_path == NULL) {
            fprintf(stderr, "\033[1;31mpeek: no such directory %s\033[1;0m\n", path);
            return 0;
        }
    }

    char** files_list = generate_sorted_file_list(new_path);

    char complete_path_of_file[MAX_LEN];
    for (int i = 0; i < strlen(new_path); i++) {
        complete_path_of_file[i] = new_path[i];
    }
    complete_path_of_file[strlen(new_path)] = '/';
    int start = strlen(new_path) + 1;
    if (a && l) {
        int idx = 0;
        while (files_list[idx] != NULL) {
            for (int j = start; j < start + strlen(files_list[idx]); j++) {
                complete_path_of_file[j] = files_list[idx][j - start];
            }
            complete_path_of_file[start + strlen(files_list[idx])] = '\0';
            int exit_status = print_extra_details(complete_path_of_file, files_list, idx);
            if (exit_status == 0) return 0;
            idx++;
        }
    } else if (a) {
        int idx = 0;
        while (files_list[idx] != NULL) {
            for (int j = start; j < start + strlen(files_list[idx]); j++) {
                complete_path_of_file[j] = files_list[idx][j - start];
            }
            complete_path_of_file[start + strlen(files_list[idx])] = '\0';

            struct stat file_info;
            stat(complete_path_of_file, &file_info);
            if (S_ISDIR(file_info.st_mode)) { // checking if directory
                // color blue
                printf("\033[1;34m%s\033[1;0m\n", files_list[idx]);
            } else if (file_info.st_mode & S_IXUSR) { // checking if a file is executable
                // color green
                printf("\033[1;32m%s\033[1;0m\n", files_list[idx]);
            } else { // else the file is regular
                // color white
                printf("%s\n", files_list[idx]);
            }
            idx++;
        }
    } else if (l) {
        char complete_path_of_file[MAX_LEN];
        for (int i = 0; i < strlen(new_path); i++) {
            complete_path_of_file[i] = new_path[i];
        }
        complete_path_of_file[strlen(new_path)] = '/';
        int start = strlen(new_path) + 1;

        int idx = 0;
        while (files_list[idx] != NULL) {
            if (files_list[idx][0] == '.') {
                idx++;
                continue;
            } else {
                for (int j = start; j < start + strlen(files_list[idx]); j++) {
                complete_path_of_file[j] = files_list[idx][j - start];
                }
                complete_path_of_file[start + strlen(files_list[idx])] = '\0';
                int exit_status = print_extra_details(complete_path_of_file, files_list, idx);
                if (exit_status == 0) return 0;
            }
            idx++;
        }
    } else {
        int idx = 0;
        while (files_list[idx] != NULL) {
            if (files_list[idx][0] == '.') {
                idx++;
                continue;
            }

            for (int j = start; j < start + strlen(files_list[idx]); j++) {
                complete_path_of_file[j] = files_list[idx][j - start];
            }
            complete_path_of_file[start + strlen(files_list[idx])] = '\0';

            struct stat file_info;
            stat(complete_path_of_file, &file_info);
            if (S_ISDIR(file_info.st_mode)) { // checking if directory
                // color blue
                printf("\033[1;34m%s\033[1;0m\n", files_list[idx]);
            } else if (file_info.st_mode & S_IXUSR) { // checking if a file is executable
                // color green
                printf("\033[1;32m%s\033[1;0m\n", files_list[idx]);
            } else { // else the file is regular
                // color white
                printf("%s\n", files_list[idx]);
            }
            idx++;
        }
    }
    free(new_path);
    free_tokens(files_list);
    return 1;
}

char** generate_sorted_file_list(char* path) {
    DIR *dr;
    struct dirent *en;

    int no_of_files = 0;

    dr = opendir(path);
    if (dr) {
        while ((en = readdir(dr)) != NULL) {
            no_of_files++;
        }
        closedir(dr);
    }

    char** files_list = (char**) calloc(no_of_files + 1, sizeof(char*));
    for (int i = 0; i < no_of_files; i++) {
        files_list[i] = (char*) calloc(MAX_LEN, sizeof(char));
    }
    files_list[no_of_files] = NULL;

    DIR *dr1;
    struct dirent *en1;

    int file_no = 0;

    dr1 = opendir(path);

    if (dr1) {
        while ((en1 = readdir(dr1)) != NULL) {
            for (int i = 0; i < strlen(en1->d_name); i++) {
                files_list[file_no][i] = en1->d_name[i];
            }
            files_list[file_no][strlen(en1->d_name)] = '\0';
            file_no++;
        }
        closedir(dr1);
    }

    sort_strings(files_list, no_of_files);
    return files_list;
}

int print_extra_details(char* complete_path_of_file, char** files_list, int idx) {
    struct stat file_stat;

    if (!stat(complete_path_of_file, &file_stat)) {
        printf((S_ISDIR(file_stat.st_mode))  ? "d" : "-");
        printf((file_stat.st_mode & S_IRUSR) ? "r" : "-");
        printf((file_stat.st_mode & S_IWUSR) ? "w" : "-");
        printf((file_stat.st_mode & S_IXUSR) ? "x" : "-");
        printf((file_stat.st_mode & S_IRGRP) ? "r" : "-");
        printf((file_stat.st_mode & S_IWGRP) ? "w" : "-");
        printf((file_stat.st_mode & S_IXGRP) ? "x" : "-");
        printf((file_stat.st_mode & S_IROTH) ? "r" : "-");
        printf((file_stat.st_mode & S_IWOTH) ? "w" : "-");
        printf((file_stat.st_mode & S_IXOTH) ? "x" : "-");
    } else {
        fprintf(stderr, "\033[1;31mpeek: error in stat\033[1;0m\n");
        return 0;
    }

    struct passwd *pw = getpwuid(file_stat.st_uid);
    struct group  *gr = getgrgid(file_stat.st_gid);

    // use '%hu' for mac and '%lu' for linux
    // use '%lld' for mac and '%ld' for linux
    printf(" %lu %s %s %ld ", file_stat.st_nlink, pw->pw_name, gr->gr_name, file_stat.st_size);

    time_t modificationTime = file_stat.st_mtime;

    // Convert time to a formatted string and print it
    char* time_string;
    time_string = ctime(&modificationTime);
    char month[4];
    char date[3];
    char hour[3];
    char mins[3];
    char year[5];
    for (int i = 4; i < 7; i++) {
        month[i - 4] = time_string[i];
    }
    month[3] = '\0';

    for (int i = 8; i < 10; i++) {
        date[i - 8] = time_string[i];
    }
    date[2] = '\0';

    for (int i = 11; i < 13; i++) {
        hour[i - 11] = time_string[i];
    }
    hour[2] = '\0';

    for (int i = 14; i < 16; i++) {
        mins[i - 14] = time_string[i];
    }
    mins[2] = '\0';

    for (int i = 20; i < 24; i++) {
        year[i - 20] = time_string[i];
    }
    year[4] = '\0';

    printf("%s %s %s %s ", month, date, hour, mins);

    if (S_ISDIR(file_stat.st_mode)) { // checking if directory
        // color blue
        printf("\033[1;34m%s\033[1;0m\n", files_list[idx]);

    } else if (file_stat.st_mode & S_IXUSR) { // checking if a file is executable
        // color green
        printf("\033[1;32m%s\033[1;0m\n", files_list[idx]);
        
    } else { // else the file is regular
        // color white
        printf("%s\n", files_list[idx]);
    }
    return 1;
}

