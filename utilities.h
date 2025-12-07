#ifndef __UTILITIES_H
#define __UTILITIES_H

extern char* global_buffer;
extern int global_buffer_empty;
extern int global_fg_pid;
extern char* fg_command_name;

extern char* home_directory;
extern char* cwd;
extern char* prev_dir;
extern char* last_command;
extern int t;

extern int shell_pid;
extern int bg_gpid;

void free_tokens(char** tokens);
char** generate_tokens(char* str, char c);
int check_if_dir(char* path, char* file_name);
int check_if_such_dir_exists(char* path, char* next_dir);
void update_path(char* path, char* next_dir);
void remove_leading_and_trailing_spaces(char* str);
char* generate_new_path(char* path);
void sort_strings(char** strings, int no_of_strings);
int str_to_int(char* str);
char* remove_extension(char* file_name);
char* relative_path(char* complete_path, char* base_dir_path);
void convert_to_int(char* number, int* num, int* flag);
int is_pipe_present(char* string);
int is_write_present(char* string);
int is_append_present(char* string, char* app_cmd, char* to_file);
int is_input_present(char* string);
int is_pastevents_execute_present(char* curr_command);

#endif