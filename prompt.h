#ifndef __PROMPT_H
#define __PROMPT_H

int prompt();
int is_inside_home_directory();
char** get_list_of_commands(char* input);
void free_commands_list(char** list_of_commands);

#endif