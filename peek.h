#ifndef __PEEK_H
#define __PEEK_H

int look_into_file(char** argument_tokens, int no_of_arguments);
int peek(char* path, int a, int l);
char** generate_sorted_file_list(char* path);
int print_extra_details(char* complete_path_of_file, char** files_list, int idx);

#endif