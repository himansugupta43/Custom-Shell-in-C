#include "headers.h"

int change_cwd(char** argument_tokens, int no_of_arguments, int ip) {
    if (ip == 1) {
        // if input redirection is provided to warp it just does nothing
        // warp does not accept input redirection
        return 1;
    } else {
        if (no_of_arguments == 0) { // if no argument is passed then warp to the home directory
            char* c = "~";
            // returns 1 if successfull and returns 0 if some error occured
            return warp(c);
        } else {                    // if multiple arguments are passed then warp to those arguments one by one (each argument treated as a separate command)
            int status = 1;
            for (int i = 1; i <= no_of_arguments; i++) {
                int exit_status = warp(argument_tokens[i]);
                status = status && exit_status;
            }
            return status;
        }
    }
    return 1;
}

int warp(char* path) {
    if (strlen(path) > 0) {
        // checking if absolute path - absolute path always starts with '/'
        if (path[0] == '/') {
            struct stat path_stat;
            if (stat(path, &path_stat) < 0) {
                // directory does not exist
                fprintf(stderr, "\033[1;31mwarp: no such directory exists: %s\033[1;0m\n", path);
                return 0;
            } else {
                // change prev to cwd
                for (int i = 0; i < strlen(cwd); i++) {
                    prev_dir[i] = cwd[i];
                }
                prev_dir[strlen(cwd)] = '\0';

                // change cwd to new path
                for (int i = 0; i < strlen(path); i++) {
                    cwd[i] = path[i];
                }
                cwd[strlen(path)] = '\0';
                // ============================================================================
                /*
                    Earlier i was not using chdir() function and was keeping track of 
                    cwd manually but then i realised that the system functions like ls etc. 
                    when called from my terminal would run inside the system terminal and
                    since it's pwd is not changed it would print results based on the file
                    where my program resides so afterwards i have added just this one line
                    to also change the pwd of the original terminal. I know after this i had
                    no need to keep track of paths manually but since i had implemented this
                    already i am not changing it. (So there may be places where i might have
                    just coded based on my earlier implementation)
                */ 
                chdir(path);
                // ============================================================================

                printf("%s\n", cwd);
            }
        } else {
            char* new_path = generate_new_path(path);
            if (new_path == NULL) {
                fprintf(stderr, "\033[1;31mwarp: no such directory exists: %s\033[1;0m\n", path);
                return 0;
            } else if (new_path[0] == '^') {
                // print nothing
                free(new_path);
                return 0;
            } else {
                // change prev to cwd
                for (int i = 0; i < strlen(cwd); i++) {
                    prev_dir[i] = cwd[i];
                }
                prev_dir[strlen(cwd)] = '\0';

                // change cwd to new path
                for (int i = 0; i < strlen(new_path); i++) {
                    cwd[i] = new_path[i];
                }
                cwd[strlen(new_path)] = '\0';
                // ============================================================================
                /*
                    Earlier i was not using chdir() function and was keeping track of 
                    cwd manually but then i realised that the system functions like ls etc. 
                    when called from my terminal would run inside the system terminal and
                    since it's pwd is not changed it would print results based on the file
                    where my program resides so afterwards i have added just this one line
                    to also change the pwd of the original terminal. I know after this i had
                    no need to keep track of paths manually but since i had implemented this
                    already i am not changing it. (So there may be places where i might have
                    just coded based on my earlier implementation)
                */ 
                chdir(new_path);
                // ============================================================================

                free(new_path);

                printf("%s\n", cwd);
            }
        }
    }

    return 1;
}

