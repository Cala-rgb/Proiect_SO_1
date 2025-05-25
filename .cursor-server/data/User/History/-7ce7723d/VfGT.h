#ifndef HANDLE_SIGNALS_H
#define HANDLE_SIGNALS_H

#include "common.h"

// Signal handlers setup
void handle_sigterm(void);
void handle_sigchld(void);
void handle_usr(void);

#endif // HANDLE_SIGNALS_H