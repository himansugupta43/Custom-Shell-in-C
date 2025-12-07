#ifndef __NEONATE_H
#define __NEONATE_H

int print_pid_of_latest_process_in_interval(char** argument_tokens);
void die(const char *s);
void disableRawMode();
void enableRawMode();
int neonate(int time_in_seconds);

#endif