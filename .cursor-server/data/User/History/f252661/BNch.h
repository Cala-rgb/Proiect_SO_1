#ifndef HUB_COMMANDS_H
#define HUB_COMMANDS_H

#include "common.h"
#include "handle_signals.h"

// Helper functions
const char* get_working_dir(void);
FILE* openFile(void);
void read_from_pipe(monitor_t *monitor);

// Monitor control functions
void start_monitor(monitor_t *monitor);
void stop_monitor(monitor_t *monitor);

// Monitor command functions
void list_hunts(monitor_t *monitor);
void view_treasure(monitor_t *monitor);
void list_treasures(monitor_t *monitor);
void calculate_score(monitor_t *monitor);

#endif