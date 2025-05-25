#ifndef HUB_COMMANDS_H
#define HUB_COMMANDS_H

#include "common.h"
#include "handle_signals.h"

// Monitor control functions
void stop_monitor(monitor_t* monitor);
void start_monitor(monitor_t* monitor);

// Monitor command functions
void list_hunts_cmd(monitor_t* monitor);
void view_treasure_cmd(monitor_t* monitor);
void list_treasures_cmd(monitor_t* monitor);
void calculate_score_cmd(monitor_t* monitor);

// Helper functions
void read_from_pipe(monitor_t* monitor);
monitor_t* get_main_monitor(void);

#endif