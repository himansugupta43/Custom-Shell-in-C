# Shell Implementation in C

This is a C language implementation of a shell. The project covers various features and functionalities commonly found in Unix-like shells. Below are the details of the features implemented, along with certain assumptions and considerations.

## Features

1. **Basic System Calls:**
   - Display requirements: Shows the current path, user and command in prompt with color coding

2. **Warp Command:**
   - Changes the current working directory.
   - Supports symbols like ".", "..", "~", and "-".
   - Handles both absolute and relative paths.

3. **Peek Command:**
   - Lists files and directories in lexicographic order.
   - Supports flags like -a and -l for additional information.
   - Color-coded output for executables, files, and directories.

4. **Pastevents Command:**
   - Keeps track of the 15 most recent command statements.
   - Executes stored commands and purges the command history.

5. **System Commands:**
   - Executes other system commands present in Bash.
   - Works in both foreground and background processes.

6. **I/O Redirection:**
   - Supports >, <, and >> for input/output redirection.
   - Handles file creation, overwriting, and appending.

7. **Pipes:**
   - Supports pipes for passing information between commands.
   - Handles any number of pipes in sequential execution.

8. **Activities Command:**
   - Lists information about currently running processes.
   - Displays Command Name, PID, and state (running or stopped).

9. **Proclore Command:**
   - Provides detailed information about a process.
   - Displays PID, process status, process group, virtual memory, and executable path.

10. **Signals and Ping Command:**
    - Sends signals to processes using the `ping` command.
    - Handles signals for stopping, resuming, and killing processes.

11. **Foreground (fg) and Background (bg) Commands:**
    - Brings background processes to the foreground and vice versa.

12. **Neonate Command:**
    - Prints the Process-ID of the most recently created process at regular intervals until the 'x' key is pressed.

13. **iMan Command:**
    - Fetches and displays man pages from http://man.he.net/.

## Assumptions

### General:

- '&' and ';' are always used in a special sense.
- Absolute file paths always begin with a '/'.
- Multiple commands in `warp` are stored in `pastevents`.
- File paths do not contain spaces.
- Handling `ctrl + z` might not work with Makefile; manual compilation is suggested.
- `ctrl + d` only works when no foreground process is running.

### Warp:

- Does not support input redirection; changes the current working directory to home on input redirection.

### Peek:

- "peek -" gives an error.
- Does not accept input using input redirection '<'.

### Background Processes:

- If an invalid command is passed as a background process, it gets stored in `past_commands.txt`.

### Seek:

- Assumes that the base directory provided in relative paths to the current folder always starts with ".".
- If flag `-e` is provided and only one file is found, a maximum of 100000 characters will be read and printed as output.
- Only the path of the file to seek can be provided through input redirection '<'.

### Proclore:

- Considers all processes spawned by the shell as background processes.

### I/O Redirection:

- Errors printed in color on the terminal are not printed in color in the text file.

### Ping:

- Processes can only be blocked by `SIGSTOP = 19` signal through the `ping` command and resumed only through `SIGCONT = 18` and killed only using `SIGKILL = 9`.

### Piping:

- If multiple inputs and outputs are provided, priorities are given to '<', '>>', and '>'.

### Errors:

- If an error occurs in redirection, the error is only printed on the terminal and not redirected.

### Activities:

- Considers processes with status 'R' and 'S' as running, and 'T', 'D', and 'Z' as stopped.

### iMan:

- Assumes that the HTML format follows a specific structure.

### fg:

- When Vim is run in the background and brought to the foreground using `fg <pid>`, quitting Vim terminates the shell.

These assumptions provide context for the implementation and help in understanding the expected behavior of the shell. Users are encouraged to review these assumptions while working with the shell.
