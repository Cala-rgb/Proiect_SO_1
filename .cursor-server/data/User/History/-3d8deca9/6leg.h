#ifndef COMMANDS_H
#define COMMANDS_H

#include "common.h"

// Log operations
void update_log_file(char hunt[], char op[], char tid[], int failed);

// Treasure operations
void create_treasure(char *hunt);
void view_treasure(char hunt_id[], char treasure_id[]);
void list_treasures(char hunt_id[]);

// Hunt operations
void delete_hunt(char hunt_id[]);
void delete_treasure(char hunt_id[], char treasure_id[]);
void create_hunt_files(char hunt_id[]);
void list_hunts(char *path);

// File operations
int create_bin_file(char dir[]);
int create_log_file(char dir[]);

// Helper functions
error_code_t write_command_file(const char* command);
void print_error(const char* context, error_code_t error);

#endif // COMMANDS_H