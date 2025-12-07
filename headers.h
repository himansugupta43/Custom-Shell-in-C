#ifndef __HEADERS_H_
#define __HEADERS_H_

#define MAX_LEN 10000

// Built in header files
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <dirent.h>
#include <time.h>
#include <pwd.h>
#include <grp.h>
#include <signal.h>
#include <fcntl.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <errno.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <termios.h>

// User defined header files
#include "prompt.h"
#include "warp.h"
#include "peek.h"
#include "utilities.h"
#include "pastevents.h"
#include "input.h"
#include "seek.h"
#include "linked_list.h"
#include "LL2.h"
#include "proclore.h"
#include "ping.h"
#include "activities.h"
#include "signal_handlers.h"
#include "iMan.h"
#include "neonate.h"
#include "bg.h"
#include "fg.h"
#include "sys_commands.h"
#include "pipe.h"

#endif