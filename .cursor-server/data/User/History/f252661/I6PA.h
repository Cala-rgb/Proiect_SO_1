#ifndef HUB_COMMANDS_H
#define HUB_COMMANDS_H

#include "common.h"
#include "handle_signals.h"

// Helper functions
const char* get_working_dir(void);
FILE* openFile(void);
void read_from_pipe(monitor_t *monitor);
monitor_t* getMainMonitor(void);

// Monitor control functions
void start_monitor(monitor_t *monitor);
void stop_monitor(monitor_t *monitor);

// Monitor command functions
void list_hunts_cmd(monitor_t *monitor);
void view_treasure_cmd(monitor_t *monitor);
void list_treasures_cmd(monitor_t *monitor);
void calculate_score_cmd(monitor_t *monitor);

#endif