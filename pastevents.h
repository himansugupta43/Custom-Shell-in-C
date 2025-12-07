#ifndef __PASTEVENTS_H
#define __PASTEVENTS_H

int history(char** argument_tokens, int no_of_arguments, int* pastevents_present, int ip, char* curr_command, int store, int* pastevents_execute_present, char* input_string);
int store_command(char* command);
int pastevents();
int purge();
int execute(int num, int store, char* curr_command, int* is_pastevents_execute_present, char* input_string);

#endif