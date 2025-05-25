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

monitor_t m = {-1, 0, {-1, -1}};

monitor_t *getMainMonitor() {
    return &m;
}

int monitor_is_stopping() {
    if (m.status == 2) {
        return 1; // Stopping
    }
    return 0; // Not stopping
}

int main() {
    // Run the menu system
    return handle_menu(&m);
}