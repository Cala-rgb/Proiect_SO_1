#ifndef HUB_COMMANDS_H
#define HUB_COMMANDS_H

#include "common.h"
#include "handle_signals.h"

// Monitor control functions
error_code_t stop_monitor(monitor_t* monitor);
error_code_t start_monitor(monitor_t* monitor);

// Monitor command functions
error_code_t list_hunts_cmd(monitor_t* monitor);
error_code_t view_treasure_cmd(monitor_t* monitor);
error_code_t list_treasures_cmd(monitor_t* monitor);
error_code_t calculate_score_cmd(monitor_t* monitor);

// Helper functions
error_code_t read_from_pipe(monitor_t* monitor);
monitor_t* get_main_monitor(void);

#endif