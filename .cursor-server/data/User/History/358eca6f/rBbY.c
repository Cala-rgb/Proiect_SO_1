#define _POSIX_SOURCE
#define _DEFAULT_SOURCE
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>
#include <errno.h>
#include "hub_commands.h"
#include "interface.h"
#include "common.h"
#include "handle_signals.h"

int main() {
    // Initialize monitor structure
    monitor_t monitor = {0};
    monitor.status = 0;  // Initially stopped

    // Set up signal handling
    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = sigchld_handler;
    sigaction(SIGCHLD, &sa, NULL);

    // Set the monitor for signal handlers
    set_monitor(&monitor);

    // Run the menu system
    return handle_menu(&monitor);
}