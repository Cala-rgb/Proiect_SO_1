#ifndef COMMANDS_H
#define COMMANDS_H

#include "common.h"

// Log operations
error_code_t update_log_file(const char* hunt, const char* op, const char* tid, int failed);

// Treasure operations
error_code_t create_treasure(const char* hunt);
error_code_t view_treasure(const char* hunt_id, const char* treasure_id);
error_code_t list_treasures(const char* hunt_id);

// Hunt operations
error_code_t delete_hunt(const char* hunt_id);
error_code_t delete_treasure(const char* hunt_id, const char* treasure_id);
error_code_t create_hunt_files(const char* hunt_id);
error_code_t list_hunts(const char* path);

// File operations
error_code_t create_bin_file(const char* dir);
error_code_t create_log_file(const char* dir);

// Helper functions
error_code_t write_command_file(const char* command);
void print_error(const char* context, error_code_t error);

#endif // COMMANDS_H