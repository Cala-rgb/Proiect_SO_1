#ifndef HANDLE_SIGNALS_H
#define HANDLE_SIGNALS_H

#include "common.h"

// Function to set the current monitor for signal handlers
void set_monitor(monitor_t *monitor);

// Signal handlers
void handle_sigterm(void);
void handle_usr(void);
void sigchld_handler(int signo);

#endif // HANDLE_SIGNALS_H