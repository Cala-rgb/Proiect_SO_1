#ifndef HANDLE_SIGNALS_H
#define HANDLE_SIGNALS_H

#include "common.h"

// Function to set the current monitor for signal handlers
void set_monitor(monitor_t *monitor);

// Signal handlers setup
void handle_sigterm(void);
void handle_sigchld(void);
void handle_usr(void);

#endif // HANDLE_SIGNALS_H