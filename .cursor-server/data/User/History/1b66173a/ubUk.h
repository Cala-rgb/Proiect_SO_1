#ifndef COMMON_H
#define COMMON_H

#include <limits.h>

// Common structure for treasure data
typedef struct {
    char id[25];
    char user[25];
    double lat, lon;
    char clue[25];
    int value;
} treasure_t;

// Structure for monitor state
typedef struct {
    int pid;
    int status;  // 0 = stopped, 1 = running, 2 = stopping
    int pipe_fd[2];  // Pipe for stdout from monitor to main
} monitor_t;

// Common error codes
typedef enum {
    SUCCESS = 0,
    ERR_FILE_OPEN = -1,
    ERR_FILE_READ = -2,
    ERR_FILE_WRITE = -3,
    ERR_PIPE_CREATE = -4,
    ERR_FORK_FAILED = -5,
    ERR_EXEC_FAILED = -6,
    ERR_INVALID_ARGS = -7,
    ERR_DIR_NOT_FOUND = -8,
    ERR_DIR_CREATE = -9,
    ERR_FILE_CREATE = -10,
    ERR_FILE_DELETE = -11,
    ERR_DIR_DELETE = -12,
    ERR_PERMISSION_DENIED = -13
} error_code_t;

// Buffer sizes
#define BUFFER_SIZE 4096
#define COMMAND_SIZE PATH_MAX
#define PATH_SIZE PATH_MAX

// Helper functions
const char* error_to_string(error_code_t error);

#endif // COMMON_H 