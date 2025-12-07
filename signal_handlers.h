#ifndef __SIGNAL_HANDLERS_H
#define __SIGNAL_HANDLERS_H

void handle_sigtstp(int sig);
void handle_sigint(int sig);
void handle_ctrl_d();

#endif