#include "headers.h"

int print_pid_of_latest_process_in_interval(char** argument_tokens) {
    if (argument_tokens[1] == NULL) {
        fprintf(stderr, "\033[1;31mneonate: Invalid argument\033[1;0m\n");
        return 0;
    } else {
        if (argument_tokens[1][0] == '-' && strcmp(argument_tokens[1], "-n") != 0) {
            fprintf(stderr, "\033[1;31mneonate : Invalid flag: must be -n\033[1;0m\n");
            return 0;
        }
        if (strcmp(argument_tokens[1], "-n") != 0) {
            fprintf(stderr, "\033[1;31mneonate : Invalid argument (-n missing)\033[1;0m\n");
            return 0;
        } else {
            if (argument_tokens[2] == NULL) {
                fprintf(stderr, "\033[1;31mneonate : missing argument (time)\033[1;0m\n");
                return 0;
            } else {
                int t_sec = atoi(argument_tokens[2]);
                return neonate(t_sec);
            }
        }
    }
    return 1;
}

void die(const char *s) {
    perror(s);
    return;
}

struct termios orig_termios;

void disableRawMode() {
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios) == -1)
        die("tcsetattr");
}

void enableRawMode() {
    // tcgetattr() gets the curretn terminal settings and we store it in a temporary variable (used to reset when raw mode is turned off)
    if (tcgetattr(STDIN_FILENO, &orig_termios) == -1) die("tcgetattr");
    atexit(disableRawMode);
    struct termios raw = orig_termios;
    raw.c_lflag &= ~(ICANON | ECHO);
    // Set minimum number of characters for non-blocking read
    raw.c_cc[VMIN] = 0;

    // Set a timeout for non-blocking read (in deciseconds)
    raw.c_cc[VTIME] = 1;
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) == -1) die("tcsetattr");
}

int neonate(int time_in_seconds) {
    if (time_in_seconds <= 0) {
        fprintf(stderr, "\033[1;31mneonate: Invalid time in seconds - should be a positive integer\033[1;0m\n");
        return 0;
    }

    char path_loadavg[] = "/proc/loadavg";
    char *inp = malloc(sizeof(char) * 100);
    char c;

    enableRawMode();

    while (1) {
        char read_buff[MAX_LEN];

        int fd = open(path_loadavg, O_RDONLY);
        if (fd < 0) {
            fprintf(stderr, "\033[1;31mopen: could not open file\033[1;0m\n");
            return 0;
        }

        if (read(fd, read_buff, MAX_LEN - 1) < 0) {
            fprintf(stderr, "\033[1;31mread: could not read from the file\033[1;0m\n");
            close(fd);
            return 0;
        }
        close(fd);

        char** fields = generate_tokens(read_buff, ' ');
        // 5th field is the pid of the latest created process
        int pid_latest = atoi(fields[4]);
        free_tokens(fields);

        setbuf(stdout, NULL);
        
        printf("%d\n", pid_latest);
        memset(inp, '\0', 100);
        int pt = 0;

        int start_time = time(NULL);
        
        int flag = 0;

        while(1) {
            if (read(STDIN_FILENO, &c, 1) == 1) {
                if (c == 'x') {
                    flag = 1;
                    break;
                }
            }
            int curr_time = time(NULL) - start_time;
            if (curr_time < time_in_seconds) {
                continue;
            } else {
                break;
            }
        }

        if (flag == 1) {
            break;
        }
    }
    disableRawMode();
    free(inp);
    return 1;
}